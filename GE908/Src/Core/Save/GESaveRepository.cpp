#include "GESaveRepository.h"
#include "GESaveSerializer.h"
#include <Windows.h>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace {
    std::filesystem::path defaultSaveDirectory() {
        wchar_t overrideDirectory[32768]{};
        const DWORD overrideLength = GetEnvironmentVariableW(L"GE908_SAVE_DIR", overrideDirectory, 32768);
        if (overrideLength > 0 && overrideLength < 32768) {
            return std::filesystem::path(overrideDirectory);
        }

        wchar_t localAppData[32768]{};
        const DWORD length = GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData, 32768);
        if (length > 0 && length < 32768) {
            return std::filesystem::path(localAppData) / L"GE908" / L"Saves";
        }
        return std::filesystem::temp_directory_path() / L"GE908" / L"Saves";
    }

    std::tm localTime(int64_t timestamp) {
        const std::time_t time = static_cast<std::time_t>(timestamp);
        std::tm result{};
        localtime_s(&result, &time);
        return result;
    }

    std::string formatIdTime(int64_t timestamp) {
        const std::tm time = localTime(timestamp);
        std::ostringstream stream;
        stream << std::put_time(&time, "%Y%m%d_%H%M%S");
        return stream.str();
    }

    std::string formatDisplayTime(int64_t timestamp) {
        const std::tm time = localTime(timestamp);
        std::ostringstream stream;
        stream << std::put_time(&time, "%Y-%m-%d %H:%M:%S");
        return stream.str();
    }

    bool newestFirst(const GESaveMetadata& lhs, const GESaveMetadata& rhs) {
        return lhs.updatedAt > rhs.updatedAt;
    }
}

GESaveRepository::GESaveRepository()
    : _saveDirectory(defaultSaveDirectory()) {
}

GESaveRepository::GESaveRepository(const std::filesystem::path& saveDirectory)
    : _saveDirectory(saveDirectory) {
}

bool GESaveRepository::ensureDirectory() const {
    std::error_code error;
    std::filesystem::create_directories(_saveDirectory, error);
    return !error;
}

std::filesystem::path GESaveRepository::pathFor(const std::string& id) const {
    return _saveDirectory / (id + ".gesave");
}

std::string GESaveRepository::createId(int64_t timestamp) const {
    const std::string baseId = "save_" + formatIdTime(timestamp);
    std::string id = baseId;
    int suffix = 2;
    while (std::filesystem::exists(pathFor(id))) {
        id = baseId + "_" + std::to_string(suffix++);
    }
    return id;
}

std::vector<GESaveMetadata> GESaveRepository::list() const {
    std::vector<GESaveMetadata> entries;
    if (!ensureDirectory()) return entries;

    std::error_code error;
    std::filesystem::directory_iterator iterator(_saveDirectory, error);
    const std::filesystem::directory_iterator end;
    while (!error && iterator != end) {
        const std::filesystem::directory_entry& entry = *iterator;
        if (entry.is_regular_file() && entry.path().extension() == ".gesave") {
            GESaveMetadata metadata;
            if (GESaveSerializer::readMetadata(entry.path(), metadata)) {
                metadata.id = entry.path().stem().string();
                entries.push_back(metadata);
            }
        }
        iterator.increment(error);
    }

    std::sort(entries.begin(), entries.end(), newestFirst);
    return entries;
}

bool GESaveRepository::save(GESaveRecord& record) const {
    if (!ensureDirectory()) return false;

    const int64_t now = static_cast<int64_t>(std::time(nullptr));
    if (record.metadata.id.empty()) {
        record.metadata.id = createId(now);
    }
    record.metadata.displayName = formatDisplayTime(now);
    record.metadata.updatedAt = now;
    record.metadata.mapMode = record.snapshot.map.mapMode;
    record.metadata.levelTimeRemaining = record.snapshot.map.levelTimeRemaining;
    record.metadata.playerHp = record.snapshot.player.hp;

    const std::filesystem::path destination = pathFor(record.metadata.id);
    const std::filesystem::path temporary = destination.wstring() + L".tmp";
    if (!GESaveSerializer::write(temporary, record)) {
        std::error_code error;
        std::filesystem::remove(temporary, error);
        return false;
    }

    // Replace saves atomically.
    if (!MoveFileExW(
        temporary.c_str(),
        destination.c_str(),
        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        std::error_code error;
        std::filesystem::remove(temporary, error);
        return false;
    }
    return true;
}

bool GESaveRepository::load(const std::string& id, GESaveRecord& record) const {
    if (!GESaveSerializer::read(pathFor(id), record)) return false;
    record.metadata.id = id;
    return true;
}

bool GESaveRepository::remove(const std::string& id) const {
    std::error_code error;
    return std::filesystem::remove(pathFor(id), error) && !error;
}

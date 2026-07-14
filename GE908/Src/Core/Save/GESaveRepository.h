#pragma once

#include <filesystem>
#include <vector>
#include "GESaveTypes.h"

// Manages save files and metadata.
class GESaveRepository {
private:
    std::filesystem::path _saveDirectory;

    bool ensureDirectory() const;
    std::filesystem::path pathFor(const std::string& id) const;
    std::string createId(int64_t timestamp) const;

public:
    GESaveRepository();
    explicit GESaveRepository(const std::filesystem::path& saveDirectory);

    std::vector<GESaveMetadata> list() const;
    bool save(GESaveRecord& record) const;
    bool load(const std::string& id, GESaveRecord& record) const;
    bool remove(const std::string& id) const;

    const std::filesystem::path& getSaveDirectory() const { return _saveDirectory; }
};

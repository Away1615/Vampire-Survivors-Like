#pragma once

#include <Windows.h>
#include <string>

namespace GEPath {
    inline bool isDirectory(const std::wstring& path) {
        const DWORD attributes = GetFileAttributesW(path.c_str());
        return attributes != INVALID_FILE_ATTRIBUTES
            && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    inline bool tryUseContentRoot(const std::wstring& path) {
        if (!isDirectory(path + L"\\Src\\Assets")) {
            return false;
        }
        return SetCurrentDirectoryW(path.c_str()) != 0;
    }

    inline bool initializeContentRoot() {
        wchar_t executablePath[MAX_PATH] = {};
        const DWORD pathLength = GetModuleFileNameW(nullptr, executablePath, MAX_PATH);
        if (pathLength == 0 || pathLength == MAX_PATH) return false;

        std::wstring directory(executablePath, pathLength);
        const std::size_t filenameSeparator = directory.find_last_of(L"\\/");
        if (filenameSeparator == std::wstring::npos) return false;
        directory.resize(filenameSeparator);

        while (!directory.empty()) {
            if (tryUseContentRoot(directory)) return true;
            if (tryUseContentRoot(directory + L"\\GE908")) return true;

            const std::size_t separator = directory.find_last_of(L"\\/");
            if (separator == std::wstring::npos || separator <= 2) break;
            directory.resize(separator);
        }
        return false;
    }
}

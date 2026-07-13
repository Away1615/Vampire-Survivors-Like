#pragma once

#include <filesystem>
#include "GESaveTypes.h"

class GESaveSerializer {
public:
    static bool write(const std::filesystem::path& filename, const GESaveRecord& record);
    static bool readMetadata(const std::filesystem::path& filename, GESaveMetadata& metadata);
    static bool read(const std::filesystem::path& filename, GESaveRecord& record);
};

#pragma once

#include <algorithm>
#include <string>

namespace modloader {
    struct ModInfo {
        std::string id{};
        std::string version{};
        size_t versionLong{};

        ModInfo(std::string_view id, std::string_view version, size_t versionLong) : id(id), version(version), versionLong(versionLong) {}
    };
}

int mkpath(std::string_view file_path);
std::string readfile(std::string_view filename);
bool writefile(std::string_view filename, std::string_view text);
bool fileexists(std::string_view filename);

inline std::string get_config_path(modloader::ModInfo const& info) {
    return info.id + ".json";
}

#pragma once

#include <string>

namespace modloader {
    struct ModInfo {
        std::string id{};
        std::string version{};
        size_t versionLong{};

        ModInfo(std::string_view id, std::string_view version, size_t versionLong) : id(id), version(version), versionLong(versionLong) {}
    };
}

inline std::string get_config_path(modloader::ModInfo const& info) {
    return info.id + ".json";
}

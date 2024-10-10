#pragma once

#include <string>

#include "config-utils.hpp"

namespace modloader {
    struct ModInfo {
        std::string id{};
        std::string version{};
        size_t versionLong{};

        ModInfo(std::string_view id, std::string_view version, size_t versionLong) : id(id), version(version), versionLong(versionLong) {}
    };
}

namespace Configuration {
    static inline std::string getConfigFilePath(modloader::ModInfo const& info) {
        return info.id + ".json";
    }
}

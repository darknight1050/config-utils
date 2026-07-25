#include "test.hpp"

#include <sstream>
#include <io.h>

static modloader::ModInfo modInfo = {"test", "0.0.1", 0};

int mkpath(std::string_view file_path) {
    std::error_code e;
    std::filesystem::create_directories(file_path, e);
    return e.value();
}

std::string readfile(std::string_view filename) {
    std::ifstream t(filename.data());
    if (!t.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

bool writefile(std::string_view filename, std::string_view text) {
    std::ofstream t(filename.data());
    if (t.is_open()) {
        t << text;
        return true;
    }
    return false;
}

bool fileexists(std::string_view filename) {
    return access(filename.data(), W_OK | R_OK) != -1;
}

int main(int argc, char** args) {
    getModConfig().Init(modInfo);
    RunTest();
}

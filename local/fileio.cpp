#include "fileio.hpp"

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>

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

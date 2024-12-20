#pragma once

#include <string>
#include <string_view>

std::string readfile(std::string_view filename);

bool writefile(std::string_view filename, std::string_view text);

bool fileexists(std::string_view filename);

#include "test.hpp"

#include <sstream>

static modloader::ModInfo modInfo = {"test", "0.0.1", 0};

int main(int argc, char** args) {
    getModConfig().Init(modInfo);
    RunTest();
}

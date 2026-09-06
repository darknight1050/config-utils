#include "test.hpp"

#include <sstream>

static modloader::ModInfo modInfo = {MOD_ID, VERSION, 0};

extern "C" void setup(CModInfo& info) {
    info.id = "config-utils-test";
    info.version = VERSION;
    info.version_long = 0;

    modInfo.assign(info);

    // Init/Load Config
    getModConfig().Init(modInfo);
}

extern "C" void load() {
    RunTest();
}

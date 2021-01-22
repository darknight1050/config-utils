#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

#include "TestConfig.hpp"

static ModInfo modInfo;

Logger& getLogger() {
    static auto logger = new Logger(modInfo);
    return *logger;
}

Configuration& getConfig() {
    static Configuration configuration(modInfo);
    return configuration;
}

DEFINE_CONFIG(TestConfig);

extern "C" void setup(ModInfo& info) {
    info.id = "ConfigUtilsTest";
    info.version = VERSION;
    modInfo = info;
    getConfig().Load();
    getTestConfig().Init(&getConfig());
}

extern "C" void load() {
    il2cpp_functions::Init();
    getLogger().info("TestBool: %d", getTestConfig().TestBool.GetValue());
    getLogger().info("TestInt: %d", getTestConfig().TestInt.GetValue());
    getLogger().info("TestFloat: %f", getTestConfig().TestFloat.GetValue());
    getLogger().info("TestString: %s", getTestConfig().TestString.GetValue().c_str());
    getTestConfig().TestBool.SetValue(false);
    getTestConfig().TestInt.SetValue(1);
    getTestConfig().TestFloat.SetValue(1337.0f);
    getTestConfig().TestString.SetValue("Changed");
}
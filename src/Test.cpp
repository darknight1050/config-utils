#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

//Include the Config header
#include "ModConfig.hpp"

#include <sstream>

static modloader::ModInfo modInfo = {"", VERSION, 0};

Logger& getLogger() {
    static auto logger = new Logger(modInfo);
    return *logger;
}

extern "C" void setup(CModInfo& info) {
    info.id = "config-utils-test";
    info.version = VERSION;
    info.version_long = 0;

    modInfo.assign(info);

    //Init/Load Config
    getModConfig().Init(modInfo);
}

extern "C" void load() {
    il2cpp_functions::Init();
    //Access Config Values
    getModConfig().SampleBool.AddChangeEvent(
        [] (bool newValue) {
            getLogger().info("SampleBool ValueChange: %d", newValue);
        }
    );
    getLogger().info("SampleBool: %d", getModConfig().SampleBool.GetValue());
    getLogger().info("SampleInt: %d", getModConfig().SampleInt.GetValue());
    getLogger().info("SampleFloat: %f", getModConfig().SampleFloat.GetValue());
    getLogger().info("SampleString: %s", getModConfig().SampleString.GetValue().c_str());
    Subclass subclass = getModConfig().SampleSubclass.GetValue();
    getLogger().info("SampleSubclass: SubDouble: %f, SubBool: %d", subclass.SubDouble, subclass.SubBool);
    std::stringstream ss;
    for(auto i : getModConfig().SampleVector.GetValue())
        ss << i << " ";
    getLogger().info("SampleVector: %s", ss.str().c_str());
    getModConfig().SampleBool.SetValue(false);
    getModConfig().SampleInt.SetValue(1);
    getModConfig().SampleFloat.SetValue(1337.0f);
    getModConfig().SampleString.SetValue("Changed");
    Subclass newSubclass;
    newSubclass.SubDouble = 1.414;
    getModConfig().SampleSubclass.SetValue(newSubclass);
    getModConfig().SampleVector.SetValue({1, 2, 3, 4});
}
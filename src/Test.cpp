#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

//Include the Config header
#include "ModConfig.hpp"

#include <sstream>

static modloader::ModInfo modInfo = {"", VERSION, 0};

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
            ConfigUtils::Logger.info("SampleBool ValueChange: %d", newValue);
        }
    );
    ConfigUtils::Logger.info("SampleBool: %d", getModConfig().SampleBool.GetValue());
    ConfigUtils::Logger.info("SampleInt: %d", getModConfig().SampleInt.GetValue());
    ConfigUtils::Logger.info("SampleFloat: %f", getModConfig().SampleFloat.GetValue());
    ConfigUtils::Logger.info("SampleString: %s", getModConfig().SampleString.GetValue().c_str());
    Subclass subclass = getModConfig().SampleSubclass.GetValue();
    ConfigUtils::Logger.info("SampleSubclass: SubDouble: %f, SubBool: %d", subclass.SubDouble, subclass.SubBool);
    std::stringstream ss;
    for(auto i : getModConfig().SampleVector.GetValue())
        ss << i << " ";
    ConfigUtils::Logger.info("SampleVector: %s", ss.str().c_str());
    getModConfig().SampleBool.SetValue(false);
    getModConfig().SampleInt.SetValue(1);
    getModConfig().SampleFloat.SetValue(1337.0f);
    getModConfig().SampleString.SetValue("Changed");
    Subclass newSubclass;
    newSubclass.SubDouble = 1.414;
    getModConfig().SampleSubclass.SetValue(newSubclass);
    getModConfig().SampleVector.SetValue({1, 2, 3, 4});
}

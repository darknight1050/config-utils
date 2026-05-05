#pragma once

#include "config-utils.hpp"

DECLARE_JSON_STRUCT(Subclass) {
    VALUE(double, SubDouble);
    VALUE_DEFAULT(bool, SubBool, true);
};

DECLARE_CONFIG(ModConfig) {
    CONFIG_VALUE(SampleBool, bool, "SampleBool", true);
    CONFIG_VALUE(SampleInt, int, "SampleInt", 1337);
    CONFIG_VALUE(SampleFloat, float, "SampleFloat", 10.0f);
    CONFIG_VALUE(SampleString, std::string, "SampleString", "hi");
    CONFIG_VALUE(SampleSubclass, Subclass, "SampleSubclass", {});
    CONFIG_VALUE(SampleVector, std::vector<int>, "SampleVector", std::vector<int>({1, 2, 3}));
};

inline void RunTest() {
    getModConfig().SampleBool.AddChangeEvent([](bool newValue) { ConfigUtils::Logger.info("SampleBool ValueChange: {}", newValue); });
    ConfigUtils::Logger.info("SampleBool: {}", getModConfig().SampleBool.GetValue());
    ConfigUtils::Logger.info("SampleInt: {}", getModConfig().SampleInt.GetValue());
    ConfigUtils::Logger.info("SampleFloat: {}", getModConfig().SampleFloat.GetValue());
    ConfigUtils::Logger.info("SampleString: {}", getModConfig().SampleString.GetValue().c_str());
    Subclass subclass = getModConfig().SampleSubclass.GetValue();
    ConfigUtils::Logger.info("SampleSubclass: SubDouble: {}, SubBool: {}", subclass.SubDouble, subclass.SubBool);
    std::stringstream ss;
    for (auto i : getModConfig().SampleVector.GetValue())
        ss << i << " ";
    ConfigUtils::Logger.info("SampleVector: {}", ss.str().c_str());
    getModConfig().SampleBool.SetValue(false);
    getModConfig().SampleInt.SetValue(1);
    getModConfig().SampleFloat.SetValue(1337.0f);
    getModConfig().SampleString.SetValue("Changed");
    Subclass newSubclass;
    newSubclass.SubDouble = 1.414;
    getModConfig().SampleSubclass.SetValue(newSubclass);
    getModConfig().SampleVector.SetValue({1, 2, 3, 4});
}

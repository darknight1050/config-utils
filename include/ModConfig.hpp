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

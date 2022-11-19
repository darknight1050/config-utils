#pragma once
#include "config-utils.hpp"

DECLARE_CONFIG(ModConfig,
    CONFIG_VALUE(SampleBool, bool, "SampleBool", true);
    CONFIG_VALUE(SampleInt, int, "SampleInt", 1337);
    CONFIG_VALUE(SampleFloat, float, "SampleFloat", 10.0f);
    CONFIG_VALUE(SampleString, std::string, "SampleString", "hi");
)

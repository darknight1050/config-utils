#pragma once
#include "config-utils.hpp"

DECLARE_CONFIG(ModConfig,

    CONFIG_VALUE(SampleBool, bool, "SampleBool", true);
    CONFIG_VALUE(SampleInt, int, "SampleInt", 1337);
    CONFIG_VALUE(SampleFloat, float, "SampleFloat", 10.0f);
    CONFIG_VALUE(SampleString, std::string, "SampleString", "hi");

    CONFIG_INIT_FUNCTION(
        CONFIG_INIT_VALUE(SampleBool);
        CONFIG_INIT_VALUE(SampleInt);
        CONFIG_INIT_VALUE(SampleFloat);
        CONFIG_INIT_VALUE(SampleString);
    )
)

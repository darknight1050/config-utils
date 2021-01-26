#pragma once
#include "config-utils.hpp"

DECLARE_CONFIG(ModConfig,

    DECLARE_VALUE(SampleBool, bool, "SampleBool", true);
    DECLARE_VALUE(SampleInt, int, "SampleInt", 1337);
    DECLARE_VALUE(SampleFloat, float, "SampleFloat", 10.0f);
    DECLARE_VALUE(SampleString, std::string, "SampleString", "hi");

    INIT_FUNCTION(
        INIT_VALUE(SampleBool);
        INIT_VALUE(SampleInt);
        INIT_VALUE(SampleFloat);
        INIT_VALUE(SampleString);
    )
)

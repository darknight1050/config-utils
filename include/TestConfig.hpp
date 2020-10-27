#pragma once
#include "ConfigUtils.hpp"

DECLARE_CONFIG(TestConfig,

    DECLARE_VALUE(TestBool, bool, "TestBool", true);
    DECLARE_VALUE(TestInt, int, "TestInt", 1337);
    DECLARE_VALUE(TestFloat, float, "TestFloat", 10.0f);
    DECLARE_VALUE(TestString, std::string, "TestString", "hi");

    INIT_FUNCTION(
        INIT_VALUE(TestBool);
        INIT_VALUE(TestInt);
        INIT_VALUE(TestFloat);
        INIT_VALUE(TestString);
    )
)


$rapidjson = "C:/Path/vcpkg/packages/rapidjson_x64-mingw-dynamic/include"
$rapidjson_macros = "rapidjson-macros/shared"
$fmt = "C:\Path\vcpkg\packages\fmt_x64-windows\include"

g++ -std=c++20 -DRAPIDJSON_MACROS_GCC_TEST -DFMT_HEADER_ONLY -DCONFIG_UTILS_GCC_TEST -I"$rapidjson" -I"$rapidjson_macros" -I"$fmt" -Iinclude -Ishared ./local/*.cpp -o cfgutilstest.exe

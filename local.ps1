$triplet = "x64-windows"

if ($null -eq $env:VCPKG_ROOT -or (Test-Path $env:VCPKG_ROOT) -eq $false) {
    Write-Error "VCPKG_ROOT not found"
}

$rapidjson = Join-Path $env:VCPKG_ROOT "installed/$triplet/include/rapidjson"
$fmt = Join-Path $env:VCPKG_ROOT "installed/$triplet/include/fmt"
# so that <fmt/x.h> works
$fmt_upper = Join-Path $env:VCPKG_ROOT "installed/$triplet/include"

if ((Test-Path $rapidjson) -eq $false) {
    Write-Error "rapidjson not found"
}
if ((Test-Path $fmt) -eq $false) {
    Write-Error "fmt not found"
}

$rapidjson_macros = "rapidjson-macros/shared"

g++ -std=c++20 -DRAPIDJSON_MACROS_GCC_TEST -DFMT_HEADER_ONLY -DCONFIG_UTILS_GCC_TEST -I"$rapidjson" -I"$rapidjson_macros" -I"$fmt" -I"$fmt_upper" -Iinclude -Ishared ./local/*.cpp -o cfgutilstest.exe

$triplet = "x64-windows"

if ($null -eq $env:VCPKG_ROOT -or (Test-Path $env:VCPKG_ROOT) -eq $false) {
    Write-Error "VCPKG_ROOT not found"
}

$fmt = Join-Path $env:VCPKG_ROOT "installed/$triplet/include/fmt"

if ((Test-Path $fmt) -eq $false) {
    Write-Error "vcpkg fmt not found"
}

# so that <fmt/x.h> works
$upper = Join-Path $env:VCPKG_ROOT "installed/$triplet/include"

$rfl = "./local/reflectcpp"

g++ -std=c++23 -o cfgutilstest.exe -fconcepts-diagnostics-depth=2 `
    -DFMT_HEADER_ONLY -DREFLECT_CPP_C_ARRAYS_OR_INHERITANCE `
    -I"$fmt" -I"$upper" -I"$rfl/include" -Iinclude -Ishared -Ilocal `
    "$rfl/src/reflectcpp.cpp" "$rfl/src/reflectcpp_json.cpp" "$rfl/src/yyjson.c" `
    ./local/test.cpp

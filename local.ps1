$triplet = "x64-windows"

if ($null -eq $env:VCPKG_ROOT -or (Test-Path $env:VCPKG_ROOT) -eq $false) {
    Write-Error "VCPKG_ROOT not found"
}

$fmt = Join-Path $env:VCPKG_ROOT "installed/$triplet/include/fmt"
# so that <fmt/x.h> works
$fmt_upper = Join-Path $env:VCPKG_ROOT "installed/$triplet/include"

if ((Test-Path $fmt) -eq $false) {
    Write-Error "vcpkg fmt not found"
}

g++ -std=c++23 -DFMT_HEADER_ONLY -I"$fmt" -I"$fmt_upper" -Iinclude -Ishared -Ilocal ./local/test.cpp -o cfgutilstest.exe

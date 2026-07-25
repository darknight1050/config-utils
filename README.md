# ConfigUtils

## Local Test

- Make sure gcc is on path and VCPKG_ROOT is set.
- Run `git checkout tags/v0.25.0` in `local/reflectcpp`.
- Run `vcpkg install fmt`.
- Update the path in `local.ps1` if needed.
- Run `local.ps1` or `qpm s local` to build.
- Run `cfgutilstest.exe` or `qpm s test` to run the test executable.

## Credits

- [darknight1050](https://github.com/darknight1050/) for the original creation
- [kodenamekrak](https://github.com/kodenamekrak) for making [reflectcpp](https://github.com/getml/reflect-cpp) available on qpm
- [Metalit](https://github.com/Metalit) for maintenance and [rapidjson-macros](https://github.com/Metalit/RapidjsonMacros) while it was used

# Building for x86-windows

Install and build 32-bit microsoft/detours, link it to trcapi.cpp.

* vcpkg install detours:x86-windows
* add vcpkg toolchain to cmake
* build project

# Attaching to LeagueClientUx

Be sure to run elevated command prompt, and run `main.exe`.

# How it Works

* main.exe tells league client to run dll through LoadLibrary
* dll searches for cef_parse_url function address
* dll re-routes all cef_parse_url api calls to dummy
* dummy does something, then returns back original cef_parse_url

Note that league client runs with admin rights, and the platform is 32-bit.
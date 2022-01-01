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

# Notes

CppCon all about dll: https://www.youtube.com/watch?v=JPQWQfDhICA

Detours:  
https://github.com/microsoft/Detours/wiki/Using-Detours  
https://github.com/microsoft/Detours/wiki/Samples

https://www.codeproject.com/Articles/30140/API-Hooking-with-MS-Detours#Remote

cef_parse_url:  
https://github.com/chromiumembedded/cef/blob/master/include/capi/cef_parser_capi.h

cef_urlparts_t:  
https://magpcss.org/ceforum/apidocs/projects/(default)/_cef_urlparts_t.html
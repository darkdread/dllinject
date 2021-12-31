#undef UNICODE
#include <cstdio>
#include <windows.h>
#include "detours.h"

#include <fstream>
#include <ostream>
#include <iostream>
#include <codecvt>
#include <locale>
#include <string>

#define LOG_FILE "MyLogFile.log"

typedef struct _cef_string_wide_t {
  wchar_t* str;
  size_t length;
  void (*dtor)(wchar_t* str);
} cef_string_wide_t;

typedef _cef_string_wide_t cef_string_t;

typedef struct _cef_urlparts_t {
  ///
  // The complete URL specification.
  ///
  cef_string_t spec;

  ///
  // Scheme component not including the colon (e.g., "http").
  ///
  cef_string_t scheme;

  ///
  // User name component.
  ///
  cef_string_t username;

  ///
  // Password component.
  ///
  cef_string_t password;

  ///
  // Host component. This may be a hostname, an IPv4 address or an IPv6 literal
  // surrounded by square brackets (e.g., "[2001:db8::1]").
  ///
  cef_string_t host;

  ///
  // Port number component.
  ///
  cef_string_t port;

  ///
  // Path component including the first slash following the host.
  ///
  cef_string_t path;

  ///
  // Query string component (i.e., everything following the '?').
  ///
  cef_string_t query;
} cef_urlparts_t;

int __cdecl MyFunc(const cef_string_t* url, struct _cef_urlparts_t* parts);

LPVOID CefParseUrlAddr;

typedef int (*cef_parse_url)(const cef_string_t* url, struct _cef_urlparts_t* parts);
// static int (*TrueCefParseUrl)(const cef_string_t* url, struct _cef_urlparts_t* parts);

cef_parse_url TrueCefParseUrl;

// Returns directory of loaded dll
std::string GetCurrentDirectoryB()
{
  HMODULE hm = NULL;

	char buffer[MAX_PATH];
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
    (LPCSTR) &MyFunc, &hm);

  GetModuleFileNameA(hm, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	
	return std::string(buffer).substr(0, pos);
}

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
  if (DetourIsHelperProcess()) {
    return TRUE;
  }
  // char* newReason = new char[MAX_PATH];
  // sprintf_s(newReason, MAX_PATH, "%lu", Reason);
  // MessageBox(0, newReason, "TITLE", MB_OK);
  switch(Reason)
  {
  case DLL_PROCESS_ATTACH:
    {
      CefParseUrlAddr = (LPVOID)GetProcAddress(GetModuleHandle("libcef.dll"),
        "cef_parse_url");

      // Save original function address
      if (CefParseUrlAddr != NULL){
        TrueCefParseUrl = (cef_parse_url) CefParseUrlAddr;
      }

      DisableThreadLibraryCalls(hDLL);
      DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());
      // I still don't know why attaching original function creates a loop
      // whereby process -> dummy -> cef_parse_url -> dummy -> ...
      // DetourAttach(&(PVOID&)CefParseUrlAddr, MyFunc);
      DetourAttach(&(PVOID&)TrueCefParseUrl, MyFunc);
      DetourTransactionCommit();
    }
    break;
  case DLL_PROCESS_DETACH:
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)TrueCefParseUrl, MyFunc);
    DetourTransactionCommit();
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    break;
  }
  return TRUE;
}

static void appendLineToFile(std::string filepath, std::string line)
{
    std::ofstream file;
    //can't enable exception now because of gcc bug that raises ios_base::failure with useless message
    //file.exceptions(file.exceptions() | std::ios::failbit);
    file.open(filepath, std::ios::out | std::ios::app);

    //make sure write fails with exception if something is wrong
    file.exceptions(file.exceptions() | std::ios::failbit | std::ifstream::badbit);

    file << line << std::endl;
}

int __cdecl MyFunc(const cef_string_t* url, struct _cef_urlparts_t* parts)
{
  appendLineToFile(GetCurrentDirectoryB() + "/" + LOG_FILE, std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(url->str));
  
  return TrueCefParseUrl(url, parts);
}
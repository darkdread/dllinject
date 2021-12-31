#undef UNICODE
#include <vector>
#include <string>
#include <windows.h>
#include <Tlhelp32.h>
using std::vector;
using std::string;

int main(void)
{
  printf("HELLO");
  vector<string>processNames;
  PROCESSENTRY32 pe32;
  pe32.dwSize = sizeof(PROCESSENTRY32);
  HANDLE hTool32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  BOOL bProcess = Process32First(hTool32, &pe32);
  if(bProcess == TRUE)
  {
    while((Process32Next(hTool32, &pe32)) == TRUE){
      processNames.push_back(pe32.szExeFile);
      if(strcmp(pe32.szExeFile, "LeagueClientUx.exe") == 0)
      {
        printf("PID: %lu\n", pe32.th32ProcessID);
        char* DirPath = new char[MAX_PATH];
        char* FullPath = new char[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, DirPath);

        char* FilePath = new char[MAX_PATH];
        GetModuleFileName(NULL, FilePath, MAX_PATH);
        size_t pos = ((std::string) FilePath).find_last_of("\\");
        std::string newPath = ((std::string) FilePath).substr(0, pos);

        sprintf_s(FullPath, MAX_PATH, "%s\\mydll.dll", newPath.c_str());
        // printf("%s\n", FullPath);

        HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD    | PROCESS_VM_OPERATION    |
            PROCESS_VM_WRITE, FALSE, pe32.th32ProcessID);
        LPVOID LoadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"),
            "LoadLibraryA");
        LPVOID LLParam = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(FullPath),
            MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        WriteProcessMemory(hProcess, LLParam, FullPath, strlen(FullPath), NULL);
        printf("%lu\n", GetLastError());
        CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddr,
          LLParam, NULL, NULL);
        CloseHandle(hProcess);
        delete [] DirPath;
        delete [] FullPath;
      }
    }
  }
  for (size_t i = 0; i < processNames.size(); i++)
  {
    // printf("%s", processNames[i].c_str());
  }
  CloseHandle(hTool32);
  printf("BYE");
  return 0;
}
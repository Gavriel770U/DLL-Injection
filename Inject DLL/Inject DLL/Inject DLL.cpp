/*
    Author: Gavriel Linoy.
    Big Credits To: Barak Gonen and Magshimim's presentation
*/

#include <Windows.h>
#include <iostream>

/* * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*   IMPORTANT NOTE FOR 32 BITS, NOW I USE 64 BITS:
*   When I wrote the project in x86 mode,
*   and it's 32 bit,
*   because x64 (64 bits) didn't work for me.
*   The path for 32 bits notepad.exe is:
*   C:\Windows\SysWOW64\notepad.exe
* 
*   
*   ! ESSENTIAL NOTE:
*   Also you have to change the full paths!                  
* * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

// CHANGE THIS!
#define TARGET_PID <PID_OF_PROCESS_TO_INJECT>

#define DLL_PATH <PATH_OF_DLL>
#define MAX_PATH_LEN 2000

#define INC 1
#define ERROR_CODE 1

int main()
{
    // Get full path of DLL to inject
    CHAR path[MAX_PATH_LEN + 1] = { 0 };

    DWORD pathLen = GetFullPathNameA(DLL_PATH, MAX_PATH_LEN, path, NULL);
 
    // Open remote process
    HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, TARGET_PID);
    if (!proc)
    {
        std::cerr << "Failed to open process: " << GetLastError() << std::endl;
        return ERROR_CODE;
    }

    // Get a pointer to memory location in remote process,
    // big enough to store DLL path
    LPVOID memAddr = VirtualAllocEx(proc, NULL, pathLen + INC, MEM_COMMIT, PAGE_READWRITE);
    if (!memAddr)
    {
        std::cerr << "Failed to allocate memory in remote process: " << GetLastError() << std::endl;
        CloseHandle(proc);
        return ERROR_CODE;
    }

    // Write DLL name to remote process memory
    if (!WriteProcessMemory(proc, memAddr, path, pathLen + INC, NULL))
    {
        std::cerr << "Failed to write to remote process memory: " << GetLastError() << std::endl;
        VirtualFreeEx(proc, memAddr, 0, MEM_RELEASE);
        CloseHandle(proc);
        return ERROR_CODE;
    }

    // Get address of LoadLibraryA function in kernel32.dll
    LPVOID addrLoadLibrary = (LPVOID)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
    if (!addrLoadLibrary)
    {
        std::cerr << "Failed to get address of LoadLibraryA: " << GetLastError() << std::endl;
        VirtualFreeEx(proc, memAddr, 0, MEM_RELEASE);
        CloseHandle(proc);
        return ERROR_CODE;
    }

    // Open remote thread, while executing LoadLibrary
    // with parameter DLL name, will trigger DLLMain
    HANDLE hRemote = CreateRemoteThread(proc, NULL, 0, (LPTHREAD_START_ROUTINE)addrLoadLibrary, memAddr, 0, NULL);
    if (!hRemote)
    {
        std::cerr << "Failed to create remote thread: " << GetLastError() << std::endl;
        VirtualFreeEx(proc, memAddr, 0, MEM_RELEASE);
        CloseHandle(proc);
        return ERROR_CODE;
    }

    WaitForSingleObject(hRemote, INFINITE);
    VirtualFreeEx(proc, memAddr, 0, MEM_RELEASE);
    CloseHandle(hRemote);
    CloseHandle(proc);

    return 0;
}

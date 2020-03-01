#include <iostream>
#include <windows.h>
#include <TlHelp32.h>


DWORD GetProcId(const char *procName) {
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap && hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry)) {

            do {
                if (!_stricmp(procEntry.szExeFile, procName)) { // string compare case INsensitive , returns 0 if true
                    procId = procEntry.th32ProcessID;
                    break;
                }

            } while (Process32Next(hSnap, &procEntry));
        }

    }
    CloseHandle(hSnap);
    return procId;
}


int main() {
    const char *dllPath = R"(C:\Users\marco\Desktop\New folder (2)\dllCheat.dll)";
    const char *procName = "ac_client.exe";

    DWORD procId = 0;

    std::printf("Simple injector - looking for process name: %s\n",procName);

    do {
        procId = GetProcId(procName);
        Sleep(30);
    } while (!procId);

    std::printf("FOUND PROCESS, INJECTING");

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, procId);

    if (hProc && hProc != INVALID_HANDLE_VALUE) {

        // allocate memory in external process, MAX PATH is for our dllPath
        void *loc = VirtualAllocEx(hProc, nullptr, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        if (loc) {
            WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, nullptr);
        }

        // creates remote thread in target process and loads DLL
        HANDLE hThread = CreateRemoteThread(hProc, nullptr, 0, (LPTHREAD_START_ROUTINE) LoadLibraryA, loc, 0, nullptr);

        if (hThread) {
            CloseHandle(hThread);
        }

    }

    CloseHandle(hProc);

    return 0;
}

#include "pch.h"
#include "RemoteThreadInject.h"

CRemoteThreadInject::CRemoteThreadInject()
{
}

CRemoteThreadInject::~CRemoteThreadInject()
{
}

void TcharToChar(const TCHAR* tchar, char* _char)
{
    int iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

bool CRemoteThreadInject::InjectorDLL(TCHAR* szPath, DWORD dwPid)
{
    HANDLE hProcess = GetProcessHandle(dwPid);
    if (hProcess == NULL)
    {
        return false;
    }

    LPVOID lpAdd = NULL;
    lpAdd = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (lpAdd == NULL)
    {
        return false;
    }
    char szTmpPath[MAX_PATH] = { 0 };
    TcharToChar(szPath, szTmpPath);
    if (!WriteProcessMemory(hProcess, lpAdd, szTmpPath, strlen(szTmpPath) + 1, NULL))
    {
        VirtualFreeEx(hProcess, lpAdd, MAX_PATH, MEM_RELEASE);
        return false;
    }

    HMODULE hModule = GetModuleHandle(L"kernel32.dll");

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryA"), lpAdd, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, lpAdd, MAX_PATH, MEM_RELEASE);

    return true;
}

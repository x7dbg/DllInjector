#pragma once
class CInjector
{
public:
    CInjector() {};
    virtual ~CInjector() {};
    virtual bool InjectorDLL(TCHAR* szPath, DWORD dwPid) = 0;
    inline bool IsCorrectTargetArchitecture(HANDLE hProc)
    {
        BOOL bTarget = FALSE;
        if (!IsWow64Process(hProc, &bTarget))
        {
            return false;
        }

        BOOL bHost = FALSE;
        IsWow64Process(GetCurrentProcess(), &bHost);
        return (bTarget == bHost);
    }
    inline HANDLE GetProcessHandle(DWORD dwPid)
    {
        TOKEN_PRIVILEGES priv = { 0 };
        HANDLE hToken = NULL;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        {
            priv.PrivilegeCount = 1;
            priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid))
                AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL);

            CloseHandle(hToken);
        }

        return OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
    }
};


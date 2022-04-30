#pragma once
#include "Injector.h"
class CRemoteThreadInject : public CInjector
{
public:
    CRemoteThreadInject();
    virtual ~CRemoteThreadInject();

    virtual bool InjectorDLL(TCHAR *szPath,DWORD dwPid);
};


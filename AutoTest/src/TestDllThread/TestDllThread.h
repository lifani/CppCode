// TestDllThread.h
// author   :       Li Fa Ni
// Date     :       2013-02-26
// Ver      :       1.0

#ifndef _TESTDLLTHREAD_H_
#define _TESTDLLTHREAD_H_

#include "../common/BaseThread.h"
#include "../common/DllInterface.h"

class DLL_API CTestDllThread : public CBaseThread
{
public:
    CTestDllThread(ThreadTag& threadTag, const string& strAppPath,
        const string& strIn, const string& strResultDir);
    
    virtual ~CTestDllThread(void);

private:

    void Run();

    bool EndThreadNormal(DWORD ulTimes = 10 );

    bool loadLibrary();

    void Destory();

private:

    ThreadTag& m_threadTag;
    const string& m_strAppPath;
    const string& m_strInputPath;
    const string& m_strResultDir;

    HMODULE m_hModule;
    CDllInterface* m_pDll;
};

#endif

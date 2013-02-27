// testDllBox.h
// author   :       Li Fa Ni
// Date     :       2013-02-26
// Ver      :       1.0

#ifndef _TESTDLLBOX_H_
#define _TESTDLLBOX_H_

#include "../../include/typedef.h"
#include "../common/BaseThread.h"
#include "../TestDllThread/TestDllThread.h"

// 线程单元
typedef struct _ThreadElement
{
    HANDLE handle;
    unsigned int uThreadId;

    CTestDllThread* pTestDllThread;

    vector<ThreadTag>::iterator itr;
}ThreadElement;

class DLL_API CTestDllBox : public CBaseThread
{
public:

    static CTestDllBox* Instance();

    CTestDllBox(void);

    ~CTestDllBox(void);

    bool StartTest(vector<ThreadTag>* pVThreadTag, const string& strAppPath,
        const string& strIn, const string& strResultDir );

    void EndTest();

    void Run();
private:

    bool EndThreadNormal(DWORD ulTimes);

    void Destory();

private:

    static CTestDllBox* m_TestDllBox;

    vector<ThreadTag>* m_pVThreadTag;
    string m_strAppPath;
    string m_strIn;
    string m_strResultDir;

    bool m_bRunning;

    HANDLE m_hThread;
    unsigned int m_uThreadId;

    vector<ThreadElement*>* m_pVThread;
};

#endif

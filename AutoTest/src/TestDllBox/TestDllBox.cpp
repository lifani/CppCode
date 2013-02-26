// testDllBox.cpp
// author   :       Li Fa Ni
// Date     :       2013-02-26
// Ver      :       1.0

#include "TestDllBox.h"

CTestDllBox* CTestDllBox::Instance()
{
    if (NULL == m_TestDllBox)
    {
        m_TestDllBox = new CTestDllBox;
    }

    return m_TestDllBox;
}

CTestDllBox* CTestDllBox::m_TestDllBox = NULL;

CTestDllBox::CTestDllBox(void)
{
}

CTestDllBox::~CTestDllBox(void)
{
}

void CTestDllBox::StartTest( vector<ThreadTag>* pVThreadTag, const string& strAppPath,
                            const string& strIn, const string& strResultDir )
{
    vector<ThreadTag>::iterator itr = pVThreadTag->begin();
    for(; itr != pVThreadTag->end(); ++itr)
    {
        CTestDllThread* pTestThread = new CTestDllThread(*itr, strAppPath, strIn, strResultDir);
        
        itr->bRunning = true;
        
        // 启动测试线程并加入线程管理器
    }
}


// TestDllThread.cpp
// author   :       Li Fa Ni
// Date     :       2013-02-26
// Ver      :       1.0

#include "TestDllThread.h"
#include "../ToolBox/toolBox.h"


CTestDllThread::CTestDllThread( ThreadTag& threadTag, const string& strAppPath,
                               const string& strIn, const string& strResultDir )
: m_threadTag(threadTag)
, m_strAppPath(strAppPath)
, m_strInputPath(strIn)
, m_strResultDir(strResultDir)
, m_hModule(NULL)
, m_pDll(NULL)
{
    
}

CTestDllThread::~CTestDllThread(void)
{
    Destory();
}

void CTestDllThread::Run( /* = 0 */ )
{
    if (!loadLibrary())
    {
        return;
    }

    string strIn;
    string strOut;

    vector<TestTag>::iterator itr = m_threadTag.vTestTag.begin();
    for (; itr != m_threadTag.vTestTag.end(); ++itr)
    {
        if (itr->nTestResult == 1)
        {
            continue;
        }

        int pos = itr->strTestPath.find_last_of('$');
        if (m_threadTag.id == 1)
        {
            strIn = m_strInputPath; 
        }
        else
        {
            string t = itr->strTestPath.substr(0, pos);
            strIn = m_strResultDir + SEPERATOR + t;
        }

        strOut = m_strResultDir + SEPERATOR + itr->strTestPath;
        string strDllName = itr->strTestPath.substr(pos + 1);
        string strAdditional = m_strAppPath + SEPERATOR + strDllName + ".xml";

        itr->nTestResult = m_pDll->StartTest(strIn, strOut, strAdditional);
        //if (!CheckFileExist(strOut))
        //{
        //    itr->nTestResult = -1;
        //}
    }

    m_threadTag.nRun = 0;
}

bool CTestDllThread::EndThreadNormal( DWORD ulTimes /* = 10 */ )
{
    return true;
}

bool CTestDllThread::loadLibrary()
{
    string strDllPath = m_strAppPath + SEPERATOR + m_threadTag.name;

    // 加载动态库，获取接口函数
    m_hModule= LoadLibraryA(strDllPath.c_str());
    if (m_hModule == NULL)
    {
        m_threadTag.nRun = -1;

        int e = GetLastError();

        return false;
    }

    FUNC func = (FUNC)GetProcAddress(m_hModule, "CreateInstance");
    if (func)
    {
        m_pDll = func();
    }

    if (NULL == m_pDll)
    {
        m_threadTag.nRun = -1;

        FreeLibrary(m_hModule);
        m_hModule = NULL;

        return false;
    }

    return true;
}

void CTestDllThread::Destory()
{
    if (NULL != m_pDll)
    {
        delete m_pDll;
        m_pDll = NULL;
    }

    if (NULL != m_hModule)
    {
        FreeLibrary(m_hModule);
        m_hModule = NULL;
    }
}

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

CTestDllBox::CTestDllBox(void) : 
m_pVThreadTag(NULL), 
m_strAppPath(""),
m_strIn(""), 
m_strResultDir(""),
m_bRunning(false),
m_hThread(NULL),
m_uThreadId(0),
m_pVThread(NULL)
{
}

CTestDllBox::~CTestDllBox(void)
{
    Destory();
}

bool CTestDllBox::StartTest( vector<ThreadTag>* pVThreadTag, const string& strAppPath,
                            const string& strIn, const string& strResultDir )
{
    if (m_bRunning)
    {
        return false;
    }

    m_bRunning = true;

    m_pVThreadTag   = pVThreadTag;
    m_strAppPath    = strAppPath;
    m_strIn         = strIn;
    m_strResultDir  = strResultDir;

    if (NULL == m_pVThread)
    {
        m_pVThread = new vector<ThreadElement*>;
    }

    m_pVThread->clear();

    // 启动守护线程
    if (MS_OK != StartNewThread(m_hThread, m_uThreadId))
    {
        m_bRunning = false;
        return false;
    }

    return true;
}

void CTestDllBox::Run()
{
    int oldId = 0;
    while (m_bRunning)
    {
        // 检测是否有已完成的线程任务
        vector<ThreadElement*>::iterator itrEle = m_pVThread->begin();
        for (; itrEle != m_pVThread->end();)
        {
            oldId = (*itrEle)->itr->id;

            if (!(*itrEle)->itr->bRunning)
            {
                delete (*itrEle)->pTestDllThread;
                itrEle = m_pVThread->erase(itrEle);
            }
            else
            {
                 ++itrEle;
            }
        }

        // 添加新线程任务
        if (m_pVThread->empty())
        {
            int newId = 0;
            int cnt = 0;
            vector<ThreadTag>::iterator itrTag = m_pVThreadTag->begin();
            for (; itrTag != m_pVThreadTag->end(); ++itrTag)
            {
                if (itrTag->id <= oldId)
                {
                    ++cnt;
                    if (cnt == m_pVThreadTag->size())
                    {
                        m_bRunning = false;
                    }

                    continue;
                }

                if (0 == newId || itrTag->id == newId)
                {
                    newId = itrTag->id;

                    ThreadElement* pThreadEle = new ThreadElement;

                    pThreadEle->pTestDllThread = new CTestDllThread(
                        *itrTag, m_strAppPath, m_strIn, m_strResultDir);
                    pThreadEle->itr = itrTag;
                    pThreadEle->itr->bRunning = true;

                    if (MS_ERR == pThreadEle->pTestDllThread->StartNewThread(
                        pThreadEle->handle, pThreadEle->uThreadId))
                    {
                        delete pThreadEle->pTestDllThread;
                        pThreadEle->pTestDllThread = NULL;

                        delete pThreadEle;
                        pThreadEle = NULL;
                    }
                    else
                    {
                        m_pVThread->push_back(pThreadEle);
                    }
                }
            }
        }
    }

    Destory();
}

bool CTestDllBox::EndThreadNormal( DWORD ulTimes )
{
    return true;
}

void CTestDllBox::EndTest()
{
    m_bRunning = false;
}

void CTestDllBox::Destory()
{
    if (NULL != m_pVThread)
    {
        vector<ThreadElement*>::iterator itr = m_pVThread->begin();
        for (; itr != m_pVThread->end(); ++itr)
        {
            (*itr)->pTestDllThread->EndThreadForce((*itr)->handle);

            delete (*itr)->pTestDllThread;
            (*itr)->pTestDllThread = NULL;

            delete *itr;
            *itr = NULL;
        }

        delete m_pVThread;
        m_pVThread = NULL;
    }
}


// BaseThread.cpp
// author   :       Li Fa Ni
// Date     :       2013-02-22
// Ver      :       1.0

#include "BaseThread.h"

CBaseThread::CBaseThread(void)
{
}

CBaseThread::~CBaseThread(void)
{
}

// Æô¶¯Ïß³Ì
unsigned CBaseThread::StartNewThread(HANDLE& hThread, unsigned int& uThreadId)
{
    hThread = (HANDLE)_beginthreadex(NULL, NULL, 
        CBaseThread::ThreadProc, this, CREATE_SUSPENDED, &uThreadId);
    if (NULL == hThread)
    {
        return MS_ERR;
    }

    ResumeThread(hThread);

    return MS_OK;
}

void CBaseThread::EndThreadForce(HANDLE hThread)
{
    TerminateThread(hThread, 0);
}

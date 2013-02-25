// BaseThread.h
// author   :       Li Fa Ni
// Date     :       2013-02-22
// Ver      :       1.0

#ifndef _BASETHREAD_H_
#define _BASETHREAD_H_

#include "../../include/typedef.h"

class DLL_API CBaseThread
{
public:
    CBaseThread(void);
    virtual ~CBaseThread(void);

    static unsigned _stdcall ThreadProc(void* ptr)
    {
        CBaseThread* pBaseThread = (CBaseThread*)ptr;
        if (NULL != pBaseThread)
        {
            pBaseThread->Run();
        }

        return MS_OK;
    }

    // �߳����к�����������ʵ��
    virtual void Run() = 0;

    // �����߳�
    unsigned StartNewThread(HANDLE& hThread, unsigned int& uThreadId);

    // ���������߳�
    virtual bool EndThreadNormal(DWORD ulTimes = 10) = 0;

    // ǿ�н����߳�
    void EndThreadForce(HANDLE hThread);
};

#endif

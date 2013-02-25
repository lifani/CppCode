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

    // 线程运行函数，由子类实现
    virtual void Run() = 0;

    // 启动线程
    unsigned StartNewThread(HANDLE& hThread, unsigned int& uThreadId);

    // 正常结束线程
    virtual bool EndThreadNormal(DWORD ulTimes = 10) = 0;

    // 强行结束线程
    void EndThreadForce(HANDLE hThread);
};

#endif

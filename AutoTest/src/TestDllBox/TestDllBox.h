#ifndef _TESTDLLBOX_H_
#define _TESTDLLBOX_H_

#include "../../include/typedef.h"

class DLL_API CTestDllBox
{
public:
    CTestDllBox(void);

    ~CTestDllBox(void);

    void StartTest(vector<ThreadTag>* pVThreadTag);

    void EndTest();
};

#endif

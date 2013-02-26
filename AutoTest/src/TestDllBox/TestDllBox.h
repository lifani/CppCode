// testDllBox.h
// author   :       Li Fa Ni
// Date     :       2013-02-26
// Ver      :       1.0

#ifndef _TESTDLLBOX_H_
#define _TESTDLLBOX_H_

#include "../../include/typedef.h"
#include "../TestDllThread/TestDllThread.h"

class DLL_API CTestDllBox
{
public:

    static CTestDllBox* Instance();

    CTestDllBox(void);

    ~CTestDllBox(void);

    void StartTest(vector<ThreadTag>* pVThreadTag, const string& strAppPath,
        const string& strIn, const string& strResultDir );

    void EndTest();

private:

    static CTestDllBox* m_TestDllBox;

};

#endif

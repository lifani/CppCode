#ifndef _TESTDLL_H_
#define _TESTDLL_H_

#include "..\common\dllinterface.h"

class CTestDll : public CDllInterface
{
public:
    CTestDll(void);

    virtual ~CTestDll(void);

    void StartTest(const string& strIn, const string& strOut);
};

#endif

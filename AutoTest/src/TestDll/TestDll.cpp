#include "TestDll.h"

CDllInterface* g_pDll = NULL;

CDllInterface* CreateInstance()
{
    if (NULL == g_pDll)
    {
        g_pDll = new CTestDll;
    }

    return g_pDll;
}

CTestDll::CTestDll(void)
{
}

CTestDll::~CTestDll(void)
{
}

void CTestDll::StartTest( const string& strIn, const string& strOut )
{
    cout << strIn.c_str() << endl;
    cout << strOut.c_str() << endl;

    system("pause");
}

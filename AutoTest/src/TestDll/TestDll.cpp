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

int CTestDll::StartTest( const string& strIn, const string& strOut, const string& strParaAdditional )
{
    cout << strIn.c_str() << endl;
    cout << strOut.c_str() << endl;

    FILE* f = fopen(strOut.c_str(), "a+");
    
    if (f != NULL)
    {
        fclose(f);
    }

    system("pause");

    return 10;
}

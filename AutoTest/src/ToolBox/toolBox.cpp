#include "toolBox.h"
#include <io.h>

int Replace(string& strInOut, const char oldVal, const char newVal)
{
    if (oldVal == '\0' || newVal == '\0' || strInOut.empty())
    {
        return 0;
    }

    int cnt = 0;
    for (unsigned int i = 0; i < strInOut.length(); ++i)
    {
        if (oldVal == strInOut.at(i))
        {
            strInOut.at(i) = newVal;
            ++cnt;
        }
    }

    return cnt;
}

DLL_API bool CheckFileExist( const string& strIn )
{
    return _access(strIn.c_str(), 0) != -1;
}

DLL_API int CreateDirectorys( const string& strIn )
{
    char szPath[MAX_PATH] = {0};

    int nRet = 0;

    unsigned int i = 0;
    while (i < strIn.length() && i < MAX_PATH)
    {
        if (strIn.at(i) == '\\' || strIn.at(i) == '/')
        {
            if ((nRet = CreateDirctoryOnly(szPath)) != MS_OK)
            {
                return nRet;
            }
        }

        szPath[i] = strIn.at(i);

        ++i;
    }

    if ((nRet = CreateDirctoryOnly(szPath)) != MS_OK)
    {
        return nRet;
    }

    return MS_OK;
}

DLL_API int CreateDirctoryOnly( const string& strIn )
{
    if (strIn.empty())
    {
        return MS_ERR;
    }

    if (_access(strIn.c_str(), 0) == -1)
    {
        if (!CreateDirectoryA(strIn.c_str(), NULL))
        {
            return GetLastError();
        }
    }

    return MS_OK;
}

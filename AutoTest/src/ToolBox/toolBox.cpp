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

DLL_API int GetFileTimes( const string& strIn, 
                         string& strCreationTime, 
                         string& strLastAccessTime, 
                         string& strLastWriteTime )
{
    FILETIME creationTime;
    FILETIME lastAccessTime;
    FILETIME lastWriteTime;

    HANDLE hdl = CreateFileA(strIn.c_str(), 
        GENERIC_READ, 
        0,
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (INVALID_HANDLE_VALUE == hdl
        || 0 == GetFileTime(hdl, &creationTime, &lastAccessTime, &lastWriteTime))
    {
        if (INVALID_HANDLE_VALUE != hdl)
        {
            CloseHandle(hdl);
        }

        return GetLastError();
    }

    if (MS_OK != FileTimeToSysTime(&creationTime, strCreationTime)
        || MS_OK != FileTimeToSysTime(&lastAccessTime, strLastAccessTime)
        || MS_OK != FileTimeToSysTime(&lastWriteTime, strLastWriteTime))
    {
        return MS_ERR;
    }
    
    return MS_OK;
}

DLL_API int FileTimeToSysTime( FILETIME* lpFileTime, string& strTimeOut )
{
    if (NULL == lpFileTime)
    {
        return MS_ERR;
    }

    FILETIME localFileTime;
    if (0 == FileTimeToLocalFileTime(lpFileTime, &localFileTime))
    {
        return GetLastError();
    }

    SYSTEMTIME sysTime;
    char szBuf[25] = {0};

    if (0 == FileTimeToSystemTime(&localFileTime, &sysTime))
    {
        return GetLastError();
    }

    sprintf_s(szBuf, 25, "%04d%02d%02d%02d%02d%02d%03d\0", 
        sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
        sysTime.wHour, sysTime.wMinute, sysTime.wSecond, 
        sysTime.wMilliseconds);

    strTimeOut = string(szBuf);

    return MS_OK;
}

DLL_API int GetCreationTime( const string& strIn, string& strOut )
{
    string t1;
    string t2;

    return GetFileTimes(strIn, strOut, t1, t2);
}

DLL_API int GetLastAccessTime( const string& strIn, string& strOut )
{
    string t1;
    string t2;

    return GetFileTimes(strIn, t1, strOut, t2);
}

DLL_API int GetLastWriteTime( const string& strIn, string& strOut )
{
    string t1;
    string t2;

    return GetFileTimes(strIn, t1, t2, strOut);
}



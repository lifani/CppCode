#ifndef _TOOLBOX_H_
#define _TOOLBOX_H_

#include "../../include/typedef.h"

DLL_API int Replace(string& strInOut, const char oldVal, const char newVal);

DLL_API bool CheckFileExist(const string& strIn);

DLL_API int CreateDirectorys(const string& strIn);

DLL_API int CreateDirctoryOnly(const string& strIn);

DLL_API int GetFileTimes(const string& strIn, string& strCreationTime, 
                         string& strLastAccessTime, string& strLastWriteTime);

DLL_API int GetCreationTime(const string& strIn, string& strOut);

DLL_API int GetLastAccessTime(const string& strIn, string& strOut);

DLL_API int GetLastWriteTime(const string& strIn, string& strOut);

DLL_API int FileTimeToSysTime(FILETIME* lpFileTime, string& strTimeOut);

#endif
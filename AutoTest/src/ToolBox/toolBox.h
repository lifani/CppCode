#ifndef _TOOLBOX_H_
#define _TOOLBOX_H_

#include "../../include/typedef.h"

DLL_API int Replace(string& strInOut, const char oldVal, const char newVal);

DLL_API bool CheckFileExist(const string& strIn);

DLL_API int CreateDirectorys(const string& strIn);

DLL_API int CreateDirctoryOnly(const string& strIn);


#endif
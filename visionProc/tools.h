#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "typedef.h"

 void Initlog(const char* strProcName);

 void Writelog(int priority, const char* strErrInfo, const char* strFileName = __FILE__, int line = __LINE__);

#endif
#include "tools.h"

 void Initlog(const char* strProcName)
{
    openlog(strProcName, LOG_ODELAY | LOG_PID, LOG_USER);
}

 void Writelog(int priority, const char* strErrInfo, const char* strFileName, int line )
{
    syslog(priority, "log : %s at file: %s line: %d", strErrInfo, strFileName, line);
}
#include "tools.h"

void Initlog(const char* strProcName)
{
	openlog(strProcName, LOG_ODELAY | LOG_PID, LOG_USER);
}

void Writelog(int priority, const char* strErrInfo, const char* strFileName, int line )
{
    // syslog(priority, "log : %s at file: %s line: %d", strErrInfo, strFileName, line);
	// printf("%s, %s : %d", strErrInfo, strFileName, line);
	
	cout << "Info: " << strErrInfo << " File: " << strFileName << " Line: " << line << endl;
}

void WriteImg(const string& strFileName, const char* pBuf, int size)
{
	if (NULL == pBuf)
	{
		return;
	}
	
	FILE* pf = fopen(strFileName.c_str(), "w+");
	if (NULL == pf) 
	{
		return;
	}
	
	fwrite(pBuf, size, 1, pf);
	
	fclose(pf);
}
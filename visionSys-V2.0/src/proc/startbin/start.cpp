/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include <typedef.h>
#include <datatype.h>

#define LOG_TAG "START.BIN"

static string getdir(const char* in)
{
	string out = "";
	if (NULL == in)
	{
		return out;
	}
	
	out = in;
	unsigned int pos = 0;
	for (unsigned int i = 0; i < out.size(); ++i)
	{
		if (out.at(i) == '/')
		{
			pos = i;
		}
	}
	
	out.resize(pos);
	
	return out;
}

static string getUname()
{
	string outUname = "";
	string strCmd = "uname -r";
	
	FILE* pf = popen(strCmd.c_str(), "r");
	if (NULL == pf)
	{
		return outUname;
	}
	
	char szBuf[1024] = {0};
	fgets(szBuf, sizeof(szBuf), pf);
	
	outUname = string(szBuf);
	
	return outUname;
}

int main(int argc, char* argv[])
{
	string strCwd = getdir(argv[0]);
	
	// 改变工作目录
	if (-1 == chdir(strCwd.c_str()))
	{
		LOGE("change dir error. %s : %d\n", __FILE__, __LINE__);
		return 0;
	}
	
	string uname = getUname();
	string strlog = uname + " | " + SOFT_VERSION;
	
	LOGW("%s. %s : %d\n", strlog.c_str(), __FILE__, __LINE__);
	
	// 替换进程内容
	if (-1 == execl("./visionDeamon", "./visionDeamon", "visionDeamon", (char*)0))
	{
		LOGE("execl error. %s : %d\n", __FILE__, __LINE__);
	}
	
	return 0;
}
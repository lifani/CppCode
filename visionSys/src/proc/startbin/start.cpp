#include <typedef.h>
#include <datatype.h>

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
	
	// �ı乤��Ŀ¼
	if (-1 == chdir(strCwd.c_str()))
	{
		cout << "change dir error." << endl;
		return 0;
	}
	
	string uname = getUname();
	string strlog = uname + " | " + SOFT_VERSION;
	
	LOGW("%s. %s : %d\n", strlog.c_str(), __FILE__, __LINE__);
	
	// �滻��������
	if (-1 == execl("./visionRcm", "./visionRcm", (char*)0))
	{
		cout << "execl error." << endl;
	}
	
	return 0;
}
#include <typedef.h>

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

int main(int argc, char* argv[])
{
	string strCwd = getdir(argv[0]);
	
	// 改变工作目录
	if (-1 == chdir(strCwd.c_str()))
	{
		cout << "change dir error." << endl;
		return 0;
	}
	
	// 替换进程内容
	if (-1 == execl("./visionRcm", "./visionRcm", (char*)0))
	{
		cout << "execl error." << endl;
	}
	
	return 0;
}
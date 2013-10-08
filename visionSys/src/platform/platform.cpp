#include <platform/platform.h>
#include <signal.h>
#include "monitor.h"

static CCommonInterface* pCommonI = NULL;

static string getProcName(const char* in)
{
	string out = "";
	
	int pos = 0;
	int i = 0;
	
	while('\0' != *(in + i))
	{
		if ('/' == *(in + i))
		{
			pos = i + 1;
		}
		
		++i;
	}
	
	if (*(in + pos) != '\0')
	{
		out = in + pos;
	}
	
	return out;
}

static void sig_handle(int signo)
{
	if (SIGINT == signo)
	{
		LOGE("recv sigint. %s : %d\n", __FILE__, __LINE__);
		if (NULL != pCommonI)
		{
			pCommonI->Deactive();
		}
	}
}

int main(int argc, char* argv[])
{
	string ppname = "";
	string pname = getProcName(argv[0]);
	
	if (argc == 2)
	{
		ppname = argv[1];
	}
	
	if (argc > 2)
	{
		LOGE("a.out [<pname>]. %s : %d\n", __FILE__, __LINE__);
		return 0;
	}
	
	signal(SIGINT, sig_handle);
	
	vector<xml_node> vXmlNode;
	
	// 加载配置文件
	CXml xml;
	if (!xml.ReadXml(CONFIG_XML, vXmlNode))
	{
		LOGE("load xml config error. %s : %d\n", __FILE__, __LINE__);
		return 0;
	}
	
	do {
	
		InitMonitor();
	
		pCommonI = CreateInstance(ppname.c_str(), pname.c_str());
		if (NULL == pCommonI)
		{
			LOGE("create instance error. %s : %d\n", __FILE__, __LINE__);
			break;
		}
		
		bool bFlg = false;
		
		vector<xml_node>::iterator itr = vXmlNode.begin();
		for (; itr != vXmlNode.end(); ++itr)
		{
			bFlg = false;
			if (itr->name.compare(pname) == 0)
			{
				vector<xml_node>::iterator it = itr->vXmlNode.begin();
				for(; it != itr->vXmlNode.end(); ++it)
				{
					pid_t pid;
					if ((pid = fork()) == -1)
					{
						LOGE("fork proc error. %s : %d\n", __FILE__, __LINE__);
						
						bFlg = true;
						break;
					}
					else if (pid > 0)
					{
						pCommonI->AddSonProcInfo(it->name.c_str(), pid);
					}
					else if (pid == 0)
					{
						string name = it->name;
						
						// 替换进程内容
						if (-1 == execl(name.c_str(), name.c_str(), pname.c_str(), (char*)0))
						{
							LOGE("%s exec error. %s : %d\n", name.c_str(), __FILE__, __LINE__);
							
							bFlg = true;
							break;
						}
					}
				}
				
				if (bFlg)
				{
					break;
				}
			}
		}
		
		if (bFlg)
		{
			break;
		}
		
		// 激活进程
		if (-1 == pCommonI->Active())
		{
			LOGE("%s active error. %s : %d\n", pname.c_str(), __FILE__, __LINE__);
			break;
		}
		
		// 启动线程
		// 执行
		if (-1 == pCommonI->Action())
		{
			LOGE("%s action error. %s : %d\n", pname.c_str(), __FILE__, __LINE__);
			break;
		}

		// 主线程进入休眠状态
		Monitor();
		
	} while (0);
	
	delete pCommonI;
	
	return 0;
}
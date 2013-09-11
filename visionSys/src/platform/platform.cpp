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
		cout << "recv sigint." << endl;
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
		cout << "a.out [<pname>]" << endl;
		return 0;
	}
	
	signal(SIGINT, sig_handle);
	
	vector<xml_node> vXmlNode;
	
	// ���������ļ�
	CXml xml;
	if (!xml.ReadXml(CONFIG_XML, vXmlNode))
	{
		cout << "load config error." << endl;
		return 0;
	}
	
	do {
	
		InitMonitor();
	
		pCommonI = CreateInstance(ppname.c_str(), pname.c_str());
		if (NULL == pCommonI)
		{
			cout << "create instance error." << endl;
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
						cout << "fork proc error." << endl;
						
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
						
						// �滻��������
						if (-1 == execl(name.c_str(), name.c_str(), pname.c_str(), (char*)0))
						{
							cout << "exec error." << endl;
							
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
		
		// �������
		if (-1 == pCommonI->Active())
		{
			cout << pname << " active error." << endl;
			break;
		}
		
		// �����߳�
		// ִ��
		if (-1 == pCommonI->Action())
		{
			cout << pname << " action error." << endl;
		}

		// ȥ����
		Monitor();
		
	} while (0);
	
	delete pCommonI;
	
	return 0;
}
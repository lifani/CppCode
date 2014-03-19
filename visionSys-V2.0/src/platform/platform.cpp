/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include <platform/platform.h>
#include <signal.h>
#include "monitor.h"

#include <execinfo.h>

#define LOG_TAG "VISION_PLATFORM"

static CCompoentInterface* pCompoentI = NULL;

static void fun_dump( int no)
{
	char _signal[64][32] = {
	"1: SIGHUP", "2: SIGINT", "3: SIGQUIT", "4: SIGILL",
	"5: SIGTRAP", "6: SIGABRT", "7: SIGBUS", "8: SIGFPE",
	"9: SIGKILL", "10: SIGUSR1", "11: SIGSEGV", "12: SIGUSR2",
	"13: SIGPIPE", "14: SIGALRM", "15: SIGTERM", "16: SIGSTKFLT",
	"17: SIGCHLD", "18: SIGCONT", "19: SIGSTOP", "20: SIGTSTP",
	"21: SIGTTIN", "22: SIGTTOU", "23: SIGURG", "24: SIGXCPU",
	"25: SIGXFSZ", "26: SIGVTALRM", "27: SIGPROF", "28: SIGWINCH",
	"29: SIGIO", "30: SIGPWR", "31: SIGSYS", "34: SIGRTMIN",
	"35: SIGRTMIN+1", "36: SIGRTMIN+2", "37: SIGRTMIN+3", "38: SIGRTMIN+4",
	"39: SIGRTMIN+5", "40: SIGRTMIN+6", "41: SIGRTMIN+7", "42: SIGRTMIN+8",
	"43: SIGRTMIN+9", "44: SIGRTMIN+10", "45: SIGRTMIN+11", "46: SIGRTMIN+12",
	"47: SIGRTMIN+13", "48: SIGRTMIN+14", "49: SIGRTMIN+15", "50: SIGRTMAX-14",
	"51: SIGRTMAX-13", "52: SIGRTMAX-12", "53: SIGRTMAX-11", "54: SIGRTMAX-10",
	"55: SIGRTMAX-9", "56: SIGRTMAX-8", "57: SIGRTMAX-7", "58: SIGRTMAX-6",
	"59: SIGRTMAX-5", "60: SIGRTMAX-4", "61: SIGRTMAX-3", "62: SIGRTMAX-2",
	"63: SIGRTMAX-1", "64: SIGRTMAX" };

	void *stack_p[10];
	char **stack_info;
	int size;

	size = backtrace( stack_p, sizeof(stack_p));
	stack_info = backtrace_symbols( stack_p, size);

	if( no >= 1 && no <= 64)   
			printf("[%s] %d stack frames.\n", _signal[no-1], size);
	else
			printf("[No infomation %d] %d stack frames.\n", no, size);

	int i = 0;
	for( ; i < size; i++)
			printf("%s\n", stack_info[i]);

	free( stack_info);

	//free anything
	fflush(NULL);
	exit(0);
}

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
		if (NULL != pCompoentI)
		{
			pCompoentI->Deactive();
		}
	}
	
	if (SIGSEGV == signo)
	{
		LOGE("recv sigsegv. %s : %d\n", __FILE__, __LINE__);
		exit(0);
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
	signal(SIGSEGV, fun_dump);
	signal(SIGABRT, fun_dump);
	signal(SIGBUS, fun_dump);
	
	vector<PROC_CONFIG> vProcXmlNode;
	vector<MSG_CONFIG> vMsgXmlNode;
	vector<OPTION> vOption;
	
	// 加载配置文件
	CXml xml;
	if (!xml.ReadXml(CONFIG_XML, vProcXmlNode, vMsgXmlNode))
	{
		LOGE("load xml config err. %s : %d\n", __FILE__, __LINE__);
		return 0;
	}
	
	if (!xml.ReadOption(CONFIG_XML, vOption))
	{
		LOGE("Read option err. %s : %d\n", __FILE__, __LINE__);
		return 0;
	}
	
	do {
	
		InitMonitor();
	
		pCompoentI = CreateInstance(ppname.c_str(), pname.c_str());
		if (NULL == pCompoentI)
		{
			LOGE("create instance error. %s : %d\n", __FILE__, __LINE__);
			break;
		}
		
		// 添加消息配置信息
		pCompoentI->AddConfig(vMsgXmlNode, vProcXmlNode);
		
		// 添加选项信息
		pCompoentI->AddOption(vOption);
		
		bool bFlg = false;
		
		vector<PROC_CONFIG>::iterator itr = vProcXmlNode.begin();
		for (; itr != vProcXmlNode.end(); ++itr)
		{
			bFlg = false;
			if (itr->name.compare(pname) == 0)
			{
				vector<PROC_CONFIG>::iterator it = itr->vProcConfig.begin();
				for(; it != itr->vProcConfig.end(); ++it)
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
						pCompoentI->AddProcInfo(it->name.c_str(), pid);
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
		if (-1 == pCompoentI->Active())
		{
			LOGE("%s active error. %s : %d\n", pname.c_str(), __FILE__, __LINE__);
			break;
		}
		
		// 启动线程
		// 执行
		if (-1 == pCompoentI->Action())
		{
			LOGE("%s action error. %s : %d\n", pname.c_str(), __FILE__, __LINE__);
			break;
		}

		// 主线程进入休眠状态
		Monitor();
		
	} while (0);
	
	delete pCompoentI;
	
	return 0;
}
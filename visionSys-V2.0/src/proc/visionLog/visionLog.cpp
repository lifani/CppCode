/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include <typedef.h>
#include <datatype.h>
#include <xml/xml.h>

static vector<PROC_CONFIG> _vProcConfig;
static int _msgqid = 0;

static int GetMsgqid(PROC_CONFIG& tProcConfig)
{	
	string pname = tProcConfig.name;
	int pid = tProcConfig.pid;
	
	char szPath[256] = {0};
	getcwd(szPath, sizeof(szPath));
	
	string strCwd = string(szPath);
	
	string strAbsPath = strCwd + string("/") + pname;
	
	key_t key = ftok(strAbsPath.c_str(), pid);
	if ((_msgqid = msgget(key, IPC_CREAT | 0600)) == -1)
	{
		return -1;
	}
	
	return 0;
}

static void SendMsgImp(const char* pname, vector<PROC_CONFIG>& vProcConfig)
{
	VISION_MSG msg;
	msg.data.ptr = NULL;
	msg.data.size = 0;
	
	vector<PROC_CONFIG>::iterator itv = vProcConfig.begin();
	for (; itv != vProcConfig.end(); ++itv)
	{
		if (NULL == pname)
		{
			msg.id = itv->pid;
			
			msgsnd(_msgqid, (char*)&msg, sizeof(MSG_DATA), IPC_NOWAIT);
		}
		else if ((itv->name).compare(string(pname)) == 0)
		{
			msg.id = itv->pid;
			msgsnd(_msgqid, (char*)&msg, sizeof(MSG_DATA), IPC_NOWAIT);
			
			return;
		}
		else
		{
			SendMsgImp(pname, itv->vProcConfig);
		}
	}
}

static void SendMsg(const char* pname)
{
	SendMsgImp(pname, _vProcConfig);
}

int main(int argc, char* argv[])
{
	vector<MSG_CONFIG> vMsgConfig;
	
	CXml xml;
	if (!xml.ReadXml(CONFIG_XML, _vProcConfig, vMsgConfig))
	{
		cout << "load config fail, please check the file : /cache/vision/proc/config.xml" << endl;
		return 0;
	}
	
	if (_vProcConfig.size() == 0)
	{
		cout << "config error, please check the file : /cache/vision/proc/config.xml" << endl;
		return 0;
	}
	
	vector<PROC_CONFIG>::reference tProcConfig = _vProcConfig.front();
	if (-1 == GetMsgqid(tProcConfig))
	{
		cout << "get msqid fail." << endl;
	}
	
	if (argc > 2)
	{
		for (int i = 1; i < argc; ++i)
		{
			SendMsg(argv[1]);
		}
	}
	else
	{
		SendMsg(NULL);
	}
	
	return 0;
}

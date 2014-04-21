#ifndef __BASE_VISION_H__
#define __BASE_VISION_H__

#include <compoentInterface.h>

// Message drive

#define DECLARE_MESSAGE_MAP \
protected : \
	static const VISION_MSGMAP* GetThisMessageMap(); \
	virtual const VISION_MSGMAP* GetMessageMap(); \
	
#define BEGAIN_MESSAGE_MAP(theClass, baseClass) \
const VISION_MSGMAP* theClass::GetMessageMap() \
{ \
	return GetThisMessageMap(); \
} \
const VISION_MSGMAP* theClass::GetThisMessageMap() \
{ \
	typedef theClass ThisClass; \
	typedef baseClass TheBaseClass; \
	static const VISION_MSGMAP_ENTRY _messageEntries[] = \
	{ \
	
#define ON_COMMAND(id, memberFunc) \
		{id, static_cast<VISION_PMSG>(memberFunc)}, \
	
#define END_MESSAGE_MAP() \
		{0, (VISION_PMSG)0} \
	}; \
	static const VISION_MSGMAP messageMap =  \
	{ &TheBaseClass::GetThisMessageMap, &_messageEntries[0] }; \
	return &messageMap; \
} \

// Timer

#define DECLARE_TIMER_MAP \
protected : \
	static const VISION_TIMERMAP* GetThisTimerMap(); \
	virtual const VISION_TIMERMAP* GetTimerMap(); \
	
#define BEGAIN_TIMER_MAP(theClass, baseClass) \
const VISION_TIMERMAP* theClass::GetTimerMap() \
{ \
	return GetThisTimerMap(); \
} \
const VISION_TIMERMAP* theClass::GetThisTimerMap() \
{ \
	typedef theClass ThisClass; \
	typedef baseClass TheBaseClass; \
	static const VISION_TIMERMAP_ENTRY _timerEntries[] = \
	{ \

#define ON_TIMER(timeusec, bWait, memberFunc) \
		{timeusec, bWait, static_cast<VISION_PTIMER>(memberFunc)}, \
	
#define END_TIMER_MAP() \
		{0, false, (VISION_PTIMER)0} \
	}; \
	static const VISION_TIMERMAP timerMap = \
	{ &TheBaseClass::GetThisTimerMap, &_timerEntries[0] }; \
	return &timerMap; \
} \

// Creator

#define DEFINE_CREATE_INSTANCE(theClass) \
CCompoentInterface* CreateInstance(const char* ppname, const char* pname) \
{ \
	CCompoentInterface* p = NULL; \
	if (NULL != ppname && NULL != pname) \
	{ \
		p = new theClass(ppname, pname); \
	} \
	return p; \
} \

// 注册线程函数
#define REGISTER_THREAD(x) \
	RegisterThread(static_cast<THREAD_FUNC>(x))

// 注册消息生成函数
#define REGISTER_MSG_FUNC(x, y) \
	RegisterMsgFunc((x), (static_cast<MSG_FUNC>(y)))

class CBaseVision : public CCompoentInterface
{
	DECLARE_MESSAGE_MAP
	
	DECLARE_TIMER_MAP
	
public :

	CBaseVision(const char* ppname, const char* pname);
	
	virtual ~CBaseVision();
	
	virtual int Active();
	
	virtual int ActiveImp() = 0;
	
	virtual int Action();
	
	virtual int Deactive();
	
	virtual int DeactiveImp() = 0;
	
	virtual void AddMsgTag(const map<long, MSG_TAG*>& mapMsgTag, const map<string, PROC_TAG>& mapProcTag);
	
	virtual void AddProcInfo(const char* pname, int pid);
	
	virtual void AddOption(vector<OPTION>& vOption);

	virtual void HandleMessage();
	
	virtual void PostMessage(const VISION_MSGMAP_ENTRY* pEntry);
	
	virtual int RecvMsg(VISION_MSG* pMsg);
	
	virtual int SendMsg(VISION_MSG* pMsg);
	
	virtual int SendSmallMsg(long id, char* ptr, unsigned int size);
	
	virtual void RegisterThread(THREAD_FUNC pfn);
	
	virtual void RegisterMsgFunc(string name, MSG_FUNC func);
	
	virtual bool IsRunning();
	
	virtual void StopRun();
	
	virtual bool IsTimerRunning();
	
	virtual void StopTimer();
	
	virtual void AddLog(const string& strLog);
	
	virtual void OutLog(VISION_MSG* pMsg);
	
	virtual void ReqTimer(VISION_TIMER* pTimer);
	
	virtual void ResTimer(VISION_TIMER* pTimer);
	
	virtual void SetTimer();
	
	virtual void KillTimer();
	
	virtual void SendHeartMsg();
	
	static void* StartThread(void* arg);
	
	static void* StartReqTimer(void* arg);
	
	static void* StartResTimer(void* arg);
	
protected :

	void USleep(unsigned int usec);
	
	void SetStatusCode(int code);
	
	void NoticeTimer();
	
	void SendCanData(int identify, int id, char* pData, size_t size);
	
private :

	void GetProcInfo();
	
	void InitMsgFunc();
	
	unsigned GetDelayTime(struct timeval* last_val, struct timeval* cur_val, unsigned dTime);
	
	void SetErr(int err);
	
	void RecoverErr(int err);

public :
	
	map<long, MSG_TAG*> m_mapPMsgTag;
	vector<OPTION> m_vOption;
	
	PROC_TAG m_procTag;

protected :

	string m_pname;
	pid_t m_pid;
	int	m_code;
	
	int m_wTime;
	
	map<string, MSG_FUNC> m_mapMsgFunc;
	map<int, PROC_INFO*> m_mapProcInfo;

private :

	vector<THREAD_ENTRY> m_vThreadEntry;
	vector<string> m_vLog;
	vector<VISION_TIMER*> m_vTimer;
	set<int> m_setErr;
	
	string m_ppname;
	pid_t m_ppid;
	
	string m_strCwd;
	
	bool m_bRunning;
	bool m_bTimerRunning;
	
	pthread_mutex_t m_lock;
	pthread_cond_t m_ready;
	
	pthread_mutex_t m_errLock;
};

#endif

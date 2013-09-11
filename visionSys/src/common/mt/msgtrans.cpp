#include <typedef.h>
#include "msgtrans.h"

CMsgTrans::CMsgTrans(key_t key) : m_key(key), m_msgid(0), m_initialized(-1)
{
}

CMsgTrans::~CMsgTrans()
{
	if (m_initialized == 0)
	{
		msgctl(m_msgid, IPC_RMID, 0);
	}
}

int CMsgTrans::Init()
{	
	if ((m_msgid = msgget(m_key, IPC_CREAT | 0600)) == -1)
	{
		return -1;
	}
	
	m_initialized = 0;
	
	return 0;
}

int CMsgTrans::write(char* ptr, unsigned int* size)
{
	return msgsnd(m_msgid, ptr, *size, IPC_NOWAIT);
}

int CMsgTrans::read(char* ptr, unsigned int* size)
{
	long type = *(long*)ptr;
	
	return msgrcv(m_msgid, ptr, *size, type, MSG_NOERROR | IPC_NOWAIT);
}

void CMsgTrans::destory(CTransData* p)
{
	delete p;
}
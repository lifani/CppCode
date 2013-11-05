#include "msgTrans.h"

CMsgTrans* CMsgTrans::m_pMsgTrans = NULL;

CMsgTrans* CMsgTrans::CreateInstance(key_t key)
{
	if (NULL == m_pMsgTrans)
	{
		m_pMsgTrans = new CMsgTrans();
		if (NULL != m_pMsgTrans && m_pMsgTrans->Init(key) == -1)
		{
			delete m_pMsgTrans;
			m_pMsgTrans = NULL;
		}
	}
	
	return m_pMsgTrans;
}

CMsgTrans::CMsgTrans() : m_msgqid(0)
{
}

CMsgTrans::~CMsgTrans()
{
	msgctl(m_msgqid, IPC_RMID, 0);
}

int CMsgTrans::Init(key_t key)
{
	if ((m_msgqid = msgget(key, IPC_CREAT | 0600)) == -1)
	{
		return -1;
	}
	
	return 0;
}

int CMsgTrans::ReadMsg(VISION_MSG* pMsg)
{
	unsigned int size = sizeof(MSG_DATA);
	
	return msgrcv(m_msgqid, (char*)pMsg, size, pMsg->id, MSG_NOERROR | IPC_NOWAIT);
}

int CMsgTrans::WriteMsg(VISION_MSG* pMsg)
{
	return msgsnd(m_msgqid, (char*)pMsg, sizeof(MSG_DATA), IPC_NOWAIT);
}


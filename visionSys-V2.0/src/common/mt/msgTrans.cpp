/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "msgTrans.h"

CMsgTrans* CMsgTrans::m_pMsgTrans = NULL;

/************************************
功能：	创建CMsgTrans实例
参数：	key key_t IPC key值
返回：	成功 0，失败 -1
************************************/
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

/************************************
功能：	消息队列初始化
参数：	key key_t IPC key值
返回：	成功 0，失败 -1
************************************/
int CMsgTrans::Init(key_t key)
{
	if ((m_msgqid = msgget(key, IPC_CREAT | 0600)) == -1)
	{
		return -1;
	}
	
	return 0;
}

/************************************
功能：	接收消息
参数：	pMsg VISION_MSG* 消息指针, 不能为空
返回：	成功 0，失败 -1
************************************/
int CMsgTrans::ReadMsg(VISION_MSG* pMsg)
{
	unsigned int size = sizeof(MSG_DATA);
	
	return msgrcv(m_msgqid, (char*)pMsg, size, pMsg->id, MSG_NOERROR | IPC_NOWAIT);
}

/************************************
功能：	发送消息
参数：	pMsg VISION_MSG* 消息指针, 不能为空
返回：	成功 0，失败 -1
************************************/
int CMsgTrans::WriteMsg(VISION_MSG* pMsg)
{
	int err = msgsnd(m_msgqid, (char*)pMsg, sizeof(MSG_DATA), IPC_NOWAIT);
	if (-1 == err)
	{
		LOGE("send msg err(%d). %s : %d\n", errno, __FILE__, __LINE__);
	}
		
	return err;
}


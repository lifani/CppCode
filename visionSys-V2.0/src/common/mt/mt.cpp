/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include <mt/mt.h>

#include "shmTrans.h"
#include "msgTrans.h"

static CShmTrans* g_pShmTrans = NULL;
static CMsgTrans* g_pMsgTrans = NULL;

/************************************
���ܣ�	ע��
������	const map<long, MSG_TAG*>& mapPMsgTag& ��Ϣ������Ϣ
		pname string ������
		pid int ����ID
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CMT::Register(const map<long, MSG_TAG*>& mapPMsgTag, string& pname, int pid)
{
	key_t key = ftok(pname.c_str(), pid);
	
	g_pShmTrans = CShmTrans::CreateInstance(mapPMsgTag, key);
	g_pMsgTrans = CMsgTrans::CreateInstance(key);
	
	if (NULL == g_pShmTrans || NULL == g_pMsgTrans)
	{
		return -1;
	}
	
	return 0;
}

/************************************
���ܣ�	ע��
������	��
���أ�	�ɹ� 0
************************************/
int CMT::Destory()
{
	if (NULL != g_pShmTrans)
	{
		delete g_pShmTrans;
		g_pShmTrans = NULL;
	}
	
	if (NULL != g_pMsgTrans)
	{
		delete g_pMsgTrans;
		g_pMsgTrans = NULL;
	}
	
	return 0;
}

/************************************
���ܣ�	������Ϣ
������	pMsg VISION_MSG* ��Ϣָ��, ����Ϊ��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CMT::RecvMsg(VISION_MSG* pMsg)
{
	if (NULL == pMsg || NULL == g_pMsgTrans || NULL == g_pShmTrans)
	{
		return -1;
	}

	if (g_pMsgTrans->ReadMsg(pMsg) == -1)
	{
		return -1;
	}
	
	if (NULL != pMsg->data.ptr)
	{
		if (g_pShmTrans->ReadMsg(pMsg) == -1)
		{
			return -1;
		}
	}
	
	return 0;
}

/************************************
���ܣ�	������Ϣ
������	pMsg VISION_MSG* ��Ϣָ��, ����Ϊ��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CMT::SendMsg(VISION_MSG* pMsg)
{
	if (NULL == pMsg || NULL == g_pMsgTrans || NULL == g_pShmTrans)
	{
		return -1;
	}
	
	if (NULL != pMsg->data.ptr && g_pShmTrans->WriteMsg(pMsg) == -1)
	{
		return -1;
	}
	
	return g_pMsgTrans->WriteMsg(pMsg);
}

#include <mt/mt.h>

#include "shmTrans.h"
#include "msgTrans.h"

static CShmTrans* g_pShmTrans = NULL;
static CMsgTrans* g_pMsgTrans = NULL;

int CMT::Register(vector<MSG_CONFIG>& vMsgConfig, string& pname, int pid)
{
	key_t key = ftok(pname.c_str(), pid);
	
	g_pShmTrans = CShmTrans::CreateInstance(vMsgConfig, key);
	g_pMsgTrans = CMsgTrans::CreateInstance(key);
	
	if (NULL == g_pShmTrans || NULL == g_pMsgTrans)
	{
		return -1;
	}
	
	return 0;
}

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

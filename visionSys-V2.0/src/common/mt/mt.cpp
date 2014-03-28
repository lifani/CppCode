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
功能：	注册
参数：	const map<long, MSG_TAG*>& mapPMsgTag& 消息配置信息
		pname string 进程名
		pid int 进程ID
返回：	成功 0，失败 -1
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
功能：	注销
参数：	无
返回：	成功 0
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
功能：	接收消息
参数：	pMsg VISION_MSG* 消息指针, 不能为空
返回：	成功 0，失败 -1
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
功能：	发送消息
参数：	pMsg VISION_MSG* 消息指针, 不能为空
返回：	成功 0，失败 -1
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

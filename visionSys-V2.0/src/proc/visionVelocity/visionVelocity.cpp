/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "visionVelocity.h"
#include <VO/demon.h>

#define LOG_TAG "VISION_VELOCITY"

BEGAIN_MESSAGE_MAP(CVisionVelocity, CBaseVision)
	ON_COMMAND(VELOCITY_ID, &CVisionVelocity::ProcessMsg)
END_MESSAGE_MAP()

DEFINE_CREATE_INSTANCE(CVisionVelocity)

CVisionVelocity::CVisionVelocity(const char* ppname, const char* pname)
: CBaseVision(ppname, pname)
, m_Num(2)
, m_index(0)
, m_pImu(0)
, m_pFeedBack(0)
{
}

CVisionVelocity::~CVisionVelocity()
{
}

/************************************
功能：	VisionVelocity激活函数，完成初始化工作
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionVelocity::ActiveImp()
{
	if (NULL == m_pFeedBack)
	{
		m_pFeedBack = new char[256];
		if (NULL == m_pFeedBack)
		{
			return -1;
		}
	}
	
	// 初始化选项
	InitOption();
	
	// 注册算法接口
	Register(m_Num);
	
	LOGW("VisionVelocity actived. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

/************************************
功能：	实现各种资源的释放
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionVelocity::DeactiveImp()
{
	if (NULL != m_pFeedBack)
	{
		delete []m_pFeedBack;
		m_pFeedBack = NULL;
	}
	
	LOGW("VisionVelocity deactived. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

/************************************
功能：	测速消息处理接口
参数：	无
返回：	无
************************************/
void CVisionVelocity::ProcessMsg(VISION_MSG* pMsg)
{
	if (NULL != pMsg)
	{
		unsigned int cnt = *(unsigned int*)pMsg->data.ptr;
		cout << "#### recv 101 ####" << cnt << endl;
		/*m_pImu = (char*)(pMsg->data.ptr + 8 * MAX_CLOUD_SIZE);
		
		// 测速算法接口
		RunVelocity(pMsg->data.ptr, m_Num, m_pImu, (char*)m_pFeedBack, 
			(Stereo_Info*)m_pStereoBuf, (VO_Info*)m_pVoBuf);*/
		
		CAN_VELOCITY_DATA* p = (CAN_VELOCITY_DATA*)m_pFeedBack;
		p->cnt = m_index++;
		
		// 发送算法结果
		if (-1 == SendSmallMsg(VELOCITY_BACK, (char*)p, sizeof(CAN_VELOCITY_DATA)))
		{
			cout << "!!!!! err " << endl;
		}
	}
}

/************************************
功能：	初始化配置选项
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionVelocity::InitOption()
{
	vector<OPTION>::iterator itv = m_vOption.begin();
	for (; itv != m_vOption.end(); ++itv)
	{
		if ((itv->key).compare(string("num")) == 0)
		{
			m_Num = itv->value;
		}
	}
	
	return 0;
}

int CVisionVelocity::ExecCmd(const char* strCmd)
{
	if (NULL == strCmd)
	{
		return -1;
	}
	
	FILE* pf = popen(strCmd, "r");
	if (NULL == pf)
	{
		return -1;
	}
	
	pclose(pf);
	
	return 0;
}

int CVisionVelocity::MovePath(const char* path)
{
	if (NULL == path)
	{
		return -1;
	}
	
	char szCmd[512] = {0};
	
	snprintf(szCmd, 512, "mv -f %s %s.bak", path, path);
	
	return ExecCmd(szCmd);
}



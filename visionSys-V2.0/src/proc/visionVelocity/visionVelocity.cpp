/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "visionVelocity.h"
#include <VO/system_io.h>

#define LOG_TAG "VISION_VELOCITY"

BEGAIN_MESSAGE_MAP(CVisionVelocity, CBaseVision)
	ON_COMMAND(VELOCITY_ID, &CVisionVelocity::ProcessMsg)
END_MESSAGE_MAP()

BEGAIN_TIMER_MAP(CVisionVelocity, CBaseVision)
	ON_TIMER(200000, &CBaseVision::SendHeartMsg)
END_TIMER_MAP()

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
���ܣ�	VisionVelocity���������ɳ�ʼ������
������	��
���أ�	�ɹ� 0��ʧ�� -1
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
	
	// ��ʼ��ѡ��
	InitOption();
	
	m_pVoInfo = new char[sizeof(vo_info)];
	if (NULL == m_pVoInfo)
	{
		LOGE("malloc vo info buf err(%d).", errno);
		return -1;
	}
	
	m_pBranchInfo = new char[sizeof(branch_info) * m_Num];
	if (NULL == m_pBranchInfo)
	{
		LOGE("malloc branch info buf err(%d).", errno);
		return -1;
	}
	
	// ע���㷨�ӿ�
	if (!init_vo("/cache", m_Num))
	{
		SetStatusCode(ERR_PROC_UNINTIED);
		
		LOGE("vo init err.");
		return -1;
	}
	
	// ����״̬��
	SetStatusCode(ERR_INTIALIZED);
	
	LOGW("VisionVelocity actived. %s : %d\n", __FILE__, __LINE__);
	
	return 0;
}

/************************************
���ܣ�	ʵ�ָ�����Դ���ͷ�
������	��
���أ�	�ɹ� 0��ʧ�� -1
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
���ܣ�	������Ϣ����ӿ�
������	��
���أ�	��
************************************/
void CVisionVelocity::ProcessMsg(VISION_MSG* pMsg)
{
	if (NULL != pMsg && NULL != pMsg->data.ptr)
	{
		m_pImu = (char*)(pMsg->data.ptr + 2 * IMG_SIZE);
		
		IMU_DATA* pImu = (IMU_DATA*)m_pImu;
		
		cout << pImu->acc_x << " " << pImu->q0 << " " << pImu->q1 << " " << pImu->q2 << " " << pImu->q3 << endl;
		
		// �����㷨�ӿ�
		run_vo(pMsg->data.ptr, m_pImu, 
			(vo_info*)m_pVoInfo, (branch_info*)m_pBranchInfo, (vo_can_output*)m_pFeedBack);
		
		CAN_VELOCITY_DATA* p = (CAN_VELOCITY_DATA*)m_pFeedBack;
		p->cnt = m_index++;
		
		// �����㷨���
		if (-1 == SendSmallMsg(VELOCITY_BACK, (char*)p, sizeof(CAN_VELOCITY_DATA)))
		{
			LOGE("send vo result msg err.");
		}
	}
}

/************************************
���ܣ�	��ʼ������ѡ��
������	��
���أ�	�ɹ� 0��ʧ�� -1
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



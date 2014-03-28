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
	
	// ע���㷨�ӿ�
	Register(m_Num);
	
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
	if (NULL != pMsg)
	{
		unsigned int cnt = *(unsigned int*)pMsg->data.ptr;
		cout << "#### recv 101 ####" << cnt << endl;
		/*m_pImu = (char*)(pMsg->data.ptr + 8 * MAX_CLOUD_SIZE);
		
		// �����㷨�ӿ�
		RunVelocity(pMsg->data.ptr, m_Num, m_pImu, (char*)m_pFeedBack, 
			(Stereo_Info*)m_pStereoBuf, (VO_Info*)m_pVoBuf);*/
		
		CAN_VELOCITY_DATA* p = (CAN_VELOCITY_DATA*)m_pFeedBack;
		p->cnt = m_index++;
		
		// �����㷨���
		if (-1 == SendSmallMsg(VELOCITY_BACK, (char*)p, sizeof(CAN_VELOCITY_DATA)))
		{
			cout << "!!!!! err " << endl;
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



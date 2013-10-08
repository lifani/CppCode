#include <common_interface.h>
#include <mt/mt.h>
#include <VO/demon.h>
#include "visionVelocity.h"

CCommonInterface* CreateInstance(const char* ppname, const char* pname)
{
	if (NULL == ppname || NULL == pname)
	{
		return NULL;
	}
	
	CCommonInterface* p = new CVisionVelocity(ppname, pname);
	
	return p;
}

CVelocityStore::CVelocityStore()
{
}

CVelocityStore::~CVelocityStore()
{
}

int CVelocityStore::Init(const string& path)
{
	if (-1 == CVisionStore::Init(path))
	{
		return -1;
	}
	
	string strPath = path + "/imu.txt";
	m_fd = open(strPath.c_str(), O_WRONLY | O_CREAT, 0666);
	if (-1 == m_fd)
	{
		return -1;
	}
	
	return 0;
}

void CVelocityStore::OutFile(STORE_NODE* p)
{
	char szData[512] = {0};
	snprintf(szData, sizeof(szData), "%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f\n",
		p->imu.acc_x, p->imu.acc_y, p->imu.acc_z,
		p->imu.gyro_x, p->imu.gyro_y, p->imu.gyro_z,
		p->imu.press,
		p->imu.q0, p->imu.q1, p->imu.q2, p->imu.q3);
	
	write(m_fd, szData, strlen(szData));
	
	CVisionStore::OutFile(p);
}

CVisionVelocity::CVisionVelocity(const char* ppname, const char* pname)
: CBaseVision(ppname, pname), m_bRunning(true)
{
}

CVisionVelocity::~CVisionVelocity()
{
}

int CVisionVelocity::Active()
{
	if (CBaseVision::Active() == -1)
	{
		LOGE("%s's basic class active error. %s : %d\n", m_pname.c_str(), __FILE__, __LINE__);
		return -1;
	}
	
	m_bRunning = true;
	
	if (m_Store.Init(VELOCITY_PATH))
	{
		LOGE("init store module error. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	// 注册算法
	if (!Register())
	{
		LOGE("register velocity alg error. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	// 注册线程
	RegisterPthread(&CBaseVision::Run1);
	
	LOGW("%s active success. %s : %d\n", m_pname.c_str(), __FILE__, __LINE__);
	
	return 0;
}

void CVisionVelocity::Run()
{
	VELOCITY_DATA tVelocity;
	FEEDBACK_DATA tFeedback;
	
	tFeedback.flg = IS_VELOCITY;
	
	unsigned int size = 0;
	
	unsigned short i = 0;
	while (m_bRunning)
	{
		int len = 0;
		if (-1 == (len = RecvData(m_pname, (char*)&tVelocity, &size)))
		{
			usleep(50000);
			continue;
		}
		
		if (len == 0)
		{
			usleep(50000);
			continue;
		}
		
		// 算法接口 参数：tFeedback.data
		RunVelocity((char*)tVelocity.lCloud, tVelocity.lcnt, (char*)tVelocity.rCloud, 
			tVelocity.rcnt, (char*)&tVelocity.imu, (char*)tFeedback.data);

		// 存入存储模块
		m_Store.push(tVelocity.lCloud, tVelocity.lcnt * POINT_LEN, 
			tVelocity.rCloud, tVelocity.rcnt * POINT_LEN, &tVelocity.imu);
		
		CAN_VELOCITY_DATA* pV = (CAN_VELOCITY_DATA*)tFeedback.data;
		
		tFeedback.cnt = i;
		
		pV->cnt = i;
		// 数据反馈
		size = sizeof(CAN_VELOCITY_DATA) + 2 * sizeof(int);
		memcpy((char*)&tVelocity, (char*)&tFeedback, size);
		
		++i;
	}
}

void CVisionVelocity::Run1()
{
	while(m_bRunning)
	{
		m_Store.pop();
	}
}

int CVisionVelocity::Deactive()
{
	m_bRunning = false;
	
	return CBaseVision::Deactive();
}
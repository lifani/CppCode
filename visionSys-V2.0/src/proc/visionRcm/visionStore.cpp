/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.15
*************************************/
#include "visionStore.h"

#undef LOG_TAG
#define LOG_TAG "VISION_STORE"

CVisionStore::CVisionStore(const char* path, 
	const unsigned int folders, const unsigned int files) 
: m_qCtrl(sizeof(STORE_NODE), 10, false)
, m_ppath(path)
, m_path("")
, m_imufd(0)
, m_fIndex(0)
, m_dIndex(0)
, m_folders(folders)
, m_files(files)
{
}

CVisionStore::~CVisionStore()
{
	if (0 != m_imufd)
	{
		close(m_imufd);
	}
}

int CVisionStore::Initialize()
{
	if (m_ppath.compare(string("")) == 0)
	{
		return 0;
	}
	
	char szBuf[256] = {0};
	for (unsigned int i = 0; i < m_folders; ++i)
	{
		sprintf(szBuf, "%s/%d/", m_ppath.c_str(), i);
		if (-1 == access(szBuf, F_OK))
		{
			if (CreateDir(szBuf) != 0)
			{
				LOGE("create dir %s err. %s : %d\n", szBuf, __FILE__, __LINE__);
				return -1;
			}
			
			m_path = szBuf;
			m_dIndex = i;
			
			break;
		}
	}
	
	if (m_path.compare("") == 0)
	{
		sprintf(szBuf, "%s/%d/", m_ppath.c_str(), 0);
		
		m_path = szBuf;
		m_dIndex = 0;
	}
	
	sprintf(szBuf, "%s/imu.txt", m_path.c_str());
	
	m_imufd = open(szBuf, O_WRONLY | O_CREAT, 0666);
	if (-1 == m_imufd)
	{
		LOGE("open file %s err. %s : %d\n", szBuf, __FILE__, __LINE__);
		return -1;
	}
	
	return 0;
}

int CVisionStore::Out(STORE_NODE* node)
{
	if (NULL == node)
	{
		return -1;
	}
	
	if (m_fIndex == m_files)
	{
		m_fIndex = 0;
		m_dIndex = (m_dIndex + 1) % m_folders;
		
		char szBuf[256] = {0};
		
		sprintf(szBuf, "%s/%d/", m_ppath.c_str(), m_dIndex);
		CreateDir(szBuf);
		
		m_path = szBuf;
		
		close(m_imufd);
		
		sprintf(szBuf, "%s/imu.txt", m_path.c_str());
		
		m_imufd = open(szBuf, O_WRONLY | O_CREAT, 0666);
		if (-1 == m_imufd)
		{
			return -1;
		}
	}
	
	OutImg(node);
	
	OutImu(node->imu);
	
	++m_fIndex;
	
	return 0;
}


int CVisionStore::ExecCmd(const char* strCmd)
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

int CVisionStore::CreateDir(const char* path)
{
	if (NULL == path)
	{
		return -1;
	}
	
	char szCmd[256] = {0};
	
	sprintf(szCmd, "mkdir -p %s", path);
	
	return ExecCmd(szCmd);
}

int CVisionStore::OutImg(STORE_NODE* node)
{
	char szBuf[256] = {0};
	
	for (unsigned int i = 0; i < node->cnt; ++i)
	{
		STORE_ENTRY* p = node->entries + i;
		
		sprintf(szBuf, "%s/%06d_%d.dat", m_path.c_str(), m_fIndex, i);
		
		int fd = open(szBuf, O_WRONLY | O_CREAT, 0666);
		if (-1 != fd)
		{
			write(fd, p->data, p->size);
			close(fd);
		}
	}
	
	return 0;
}

int CVisionStore::OutImu(IMU_DATA& imu)
{
	char szData[512] = {0};
	snprintf(szData, sizeof(szData), "%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f	%f\n",
		imu.acc_x, imu.acc_y, imu.acc_z,
		imu.gyro_x, imu.gyro_y, imu.gyro_z,
		imu.press,
		imu.q0, imu.q1, imu.q2, imu.q3,
		imu.vgx, imu.vgy, imu.vgz);
		
	write(m_imufd, szData, strlen(szData));
	
	return 0;
}

#include "visionStore.h"

#define LOG_TAG "VISION_STORE"

BEGAIN_MESSAGE_MAP(CVisionStore, CBaseVision)
	ON_COMMAND(STORE_FRONT, &CVisionStore::ProcessMsg)
END_MESSAGE_MAP()

DEFINE_CREATE_INSTANCE(CVisionStore)

CVisionStore::CVisionStore(const char* ppname, const char* pname) 
: CBaseVision(ppname, pname)
, m_strPath("")
, m_num(0)
, m_dIndex(0)
, m_fIndex(0)
, m_files(0)
, m_folders(0)
, m_above(0)
, m_ready(false)
, m_ptr(NULL)
, m_fetchPos(0)
, m_storePos(0)
{
	pthread_mutex_init(&m_lock, NULL);
	pthread_cond_init(&m_cond, NULL);
}

CVisionStore::~CVisionStore()
{
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_cond);
}

int CVisionStore::ActiveImp()
{
	InitOption();
	
	if (-1 == InitStore())
	{
		return -1;
	}
	
	if (-1 == InitMem())
	{
		return -1;
	}
	
	//RegisterThread(static_cast<THREAD_FUNC>(&CVisionStore::Store));
	
	LOGW("VisionStore actived. %s : %d\n", __FILE__, __LINE__);
	return 0;
}

int CVisionStore::DeactiveImp()
{
	LOGW("VisionStore Deactived. %s : %d\n", __FILE__, __LINE__);
	return 0;
}

void CVisionStore::ProcessMsg(VISION_MSG* pMsg)
{
	/*if (NULL != pMsg)
	{
		pthread_mutex_lock(&m_lock);
		
		for (int i = 0; i < m_num; ++i)
		{
			memcpy(m_ptrIndex[m_storePos], pMsg->data.ptr + i * IMG_SIZE, IMG_SIZE);
			
			++m_storePos;
		}
		
		m_ready = true;
		
		if (m_storePos == m_above)
		{
			m_storePos = 0;
		}
		pthread_mutex_unlock(&m_lock);
		pthread_cond_signal(&m_cond);
	}*/
	
	char szBuf[256] = {0};
	for (int i = 0; i < m_num; ++i)
	{
		snprintf(szBuf, 256, "%s/%06d_%d.dat", m_strPath.c_str(), m_fIndex, i);
		
		OutFile(szBuf, pMsg->data.ptr + i * IMG_SIZE, IMG_SIZE);
	}
	
	++m_fIndex;
		
	if (m_fIndex == m_files)
	{
		ResetStore();
	}
}

void CVisionStore::Store()
{
	while(IsRunning())
	{
		pthread_mutex_lock(&m_lock);
		while(!m_ready)
		{
			pthread_cond_wait(&m_cond, &m_lock);
		}
		
		char szBuf[256] = {0};
		for (int i = 0; i < m_num && m_fetchPos < m_above; ++i, ++m_fetchPos)
		{
			snprintf(szBuf, 256, "%s/%06d_%d.dat", m_strPath.c_str(), m_fIndex, i);
			
			OutFile(szBuf, m_ptrIndex[m_fetchPos], IMG_SIZE);
		}
		
		if (m_fetchPos == m_above)
		{
			m_fetchPos = 0;
		}
		
		if (m_fetchPos == m_storePos)
		{
			m_ready = false;
		}
		
		++m_fIndex;
		
		if (m_fIndex == m_files)
		{
			ResetStore();
		}
		
		pthread_mutex_unlock(&m_lock);
	}
}

/************************************
功能：	初始化配置选项
参数：	无
返回：	成功 0，失败 -1
************************************/
int CVisionStore::InitOption()
{
	vector<OPTION>::iterator itv = m_vOption.begin();
	for (; itv != m_vOption.end(); ++itv)
	{
		if ((itv->key).compare(string("folder")) == 0)
		{
			m_folders = itv->value;
		}
		
		if ((itv->key).compare(string("file")) == 0)
		{
			m_files = itv->value;
		}
		
		if ((itv->key).compare(string("num")) == 0)
		{
			m_num = itv->value;
		}
	}
	
	return 0;
}

int CVisionStore::InitStore()
{
	char szBuf[256] = {0};
	for (int i = 0; i < m_folders; ++i)
	{
		//sprintf(szBuf, "%s/%d/", VELOCITY_PATH, i);
		sprintf(szBuf, "%s/%d/", "/mnt/disk", i);
		if (-1 == access(szBuf, F_OK))
		{
			m_strPath = szBuf;
			m_dIndex = i;
			
			break;
		}
	}
	
	if (m_strPath.compare("") == 0)
	{
		//sprintf(szBuf, "%s/%d/", VELOCITY_PATH, 0);
		sprintf(szBuf, "%s/%d/", "/mnt/disk", 0);
		RemoveDir(szBuf);
		
		m_strPath = szBuf;
		m_dIndex = 0;
	}
	
	if (CreateDir(szBuf) != 0)
	{
		LOGE("create dir %s err. %s : %d\n", szBuf, __FILE__, __LINE__);
		return -1;
	}
	
	cout << m_strPath << endl;
	
	return 0;
}

int CVisionStore::InitMem()
{
	m_ptr = new char[MEM_SIZE];
	if (NULL == m_ptr)
	{
		LOGE("alloc memory err. %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	for (unsigned int i = 0; i < MEM_CNT; ++i)
	{
		m_ptrIndex[i] = m_ptr + i * 76800;
	}
	
	m_above = MEM_CNT - MEM_CNT % m_num;
	
	return 0;
}

int CVisionStore::ResetStore()
{
	if (++m_dIndex == m_folders)
	{
		m_dIndex = 0;
	}
	
	char szBuf[256] = {0};
	snprintf(szBuf, 256, "%s/%d", VELOCITY_PATH, m_dIndex);
	
	RemoveDir(szBuf);
	
	if (CreateDir(szBuf) != 0)
	{
		LOGE("create dir %s err. %s : %d\n", szBuf, __FILE__, __LINE__);
	}
	
	m_strPath = szBuf;
	m_fIndex = 0;
	
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

int CVisionStore::RemoveDir(const char* path)
{
	if (NULL == path)
	{
		return -1;
	}
	
	char szCmd[256] = {0};
	snprintf(szCmd, 256, "rm -r -f %s", path);
	
	return ExecCmd(szCmd);
}

int CVisionStore::OutFile(const char* path, const char* ptr, int len)
{
	if (NULL == path || NULL == path || len < 0)
	{
		return -1;
	}
	
	FILE* pf = fopen(path, "wb");
	if (NULL == pf)
	{
		return -1;
	}
	
	fwrite(ptr, len, 1, pf);
	
	fclose(pf);

	return 0;
}

/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "shmTrans.h"

CShmTrans* CShmTrans::m_pShmTrans = NULL;
char* CShmCtrl::m_lastPtr = NULL;

CShmCtrl::CShmCtrl(MSG_CONFIG& tMsgConfig) : m_tMsgConfig(tMsgConfig), m_uCnt(0), m_pHead(NULL), m_pData(NULL), m_id(0)
{
}

CShmCtrl::~CShmCtrl()
{
}

/************************************
功能：	管理类初始化
参数：	ptr char*& 存储地址
返回：	无
************************************/
void CShmCtrl::Init(char*& ptr)
{
	if (NULL == ptr)
	{
		return;
	}
	
	unsigned int size = m_tMsgConfig.size * m_tMsgConfig.cnt;
	if (size > 0)
	{
		m_pHead = (SHM_HEAD*)ptr;
		
		if (m_pHead->m_initialized == 0)
		{
			m_pHead->m_initialized = 1;
			
			m_pHead->m_size = m_tMsgConfig.size;
			if (1 == m_tMsgConfig.imu)
			{
				m_pHead->m_size += sizeof(IMU_DATA);
			}
			
			m_pHead->m_cnt = 0;
			m_pHead->m_fetch = 0;
			m_pHead->m_store = 0;
		}

		m_pData = ptr + sizeof(SHM_HEAD);
		m_uCnt = m_tMsgConfig.cnt;
		m_id = m_tMsgConfig.id;
		
		size = m_pHead->m_size * m_tMsgConfig.cnt + sizeof(SHM_HEAD);
		ptr += size;
		
		m_lastPtr = ptr;
	}
}

/************************************
功能：	存储消息
参数：	*pMsg VISION_MSG 消息指针, 不能为空
返回：	成功 0，失败 -1
************************************/
int CShmCtrl::push(VISION_MSG* pMsg)
{
	if (NULL == pMsg->data.ptr)
	{
		return -1;
	}
	
	if (m_pHead->m_cnt == m_uCnt)
	{
		return -1;
	}
	
	unsigned int len = pMsg->data.size > m_pHead->m_size ? m_pHead->m_size : pMsg->data.size;
	
	char* p = m_pData + m_pHead->m_store * m_pHead->m_size;
	memcpy(p, pMsg->data.ptr, len);
	
	++m_pHead->m_store;
	if (m_pHead->m_store == m_uCnt)
	{
		m_pHead->m_store = 0;
	}
	
	++m_pHead->m_cnt;
	
	return 0;
}

/************************************
功能：	取消息
参数：	*pMsg VISION_MSG 消息指针, 不能为空
返回：	成功 0，失败 -1
************************************/
int CShmCtrl::pop(VISION_MSG* pMsg)
{
	if (NULL == pMsg->data.ptr)
	{
		return -1;
	}

	if (m_pHead->m_cnt == 0)
	{
		return -1;
	}
	
	unsigned int len = pMsg->data.size > m_pHead->m_size ? m_pHead->m_size : pMsg->data.size;
	
	char* p = m_pData + m_pHead->m_fetch * m_pHead->m_size;
	memcpy(pMsg->data.ptr, p, len);
	
	++m_pHead->m_fetch;
	if (m_pHead->m_fetch == m_uCnt)
	{
		m_pHead->m_fetch = 0;
	}
	
	--m_pHead->m_cnt;

	return 0;
}

/************************************
功能：	创建CShmTrans实例
参数：	vMsgConfig vector<MSG_CONFIG>& 消息配置信息
		key key_t IPC key值
返回：	成功 0，失败 -1
************************************/
CShmTrans* CShmTrans::CreateInstance(vector<MSG_CONFIG>& vMsgConfig, key_t key)
{
	if (NULL == m_pShmTrans)
	{
		m_pShmTrans = new CShmTrans;
		if (NULL != m_pShmTrans && m_pShmTrans->Init(vMsgConfig, key) == -1)
		{
			delete m_pShmTrans;
			m_pShmTrans = NULL;
		}
	}
	
	return m_pShmTrans;
}

CShmTrans::CShmTrans() : m_shmid(0), m_pShmData(NULL), m_pOutData(NULL)
{
}

CShmTrans::~CShmTrans()
{
	if (NULL != m_pOutData)
	{
		delete m_pOutData;
		m_pOutData = NULL;
	}
	
	shmdt(m_pShmData);
		
	struct shmid_ds ds;
	if (shmctl(m_shmid, IPC_STAT, &ds) == 0)
	{
		if (ds.shm_nattch == 0)
		{
			shmctl(m_shmid, IPC_RMID, 0);
		}
	}
	
	// m_pShmData 为共享内存
}

/************************************
功能：	初始化
参数：	vMsgConfig vector<MSG_CONFIG>& 消息配置信息
		key key_t IPC key值
返回：	成功 0，失败 -1
************************************/
int CShmTrans::Init(vector<MSG_CONFIG>& vMsgConfig, key_t key)
{
	unsigned int sum_size = 0;
	unsigned int max_size = 0;
	unsigned int imu_size = 0;
	
	// 消息暗道
	sum_size += sizeof(SHM_DEAMON);
	
	if (m_Sem.Init(key) == -1)
	{
		return -1;
	}
	
	vector<MSG_CONFIG>::iterator itv = vMsgConfig.begin();
	for (; itv != vMsgConfig.end(); ++itv)
	{
		CShmCtrl* p = new CShmCtrl(*itv);
		if (NULL == p)
		{
			return -1;
		}
		
		m_mapShmCtrl[itv->id] = p;
		
		imu_size = itv->imu == 0 ? 0 : sizeof(IMU_DATA);
		
		unsigned int size = (itv->size + imu_size) * itv->cnt + sizeof(SHM_HEAD);
		if (itv->size + imu_size > max_size)
		{
			max_size = itv->size + imu_size;
		}
		
		sum_size += size;
	}
	
	m_pOutData = new char[max_size + 1];
	if (NULL == m_pOutData)
	{
		return -1;
	}
	
	m_pOutData[max_size] = '\0';
	
	if ((m_shmid = shmget(key, sum_size, IPC_CREAT | SHM_R | SHM_W)) == -1)
	{
		return -1;
	}
	
	if ((m_pShmData = (char*)shmat(m_shmid, 0, 0)) == (void*)-1)
	{
		m_pShmData = NULL;
		return -1;
	}
	
	char* p = m_pShmData + sizeof(SHM_DEAMON);
	
	map<long, CShmCtrl*>::iterator itm = m_mapShmCtrl.begin();
	for (; itm != m_mapShmCtrl.end(); ++itm)
	{
		itm->second->Init(p);
	}
	
	return 0;
}

/************************************
功能：	接收消息
参数：	*pMsg VISION_MSG 消息指针, 不能为空
返回：	成功 0，失败 -1
************************************/
int CShmTrans::ReadMsg(VISION_MSG* pMsg)
{
	if (NULL == pMsg->data.ptr || 0 == pMsg->data.size)
	{
		return 0;
	}
	
	// 消息暗道
	if (DEAMON_ID == pMsg->id)
	{
		return ReadDeamonMsg(pMsg);
	}
	
	CShmCtrl* pShmCtrl = m_mapShmCtrl[pMsg->id];
	if (NULL != pShmCtrl)
	{
		pMsg->data.ptr = m_pOutData;
		
		m_Sem.P();
		if (pShmCtrl->pop(pMsg) == -1)
		{
			m_Sem.V();
			return -1;
		}
		m_Sem.V();
	}
	else
	{
		return -1;
	}
	
	return 0;
}

/************************************
功能：	接收消息
参数：	*pMsg VISION_MSG 消息指针, 不能为空
返回：	成功 0，失败 -1
************************************/
int CShmTrans::WriteMsg(VISION_MSG* pMsg)
{
	if (NULL == pMsg->data.ptr || 0 == pMsg->data.size)
	{
		return 0;
	}
	
	// 消息暗道
	if (DEAMON_ID == pMsg->id)
	{
		return WriteDeamonMsg(pMsg);
	}
	
	CShmCtrl* pShmCtrl = m_mapShmCtrl[pMsg->id];
	if (NULL != pShmCtrl)
	{
		m_Sem.P();
		if (pShmCtrl->push(pMsg) == -1)
		{
			m_Sem.V();
			return -1;
		}
		m_Sem.V();
	}
	else
	{
		return -1;
	}
	
	return 0;
}

/************************************
功能：	读取暗道信息
参数：	*pMsg VISION_MSG 消息指针, 不能为空
返回：	成功 0，失败 -1
************************************/
int CShmTrans::ReadDeamonMsg(VISION_MSG* pMsg)
{
	if (NULL == pMsg->data.ptr || 0 == pMsg->data.size)
	{
		return 0;
	}
	
	int len = sizeof(SHM_DEAMON) > pMsg->data.size ? pMsg->data.size : sizeof(SHM_DEAMON);
	
	m_Sem.P();
	
	memcpy(pMsg->data.ptr, m_pShmData, len);
	
	pMsg->data.size = len;
	
	m_Sem.V();

	return 0;
}

/************************************
功能：	写暗道信息
参数：	*pMsg VISION_MSG 消息指针, 不能为空
返回：	成功 0，失败 -1
************************************/
int CShmTrans::WriteDeamonMsg(VISION_MSG* pMsg)
{
	if (NULL == pMsg->data.ptr || 0 == pMsg->data.size)
	{
		return 0;
	}
	
	int len = sizeof(SHM_DEAMON) > pMsg->data.size ? pMsg->data.size : sizeof(SHM_DEAMON);
	
	m_Sem.P();
	
	memcpy(m_pShmData, pMsg->data.ptr, len);
	
	pMsg->data.size = len;
	
	m_Sem.V();
	
	return 0;
}

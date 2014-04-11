/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#include "shmTrans.h"

CShmTrans* CShmTrans::m_pShmTrans = NULL;

CShmCtrl::CShmCtrl(int fd) : LCK_SIZE(MSG_MEM_SIZE / 2), m_fd(fd), m_pHead(NULL), m_pData(NULL)
{
}

CShmCtrl::~CShmCtrl()
{
}

/************************************
���ܣ�	�������ʼ��
������	ptr char*& �洢��ַ
���أ�	��
************************************/
void CShmCtrl::Init(char*& ptr, off_t offset)
{
	if (NULL == ptr)
	{
		return;
	}
	
	m_offset = offset;
	
	m_pHead = (SHM_HEAD*)ptr;
	m_pData = ptr + sizeof(SHM_HEAD);
	
	m_pHead->m_fetch = 0;
	m_pHead->m_store = 0;
	m_pHead->m_total = 0;
}

/************************************
���ܣ�	�洢��Ϣ
������	*pMsg VISION_MSG ��Ϣָ��, ����Ϊ��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CShmCtrl::push(VISION_MSG* pMsg)
{
	if (NULL == pMsg->data.ptr)
	{
		return -1;
	}
	
	// ����
	if (writew_lock(m_fd, m_offset, MSG_MEM_SIZE) == -1)
	{
		LOGE("lock err(errno). %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	if (m_pHead->m_total == MAX_MSG)
	{
		LOGE("content reach max, id = %ld. %s : %d\n", pMsg->id, __FILE__, __LINE__);
		
		// ����
		un_lock(m_fd, m_offset, MSG_MEM_SIZE);
		return -1;
	}
	
	memcpy(m_pData + m_pHead->m_store * LCK_SIZE, pMsg->data.ptr, pMsg->data.x.size);
	
	m_pHead->m_fetch = m_pHead->m_store;
	m_pHead->m_store = (m_pHead->m_store + 1) % 2;
	m_pHead->m_total++;
	
	// ����
	un_lock(m_fd, m_offset, MSG_MEM_SIZE);
	
	return 0;
}

/************************************
���ܣ�	ȡ��Ϣ
������	*pMsg VISION_MSG ��Ϣָ��, ����Ϊ��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CShmCtrl::pop(VISION_MSG* pMsg)
{
	if (NULL == pMsg->data.ptr)
	{
		return -1;
	}
	
	if (writew_lock(m_fd, m_offset, MSG_MEM_SIZE) == -1)
	{
		LOGE("lock err(errno). %s : %d\n", __FILE__, __LINE__);
		return -1;
	}
	
	if (m_pHead->m_total == 0)
	{
		LOGE("no content. %s : %d\n", __FILE__, __LINE__);
		
		// ����
		un_lock(m_fd, m_offset, MSG_MEM_SIZE);
		return -1;
	}
	
	memcpy(pMsg->data.ptr, m_pData + m_pHead->m_fetch * LCK_SIZE, pMsg->data.x.size);
	
	m_pHead->m_total--;
	
	un_lock(m_fd, m_offset, MSG_MEM_SIZE);	

	return 0;
}

int CShmCtrl::lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
	struct flock lock;
	
	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;
	
	return fcntl(fd, cmd, &lock);
}

/************************************
���ܣ�	����CShmTransʵ��
������	const map<long, MSG_TAG*>& ��Ϣ������Ϣ
		key key_t IPC keyֵ
���أ�	�ɹ� 0��ʧ�� -1
************************************/
CShmTrans* CShmTrans::CreateInstance(const map<long, MSG_TAG*>& mapPMsgTag, key_t key)
{
	if (NULL == m_pShmTrans)
	{
		m_pShmTrans = new CShmTrans;
		if (NULL != m_pShmTrans && m_pShmTrans->Init(mapPMsgTag, key) == -1)
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
	
	map<long, CShmCtrl*>::iterator itm = m_mapShmCtrl.begin();
	for (; itm != m_mapShmCtrl.end(); ++itm)
	{
		if (NULL != itm->second)
		{
			delete itm->second;
			itm->second = NULL;
		}
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
	
	// m_pShmData Ϊ�����ڴ�
}

/************************************
���ܣ�	��ʼ��
������	const map<string, PROC_TAG>& ��Ϣ������Ϣ
		key key_t IPC keyֵ
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CShmTrans::Init(const map<long, MSG_TAG*>& mapPMsgTag, key_t key)
{
	m_fd = open(RECORD_LOCKING, O_CREAT | O_TRUNC | O_RDWR, 0666);
	if (-1 == m_fd)
	{
		LOGE("open locking file err(%d). %s : %d\n", errno, __FILE__, __LINE__);
		return -1;
	}
	
	unsigned int sum_size = 0;
	
	// ������Ϣ�б�
	map<long, MSG_TAG*>::const_iterator itm = mapPMsgTag.begin();
	for (; itm != mapPMsgTag.end(); ++itm)
	{
		if (NULL != itm->second && itm->second->isBig)
		{
			sum_size += MSG_MEM_SIZE;
			
			m_mapShmCtrl[itm->first] = new CShmCtrl(m_fd);
			if (NULL == m_mapShmCtrl[itm->first])
			{
				LOGE("malloc mem err(%d). %s : %d\n", errno, __FILE__, __LINE__);
				return -1;
			}
		}
	}
	
	// ���������ڴ�
	if ((m_shmid = shmget(key, sum_size, IPC_CREAT | SHM_R | SHM_W)) == -1)
	{
		LOGE("get shm id err(%d). %s : %d\n", errno, __FILE__, __LINE__);
		return -1;
	}
	
	if ((m_pShmData = (char*)shmat(m_shmid, 0, 0)) == (void*)-1)
	{
		LOGE("mat shm err(%d). %s : %d\n", errno, __FILE__, __LINE__);
		m_pShmData = NULL;
		return -1;
	}
	
	// ��ʼ��������
	char* p = m_pShmData;
	
	map<long, CShmCtrl*>::iterator it_ctrl = m_mapShmCtrl.begin();
	for (; it_ctrl != m_mapShmCtrl.end(); ++it_ctrl)
	{
		it_ctrl->second->Init(p, p - m_pShmData);
		
		p += MSG_MEM_SIZE;
	}
	
	m_pOutData = new char[MSG_MEM_SIZE];
	if (NULL == m_pOutData)
	{
		LOGE("malloc mem err(%d). %s : %d\n", errno, __FILE__, __LINE__);
		return -1;
	}
	
	return 0;
}

/************************************
���ܣ�	������Ϣ
������	*pMsg VISION_MSG ��Ϣָ��, ����Ϊ��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CShmTrans::ReadMsg(VISION_MSG* pMsg)
{
	if (NULL == pMsg->data.ptr || 0 == pMsg->data.x.size)
	{
		return 0;
	}
	
	CShmCtrl* pShmCtrl = m_mapShmCtrl[pMsg->id];
	if (NULL != pShmCtrl)
	{
		pMsg->data.ptr = m_pOutData;

		if (pShmCtrl->pop(pMsg) == -1)
		{
			LOGE("read msg %ld content err(%d). %s : %d\n", pMsg->id, errno, __FILE__, __LINE__);
			return -1;
		}
	}
	else
	{
		return -1;
	}
	
	return 0;
}

/************************************
���ܣ�	������Ϣ
������	*pMsg VISION_MSG ��Ϣָ��, ����Ϊ��
���أ�	�ɹ� 0��ʧ�� -1
************************************/
int CShmTrans::WriteMsg(VISION_MSG* pMsg)
{
	if (NULL == pMsg->data.ptr || 0 == pMsg->data.x.size)
	{
		return 0;
	}

	CShmCtrl* pShmCtrl = m_mapShmCtrl[pMsg->id];
	if (NULL != pShmCtrl)
	{
		if (pShmCtrl->push(pMsg) == -1)
		{
			LOGE("write msg %ld content err(%d). %s : %d\n", pMsg->id, errno, __FILE__, __LINE__);
			return -1;
		}
	}
	else
	{
		return -1;
	}
	
	return 0;
}

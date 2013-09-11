#include <typedef.h>
#include "shmtrans.h"

CShmTrans::CShmTrans(unsigned int size, unsigned int size_out, unsigned int num, key_t key) 
: m_initialized(-1) 
, m_num(num)
, m_size(size)
, m_size_out(size_out)
, m_tSize(0)
, m_pHead(NULL)
, m_shmid(-1)
, m_ptr(NULL)
, m_key(key)
{

}

CShmTrans::~CShmTrans()
{
	if (m_initialized == 0)
	{
		shmdt(m_ptr);
		
		struct shmid_ds ds;
		if (shmctl(m_shmid, IPC_STAT, &ds) == 0)
		{
			if (ds.shm_nattch == 0)
			{
				shmctl(m_shmid, IPC_RMID, 0);
			}
		}
		
		m_initialized = -1;
	}
}

int CShmTrans::Init()
{
	if (m_sem.Init(m_key) == -1)
	{
		return -1;
	}
	
	m_tSize = m_size * m_num + sizeof(Head);
	if ((m_shmid = shmget(m_key, m_tSize + m_size_out, IPC_CREAT | SHM_R | SHM_W)) == -1)
	{
		return -1;
	}
	
	if ((m_ptr = (char*)shmat(m_shmid, 0, 0)) == (void*)-1)
	{
		m_ptr = NULL;
		return -1;
	}
	
	m_pHead = (Head*)m_ptr;
	
	InitMem();
	
	m_initialized = 0;
	
	return 0;
}

void CShmTrans::destory(CTransData* p)
{
	delete p;
}

int CShmTrans::write(char* ptr, unsigned int* size)
{
	if (NULL == m_ptr)
	{
		return -1;
	}
	
	int len = *size > m_size ? m_size : *size;
	
	m_sem.P();

	if(m_pHead->cnt == m_num)
	{
		len = 0;
	}
	else
	{
		char* storePtr = m_ptr + sizeof(Head) + m_pHead->store_pos * m_size;
		memcpy(storePtr, ptr, len);

		++m_pHead->store_pos;
		++m_pHead->cnt;
		
		if (m_pHead->store_pos == m_num)
		{
			m_pHead->store_pos = 0;
		}
	}
	
	if (m_size_out > 0)
	{
		memcpy(ptr, m_ptr + m_tSize, m_size_out);
		*size = m_size_out;
	}
	
	m_sem.V();
	
	return len;
}

int CShmTrans::read(char* ptr, unsigned int* size)
{
	if (NULL == m_ptr)
	{
		return -1;
	}
	
	int len = *size > m_size_out ? m_size_out : *size;
	
	m_sem.P();
	
	if (m_size_out > 0 && len > 0)
	{
		memcpy(m_ptr + m_tSize, ptr, len);
	}
	
	len = m_size;
	if (m_pHead->cnt == 0)
	{
		len = 0;
	}
	else
	{
		char* fetchPtr = m_ptr + sizeof(Head) + m_pHead->fetch_pos * m_size;
		memcpy(ptr, fetchPtr, m_size);
		
		++m_pHead->fetch_pos;
		--m_pHead->cnt;
		
		if (m_pHead->fetch_pos == m_num)
		{
			m_pHead->fetch_pos = 0;
		}
	}
	
	m_sem.V();
	
	return len;
}

void CShmTrans::InitMem()
{
	m_sem.P();
	if (0xAA != m_pHead->magic)
	{
		m_pHead->magic = 0xAA;
		m_pHead->fetch_pos = 0;
		m_pHead->store_pos = 0;
		m_pHead->cnt = 0;
	}
	m_sem.V();
}
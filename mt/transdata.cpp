#include "transdata.h"

CTransData::CTransData(unsigned int size, unsigned int num, key_t key) 
: m_initialized(-1) 
, m_num(num)
, m_size(size)
, m_hSize(2 * sizeof(int) + sizeof(char))
, m_pFetch_pos(NULL)
, m_pStore_pos(NULL)
, m_shmid(-1)
, m_ptr(NULL)
, m_key(key)
{

}

CTransData::~CTransData()
{
	if (m_initialized == 0)
	{
		shmdt(m_ptr);
		shmctl(m_shmid, IPC_RMID, 0);
		m_initialized = -1;
	}
}

int CTransData::Init()
{
	if (m_sem.Init(m_key) == -1)
	{
		return -1;
	}
	
	unsigned int tSize = m_size * m_num + 2 * sizeof(int) + sizeof(char);
	if ((m_shmid = shmget(m_key, tSize, IPC_CREAT | SHM_R | SHM_W)) == -1)
	{
		return -1;
	}
	
	if ((m_ptr = (char*)shmat(m_shmid, 0, 0)) == (void*)-1)
	{
		m_ptr = NULL;
		return -1;
	}
	
	m_pFetch_pos = (int*)(m_ptr + sizeof(char));
	m_pStore_pos = (int*)(m_ptr + sizeof(char) + sizeof(int));
	
	InitMem();
	
	m_initialized = 0;
	
	return 0;
}

void CTransData::destory(CTransData* p)
{
	delete p;
}

int CTransData::write(const char* ptr, unsigned int size)
{
	if (NULL == m_ptr)
	{
		return -1;
	}
	
	int len = size > m_size ? m_size : size;
	
	m_sem.P();

	if(*m_pStore_pos != *m_pFetch_pos)
	{
		char* storePtr = m_ptr + m_hSize + *m_pStore_pos * m_size;
		memcpy(storePtr, ptr, len);
		
		if (*m_pFetch_pos == -1)
		{
			*m_pFetch_pos = 0;
		}
		
		++*m_pStore_pos;
		if (*m_pStore_pos == static_cast<int>(m_num))
		{
			*m_pStore_pos = 0;
		}
	}
	else
	{
		len = 0;
	}
	
	m_sem.V();
	
	return len;
}

int CTransData::read(char* ptr, unsigned int size)
{
	if (NULL == m_ptr)
	{
		return -1;
	}
	
	int len = size > m_size ? m_size : size;
	
	m_sem.P();
	memcpy(ptr, m_ptr, len);

	if (*m_pFetch_pos == *m_pStore_pos)
	{
		len = 0;
	}
	else
	{
		char* fetchPtr = m_ptr + m_hSize + *m_pFetch_pos * m_size;
		memcpy(ptr, fetchPtr, len);
		
		++*m_pFetch_pos;
	}
	
	m_sem.V();
	
	return len;
}

void CTransData::InitMem()
{
	m_sem.P();
	if (0xAA != *m_ptr)
	{
		*m_ptr = 0xAA;
		
		*m_pFetch_pos = -1;
		*m_pStore_pos = 0;
	}
	m_sem.V();
}
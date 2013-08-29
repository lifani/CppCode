#include "vSem.h"

CvSem::CvSem() : m_semid(0), m_initialized(-1)
{

}

CvSem::~CvSem()
{
	if (m_initialized == 0)
	{
		semctl(m_semid, 0, IPC_RMID);
	}
}

int CvSem::Init(key_t key)
{
	if (0 == m_initialized)
	{
		return 0;
	}

	if ((m_semid = semget(key, 1, 0666 | IPC_CREAT)) == -1)
	{
		return -1;
	}
	
	union semun arg;
	arg.val = 1;
	
	if (-1 == semctl(m_semid, 0, SETVAL, arg))
	{
		return -1;
	}
	
	m_initialized = 0;
	
	return 0;
}

int CvSem::P()
{
	if (m_initialized == -1)
	{
		return -1;
	}
	
	struct sembuf opbuf;
	opbuf.sem_num = 0;
	opbuf.sem_op = -1;
	opbuf.sem_flg = SEM_UNDO;
	
	return semop(m_semid, &opbuf, 1);
}

int CvSem::V()
{
	if (m_initialized == -1)
	{
		return -1;
	}
	
	struct sembuf opbuf;
	opbuf.sem_num = 0;
	opbuf.sem_op = 1;
	opbuf.sem_flg = SEM_UNDO;
	
	return semop(m_semid, &opbuf, 1);
}
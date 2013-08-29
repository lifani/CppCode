#ifndef __TRANSDATA_H__
#define __TRANSDATA_H__

#include "vSem.h"

// ½ûÖ¹Õ»¶ÔÏó

class CTransData
{
public :
	CTransData(unsigned int size, unsigned int num, key_t key);

	int Init();
	
	int write(const char* ptr, unsigned int size);
	
	int read(char* ptr, unsigned int size);
	
	void destory(CTransData* p);
	
private :

	~CTransData();
	
	void InitMem();
	
private :
	int m_initialized;

	unsigned int m_num;
	unsigned int m_size;
	
	const unsigned int m_hSize;
	
	int* m_pFetch_pos;
	int* m_pStore_pos;
	
	int m_shmid;
	char* m_ptr;
	
	key_t m_key;
	
	CvSem m_sem;
};

#endif

#ifndef __TRANSDATA_H__
#define __TRANSDATA_H__

#include "vSem.h"

#pragma pack(1)
typedef struct _Head
{
	unsigned char magic;
	unsigned int fetch_pos;
	unsigned int store_pos;
	unsigned int cnt;
} Head;
#pragma pack()

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

	const unsigned int m_num;
	const unsigned int m_size;
	
	Head* m_pHead;
	
	int m_shmid;
	char* m_ptr;
	
	key_t m_key;
	
	CvSem m_sem;
};

#endif

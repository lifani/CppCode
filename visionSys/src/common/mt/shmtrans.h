#ifndef __TRANSDATA_H__
#define __TRANSDATA_H__

#include "transdata.h"
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

class CShmTrans : public CTransData
{
public :
	CShmTrans(unsigned int size, unsigned int size_out, unsigned int num, key_t key);

	virtual int Init();
	
	virtual int write(char* ptr, unsigned int* size);
	
	virtual int read(char* ptr, unsigned int* size);
	
	virtual void destory(CTransData* p);
	
private :

	virtual ~CShmTrans();
	
	void InitMem();
	
private :
	int m_initialized;

	const unsigned int m_num;
	const unsigned int m_size;
	
	const unsigned int m_size_out;
	
	unsigned int m_tSize;
	
	Head* m_pHead;
	
	int m_shmid;
	char* m_ptr;
	
	key_t m_key;
	
	CvSem m_sem;
};

#endif

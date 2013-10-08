#ifndef __VISION_STORE_H__
#define __VISION_STORE_H__

#include <typedef.h>
#include <datatype.h>

typedef struct _STORE_NODE
{
	unsigned char lImg[IMG_SIZE];
	unsigned char rImg[IMG_SIZE];
	
	unsigned int lLen;
	unsigned int rLen;
	
	IMU imu;
	
	unsigned int index;
	
} STORE_NODE;

class CVisionStore
{
public :

	CVisionStore();
	
	virtual ~CVisionStore();
	
	virtual int Init(const string& path);
	
	virtual int push(unsigned char* lptr, unsigned int lLen, unsigned char* rptr, unsigned int rLen, IMU* pImu = NULL);
	
	virtual int pop();

	virtual void OutFile(STORE_NODE* p);
	
private :

	bool OutImg(const char* pData, unsigned int size, const char* szPath);
	
private :

	STORE_NODE* m_StoreQueue[STORE_QUUE_SIZE];

	unsigned int m_cnt;
	unsigned int m_fetch;
	unsigned int m_store;
	unsigned int m_index;
	
	string m_path;
	
	static pthread_mutex_t m_lock;
	static pthread_cond_t m_ready;
};

#endif

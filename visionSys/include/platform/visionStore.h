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
	
	unsigned int index;
	
} STORE_NODE;

class CVisionStore
{
public :

	CVisionStore();
	
	~CVisionStore();
	
	int Init(const string& path);
	
	int push(unsigned char* lptr, unsigned int lLen, unsigned char* rptr, unsigned int rLen);
	
	int pop();

private :

	void OutFile(STORE_NODE* p);

	bool OutImg(const char* pData, unsigned int size, const char* szPath);
	
private :

	STORE_NODE* m_StoreQueue[QUEUE_SIZE];

	unsigned int m_cnt;
	unsigned int m_fetch;
	unsigned int m_store;
	unsigned int m_index;
	
	string m_path;
	
	static pthread_mutex_t m_lock;
	static pthread_cond_t m_ready;
};

#endif

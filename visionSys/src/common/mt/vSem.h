#ifndef __SEM_H__
#define __SEM_H__

#include "../../../include/typedef.h"

union semun
{
    int val ;
    struct semid_ds * buf ;
    unsigned short int * array ;
};

// 禁止产生堆对象

class CvSem
{
public :
	CvSem();
	
	~CvSem();
	
	int Init(key_t key);
	
	int P();
	
	int V();
	
private :

	void* operator new(size_t size)
    {
        return malloc(size);
    }

    void operator delete(void* p)
    {
        free(p);
    }

	
private :

	int m_semid;
	
	int m_initialized;
};

#endif

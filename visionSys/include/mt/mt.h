#ifndef __MT_H__
#define __MT_H__

#include <typedef.h>

#define SHM_MODE 0
#define SEM_MODE 1
#define SCK_MODE 2



class CMt
{
public :
	static int mt_init(int mode, const char* path = ".", int id = 0, unsigned int size = 0, unsigned int size_out = 0, int num = 10);

	static int mt_send(key_t tid, char* ptr, unsigned int* size);

	static int mt_recv(key_t tid, char* ptr, unsigned int* size);
	
	static void mt_destory(key_t tid);
};

#endif


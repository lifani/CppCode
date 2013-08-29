#ifndef __MT_H__
#define __MT_H__

class CMt
{
public :
	static int shm_tm_init(unsigned int size = 0, const char* path = ".", int id = 0, int num = 10);

	static int shm_tm_send(int tid, const char* ptr, unsigned int size);

	static int shm_tm_recv(int tid, char* ptr, unsigned int size);
	
	static void shm_tm_destory(int tid);
};

#endif
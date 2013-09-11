#ifndef __MT_H__
#define __MT_H__

#define SHM_MODE 0
#define SEM_MODE 1
#define SCK_MODE 2



class CMt
{
public :
	static int mt_init(int mode, const char* path = ".", int id = 0, unsigned int size = 0, unsigned int size_out = 0, int num = 10);

	static int mt_send(int tid, char* ptr, unsigned int* size);

	static int mt_recv(int tid, char* ptr, unsigned int* size);
	
	static void mt_destory(int tid);
};

#endif


#include "mt.h"
#include "transdata.h"

using namespace std;

static map<int, CTransData*> g_MapTransData;

int CMt::shm_tm_init(unsigned int size, const char* path, int id, int num)
{	
	key_t key = ftok(path, id);
	
	map<int, CTransData*>::iterator it = g_MapTransData.find(key);
	if (it == g_MapTransData.end())
	{
		g_MapTransData[key] = new CTransData(size, num, key);
	}
	
	return key;
}

int CMt::shm_tm_send(int tid, const char* ptr, unsigned int size)
{
	CTransData* p = g_MapTransData[tid];
	if (NULL == p)
	{
		return -1;
	}
	
	return p->write(ptr, size);
}

int CMt::shm_tm_recv(int tid, char* ptr, unsigned int size)
{
	CTransData* p = g_MapTransData[tid];
	if (NULL == p)
	{
		return -1;
	}

	return p->read(ptr, size);
}

void CMt::shm_tm_destory(int tid)
{
	map<int, CTransData*>::iterator it = g_MapTransData.find(tid);
	if (it != g_MapTransData.end())
	{
		it->second->destory(it->second);
		it->second = NULL;
		
		g_MapTransData.erase(it);
	}
}
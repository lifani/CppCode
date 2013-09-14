#include <mt/mt.h>
#include "transdata.h"
#include "shmtrans.h"
#include "msgtrans.h"

using namespace std;

static map<key_t, CTransData*> g_MapTransData;

int CMt::mt_init(int mode, const char* path , int id , unsigned int size , unsigned int size_out, int num )
{	
	key_t key = ftok(path, id);
	
	map<key_t, CTransData*>::iterator it = g_MapTransData.find(key);
	if (it == g_MapTransData.end())
	{
		CTransData* pTransData = NULL;
		switch (mode)
		{
		case SHM_MODE:
			pTransData = new CShmTrans(size, size_out, num, key);
			
			break;
		case SEM_MODE:
			pTransData = new CMsgTrans(key);
			break;
		case SCK_MODE:
			break;
		default:
			break;
		}
		
		if (NULL == pTransData)
		{
			return -1;
		}
		
		if (pTransData->Init() == -1)
		{
			return -1;
		}
		
		g_MapTransData[key] = pTransData;
	}
	
	return key;
}

int CMt::mt_send(key_t tid, char* ptr, unsigned int* size)
{
	CTransData* p = g_MapTransData[tid];
	if (NULL == p)
	{
		return -1;
	}
	
	return p->write(ptr, size);
}

int CMt::mt_recv(key_t tid, char* ptr, unsigned int* size)
{
	CTransData* p = g_MapTransData[tid];
	if (NULL == p)
	{
		return -1;
	}

	return p->read(ptr, size);
}

void CMt::mt_destory(key_t tid)
{
	map<key_t, CTransData*>::iterator it = g_MapTransData.find(tid);
	if (it != g_MapTransData.end())
	{
		it->second->destory(it->second);
		it->second = NULL;
		
		g_MapTransData.erase(it);
	}
}
#include <platform/visionStore.h>

pthread_mutex_t CVisionStore::m_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CVisionStore::m_ready = PTHREAD_COND_INITIALIZER;

CVisionStore::CVisionStore() : m_cnt(0), m_fetch(0), m_store(0), m_index(0), m_path("")
{
}

CVisionStore::~CVisionStore()
{
	for (unsigned int i = 0; i < QUEUE_SIZE; ++i)
	{
		if (NULL != m_StoreQueue[i])
		{
			delete m_StoreQueue[i];
			m_StoreQueue[i] = NULL;
		}
	}
}

int CVisionStore::Init(const string& path)
{
	if (path.empty())
	{
		return  -1;
	}
	
	char szCmd[256];
	sprintf(szCmd, "rm -r %s", path.c_str());
	
	FILE* fp = popen(szCmd, "r");
	
	if (NULL != fp)
	{
		pclose(fp);
	}
	
	if (0 != mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO))
	{
		return -1;
	}
	
	m_path = path;
	
	return 0;
}

int CVisionStore::push(unsigned char* lptr, unsigned int lLen, unsigned char* rptr, unsigned int rLen, IMU* pImu)
{
	STORE_NODE* node = new STORE_NODE;
	if (NULL == node)
	{
		return -1;
	}
	
	int len = lLen > IMG_SIZE ? IMG_SIZE : lLen;
	node->lLen = len;
	
	memcpy((char*)node->lImg, (char*)lptr, len);
	
	len = rLen > IMG_SIZE ? IMG_SIZE : rLen;
	node->rLen = len;
	
	memcpy((char*)node->rImg, (char*)rptr, len);
	
	if (NULL != pImu)
	{
		node->imu = *pImu;
	}
	
	node->index = m_index++;
	
	pthread_mutex_lock(&m_lock);
	
	if (m_cnt == STORE_QUUE_SIZE)
	{
		delete node;
		node = NULL;
		
		pthread_mutex_unlock(&m_lock);
		pthread_cond_signal(&m_ready);
		return -1;
	}
	
	m_StoreQueue[m_store] = node;
	
	if (++m_store == STORE_QUUE_SIZE)
	{
		m_store = 0;
	}
	
	++m_cnt;
	
	pthread_mutex_unlock(&m_lock);
	pthread_cond_signal(&m_ready);
	
	return 0;
}

int CVisionStore::pop()
{
	STORE_NODE* p = NULL;

	pthread_mutex_lock(&m_lock);
	
	while (NULL == m_StoreQueue[m_fetch] || m_cnt == 0)
	{
		pthread_cond_wait(&m_ready, &m_lock);
	}
	
	p = m_StoreQueue[m_fetch];
	m_StoreQueue[m_fetch] = NULL;
	
	if (++m_fetch == STORE_QUUE_SIZE)
	{
		m_fetch = 0;
	}
	
	--m_cnt;
	
	pthread_mutex_unlock(&m_lock);
	
	OutFile(p);
	
	delete p;
	p = NULL;
	
	return 0;
}

void CVisionStore::OutFile(STORE_NODE* p)
{	
	char szData[256] = {0};
	sprintf(szData, "%s/%06d_l.dat", m_path.c_str(), p->index);
	if (!OutImg((char*)p->lImg, p->lLen, szData))
	{
		cout << "out put left img fail." << endl;
	}
	
	sprintf(szData, "%s/%06d_r.dat", m_path.c_str(), p->index);
	if (!OutImg((char*)p->rImg, p->rLen, szData))
	{
		cout << "out put right img fail." << endl;
	}
}

bool CVisionStore::OutImg(const char* pData, unsigned int size, const char* szPath)
{
	if (NULL == pData || NULL == szPath)
	{
		return false;
	}
	
	FILE* pf = fopen(szPath, "wb");
	if (NULL == pf)
	{
		return false;
	}
	
	// write data
	fwrite(pData, size, 1, pf);

	fclose(pf);
	
	return true;
}


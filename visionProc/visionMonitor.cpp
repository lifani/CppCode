#include "visionMonitor.h"
#include "tools.h"

extern pthread_t VISION_READ_TID;
extern pthread_t VISION_PROC_TID;

static pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t m_exit = PTHREAD_COND_INITIALIZER;

static int iCond = -1;

 bool InitMonitor()
{	
	pthread_mutex_lock(&m_lock);
	
	if (-1 != iCond)
	{
		pthread_mutex_unlock(&m_lock);
		return false;
	}
	
	iCond = MONITOR_WAIT;
	
	return true;
}

 void Monitor()
{
	while (iCond == MONITOR_WAIT)
	{
		pthread_cond_wait(&m_exit, &m_lock);
	}
	
	switch (iCond)
	{
	case PROC_EXIT:
		Writelog(LOG_NOTICE, "Monitor received proc thread exit's signal.", __FILE__, __LINE__);
		pthread_cancel(VISION_READ_TID);
		break;
	case READ_EXIT:
		Writelog(LOG_NOTICE, "Monitor received read thread exit's signal.", __FILE__, __LINE__);
		pthread_cancel(VISION_PROC_TID);
		break;
	default:
		Writelog(LOG_ERR, "Monitor fail.", __FILE__, __LINE__);
		break;
	}
	
	pthread_mutex_unlock(&m_lock);
}

 void NotifyExit(int cond)
{
	pthread_mutex_lock(&m_lock);
	iCond = cond;
	pthread_mutex_unlock(&m_lock);
	pthread_cond_signal(&m_exit);
}

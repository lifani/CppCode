#include "visionMonitor.h"
#include "tools.h"

extern pthread_t VISION_TID_ARR[THREAD_COUNT];

static pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t m_exit = PTHREAD_COND_INITIALIZER;

static int iCond = -1;

static void StopAllThread()
{
	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		if (0 != VISION_TID_ARR[i])
		{
			pthread_cancel(VISION_TID_ARR[i]);
		}
	}
}

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
	
	StopAllThread();

	pthread_mutex_unlock(&m_lock);
}

void NotifyExit(int cond)
{
	pthread_mutex_lock(&m_lock);
	iCond = cond;
	pthread_mutex_unlock(&m_lock);
	pthread_cond_signal(&m_exit);
}

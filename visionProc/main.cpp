#include "typedef.h"
#include "visionProcess.h"
#include "visionMonitor.h"
#include "visionImu.h"
#include "tools.h"

const string msg[THREAD_COUNT + 1] = 
{
"All thread start succeed.",
"Vision process thread start fail.",
"Vision read thread start fail.",
"Vision receive imu package thread start fail."
}; 

const char BIT_MASK = 0x07;

pthread_t VISION_TID_ARR[THREAD_COUNT] = {0};
FUNC FUNC_ARR[THREAD_COUNT] = {process_vision, read_vision, IMUCanRecv};

static int CreatePthread(FUNC func, pthread_t& tid)
{
    int err = pthread_create(&tid, NULL, func, NULL);
    if (err != 0)
	{
		Writelog(LOG_ERR, strerror(err));
	}

    return err;
}

static int StartAllThread()
{
	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		if ((BIT_MASK & (1 << i)) && 0 != CreatePthread(FUNC_ARR[i], VISION_TID_ARR[i]))
		{
			return i + 1;
		}
	}
	
	return 0;
}

static bool Initialize()
{
	if (NULL == InitMMap())
	{
		Writelog(LOG_ERR, "End vision proc.", __FILE__, __LINE__);
		return false;
	}
	
	if (!InitIMUCan())
	{
		Writelog(LOG_ERR, "Initialize imu can fail.", __FILE__, __LINE__);
		return false;
	}
	
	if (!InitMonitor())
	{
		Writelog(LOG_ERR, "Init monitor fail.", __FILE__, __LINE__);
		return false;
	}
	
	return true;
}

int main(int argc, char* argv[])
{
    // 启动日志
	Initlog(argv[0]);

    // 启动进程
	Writelog(LOG_NOTICE, "Start vision proc.", __FILE__, __LINE__);
	
	if (!Initialize())
	{
		exit(0);
	}
	
	int nRet = StartAllThread();
	if (0 != nRet)
	{
		Writelog(LOG_ERR, msg[nRet].c_str(), __FILE__, __LINE__);
		exit(0);
	}
	
	Writelog(LOG_NOTICE, msg[0].c_str(), __FILE__, __LINE__);
	
	// Start monitor
	Monitor();

	DestoryMMap();

	Writelog(LOG_NOTICE, "Vision system exit normal.", __FILE__, __LINE__);
	
	return 0;
}

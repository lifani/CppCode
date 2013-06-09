#include "typedef.h"
#include "visionProcess.h"
#include "visionMonitor.h"
#include "tools.h"

pthread_t VISION_READ_TID = 0;
pthread_t VISION_PROC_TID = 0;

static int CreatePthread(FUNC func, pthread_t& tid)
{
    int err = pthread_create(&tid, NULL, func, NULL);
    if (err != 0)
    {
        Writelog(LOG_ERR, strerror(err));
    }

    return err;
}

int main(int argc, char* argv[])
{
    int err = 0;
    void* tret;

    // 启动日志
    Initlog(argv[0]);

    // 启动进程
    Writelog(LOG_NOTICE, "Start vision proc.", __FILE__, __LINE__);
	
    if (NULL == InitMMap())
    {
		Writelog(LOG_ERR, "End vision proc.", __FILE__, __LINE__);
		exit(0);
    }
	
	if (!InitMonitor())
	{
		Writelog(LOG_ERR, "Init monitor fail.", __FILE__, __LINE__);
		exit(0);
	}

    // 先启动处理算法处理线程
    if (0 != CreatePthread(process_vision, VISION_PROC_TID))
    {
        Writelog(LOG_ERR, "Create process failed.", __FILE__, __LINE__);
        exit(0);
    }

    Writelog(LOG_NOTICE, "Start alg process succeed.", __FILE__, __LINE__);

    // 再启动读取线程
    if (0 != CreatePthread(read_vision, VISION_READ_TID))
    {
        pthread_cancel(VISION_PROC_TID);

        Writelog(LOG_ERR, "Create read process failed.", __FILE__, __LINE__);
        exit(0);
    }
	
	Writelog(LOG_NOTICE, "Start read vision succeed.", __FILE__, __LINE__);
	
	// Start monitor
	Monitor();

    DestoryMMap();

	Writelog(LOG_NOTICE, "Vision system exit normal.", __FILE__, __LINE__);
	
    return 0;
}

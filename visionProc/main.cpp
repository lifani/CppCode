#include "typedef.h"
#include "visionProcess.h"

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

    //daemonize();

    // 先启动处理算法处理线程
    if (0 != CreatePthread(process_vision, VISION_PROC_TID))
    {
        Writelog(LOG_ERR, "Create process failed.", __FILE__, __LINE__);
        exit(0);
    }

    Writelog(LOG_NOTICE, "Start alg process succed.", __FILE__, __LINE__);

    // 再启动读取线程
    if (0 != CreatePthread(read_vision, VISION_READ_TID))
    {
        pthread_cancel(VISION_PROC_TID);

        Writelog(LOG_ERR, "Create read process failed.", __FILE__, __LINE__);
        exit(0);
    }

    Writelog(LOG_NOTICE, "Start read vision succed.", __FILE__, __LINE__);

    err = pthread_join(VISION_PROC_TID, &tret);
    if (err != 0)
    {
        Writelog(LOG_ERR, "can't join vision read process.", __FILE__, __LINE__);

        pthread_cancel(VISION_PROC_TID);
        pthread_cancel(VISION_READ_TID);
    }

    err = pthread_join(VISION_READ_TID, &tret);
    if (err != 0)
    {
        Writelog(LOG_ERR, "can't join vision alg process.", __FILE__, __LINE__);
    }
	
    DestoryMMap();

    return 0;
}

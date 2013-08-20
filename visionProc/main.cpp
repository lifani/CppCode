#include "typedef.h"
#include "visionProcess.h"
#include "visionMonitor.h"
#include "visionImu.h"
#include "visionStore.h"
#include "tools.h"
#include "../VO/odometer.h"
#include "../visionDsp/visionDsp.h"
#include "../libproperty/cutils/properties.h"

#include <signal.h>


extern "C" int __system_properties_init(void);

const string msg[THREAD_COUNT + 1] = 
{
"All thread start succeed.",
"Vision process thread start fail.",
"Vision read thread start fail.",
"Vision receive imu package thread start fail.",
"Vision store thread start fail.",
"Vision motion thread start fail."
}; 

pthread_t VISION_TID_ARR[THREAD_COUNT] = {0};
FUNC FUNC_ARR[THREAD_COUNT] = {
	process_vision, 
	read_vision, 
	IMUCanRecv, 
	store_vision, 
	visionMotion
};

EXIT_FUNC EXIT_FUNC_ARR[THREAD_COUNT] = {
	exit_process_vision, 
	exit_read_vision, 
	exit_imu_receive, 
	exit_vision_store, 
	exit_vision_motion
};

// 退出线程
static void signal_exit()
{
	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		if (0 != VISION_TID_ARR[i])
		{
			(*EXIT_FUNC_ARR[i])();
		}
	}
}

// 信号捕捉函数
static void VisionSignal(int signo)
{	
	if (SIGINT == signo)
	{
		Writelog(LOG_ERR, "Receive SIGINT.", __FILE__, __LINE__);
		signal_exit();
	}
	
	// 暂停2秒
	sleep(2);
	
	// 退出程序
	NotifyExit(ALL_EXIT);
}

// 创建线程
static int CreatePthread(FUNC func, pthread_t& tid)
{
    int err = pthread_create(&tid, NULL, func, NULL);
    if (err != 0)
	{
		Writelog(LOG_ERR, strerror(err));
	}

    return err;
}

// 启动线程
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

static void InitBitMask()
{
#ifndef NO_IMURECV
	BIT_MASK |= 0x04;
#endif

#ifndef NO_STORE
	BIT_MASK |= 0x08;
#endif

#ifndef NO_MOTION
	BIT_MASK |= 0x10;
#endif

	return;
}

static void WaitFpgaReady()
{
	int fd = open(FPGA_READY, O_RDONLY);
	if (fd < 0)
	{
		printf("open fpga_ready error...\n");	
		return;
	}
	
	char data;
	while(1)
	{
		int err = read(fd, &data, 1);
		if ((err > 0) && (data == '1')) 
		{
			printf("FPGA Ready to go...\n");
			break;
		}
	}
	
	close(fd);
}

static void EnableFpga()
{
    property_set("sys.dsp.config", "1");	// set dsp speed
    property_set("sys.fpga.config", "0");	// start fpga sample
}

// 初始化操作
static bool Initialize()
{
	__system_properties_init();
	
	// wait for fpga
	WaitFpgaReady();

	// Init bit mask
	InitBitMask();
	
#ifndef NO_DSP
	if (0 == InitDsp())
	{
		Writelog(LOG_ERR, "Init dsp fail.", __FILE__, __LINE__);
		return false;
	}
#endif
	
	if (NULL == InitMMap())
	{
		Writelog(LOG_ERR, "End vision proc.", __FILE__, __LINE__);
		return false;
	}

#ifndef NO_IMURECV
	if (!InitIMUCan())
	{
		Writelog(LOG_ERR, "Initialize imu can fail.", __FILE__, __LINE__);
		return false;
	}
#endif
	
	if (!InitMonitor())
	{
		Writelog(LOG_ERR, "Init monitor fail.", __FILE__, __LINE__);
		return false;
	}

#ifndef NO_STORE
	if (!InitStore())
	{
		Writelog(LOG_ERR, "Init vision store fail.", __FILE__, __LINE__);
		return false;
	}
#endif
	
	if (signal(SIGINT, VisionSignal) == SIG_ERR)
	{
		Writelog(LOG_ERR, "can't catch SIGINT");
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
	
	// sleep 3
	sleep(3);
	
	// 
	EnableFpga();
	
	// Start monitor
	Monitor();

	DestoryMMap();

#ifndef NO_DSP
	destoryDsp();
#endif

	Writelog(LOG_NOTICE, "Vision system exit normal.", __FILE__, __LINE__);
	
	return 0;
}

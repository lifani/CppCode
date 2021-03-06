/*************************************
VERSION :	2.0
CREATOR :	lifani
DATE	:	2014.1.2
*************************************/
#ifndef __ABSTRACT_CAN_CTRL_H__
#define __ABSTRACT_CAN_CTRL_H__

#include <can_config.h> 
#include <libsocketcan.h>

#include <getopt.h>
#include <libgen.h>

#include <limits.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <typedef.h>
#include <datatype.h>

#define LOG_TAG "CAN_CTRL"

#pragma pack(1)

typedef struct _NormalMode
{
	float imu_offset_x;
	float imu_offset_y;
	float imu_offset_z;
	
	float gps_offset_x;
	float gps_offset_y;
	float gps_offset_z;
	
	short res0;
	char  key;
	char  res1;
	
	float compass_bias_x;
	float compass_bias_y;
	float compass_bias_z;
	
	float compass_scale_x;
	float compass_scale_y;
	float compass_scale_z;
} NormalMode;

typedef struct tManageCtrlDataA2
{
	char			head[4];
	unsigned int  	g_real_clock;
	short 			g_real_input_channel_COMMAND_AILERON;
	short 			g_real_input_channel_COMMAND_ELEVATOR;
	short 			g_real_input_channel_COMMAND_THROTTLE;
	short 			g_real_input_channel_COMMAND_RUDDER;
	short 			g_real_input_channel_COMMAND_MODE;
	short 			g_real_input_channel_COMMAND_IOC;
	short 			g_real_input_channel_COMMAND_GO_HOME;
	short 			g_real_input_channel_COMMAND_D4;
	short			g_real_input_control_core_pitch;
	short			g_real_input_control_core_roll;
	short			g_real_input_control_core_alti;
	short			g_real_input_control_core_tail;
	short 			g_real_output_control_core_pitch;
	short 			g_real_output_control_core_roll;
	short 			g_real_output_control_core_alti;
	short 			g_real_output_control_core_tail;
	short 			g_real_output_control_w_pitch;
	short 			g_real_output_control_w_roll;
	short 			g_real_output_control_w_alti;
	short 			g_real_output_control_w_tail;
	int 			g_real_output_channel_0;
	int 			g_real_output_channel_1;
	int 			g_real_output_channel_2;
	int 			g_real_output_channel_3;
	int 			g_real_output_channel_4;
	int 			g_real_output_channel_5;
	int 			g_real_output_channel_6;
	int 			g_real_output_channel_7;
	int 			g_raw_output_channel_0;
	int 			g_raw_output_channel_1;
	int 			g_raw_output_channel_2;
	int 			g_raw_output_channel_3;
	int 			g_raw_output_channel_4;
	int 			g_raw_output_channel_5;
	int 			g_raw_output_channel_6;
	int 			g_raw_output_channel_7;
	unsigned char 	g_real_status_cotrol_command_mode;
	unsigned char 	g_real_status_control_real_mode;
	unsigned char	g_real_status_ioc_control_command_mode;
	unsigned char 	g_ground_station_status_status_case;
	int				dyn_tail_ctrl_value;
	unsigned char	g_real_status_rc_state;
	float			g_debug_inr0;
	float			g_debug_inr1;
	float			g_debug_inr2;
	float			g_debug_inr3;
	float			g_debug_inr4;
	float			g_debug_inr5;
	float			g_debug_inr6;
	float			g_debug_inr7;
	float			g_debug_inr8;
	float			g_debug_inr9;
	unsigned char	OSCPUUsage;
	unsigned char   takeoff_assistant_working;
	unsigned char   g_real_status_motor_status;
	unsigned int  	imu_package_lost_count;
	unsigned char	ctrl_data_hold_horizontal;
	short  			g_real_status_main_batery_voltage;
} ManageCtrlDataA2;

typedef struct tManageCtrlDataC2
{
	char		   head[4];
	unsigned short pulseout_1;
	unsigned short pulseout_2;
	unsigned short pulseout_3;
	unsigned short pulseout_4;
	unsigned short pulseout_5;
	unsigned short pulseout_6;
	unsigned short pulseout_7;
	unsigned short pulseout_8;
	//unsigned short null_1;
	//unsigned short null_2;
	short		   in_1;
	short		   in_2;
	short		   in_3;
	short		   in_4;
	short		   in_5;
	short		   in_6;
	short		   in_7;
	short		   in_8;
	short		   in_9;
	short		   in_10;
	short		   out1000_1;
	short		   out1000_2;
	short		   out1000_3;
	short		   out1000_4;
	short		   out1000_5;
	short		   out1000_6;
	short		   out1000_7;
	short		   out1000_8;
	short		   out1000_9;
	short		   out1000_10;
	short		   out_1;
	short		   out_2;
	short		   out_3;
	short		   out_4;
	short		   out_5;
	short		   out_6;
	short		   out_7;
	short		   out_8;
	short		   out_9;
	short		   out_10;
} ManageCtrlDataC2;

typedef struct cmd_imu_body
{
	double 	longti;
	double 	lati;
	float	alti;
	
	float acc_x;
	float acc_y;
	float acc_z;
	
	float gyro_x;
	float gyro_y;
	float gyro_z;
	
	float	press;
	
	float	q0;
	float	q1;
	float	q2;
	float 	q3;
	
	float	agx;
	float	agy;
	float	agz;
	
	float	vgx;
	float	vgy;
	float	vgz;
	
	float	gbx;
	float	gby;
	float	gbz;
	
	short	mx;
	short	my;
	short	mz;
	short	temp[3];
	
	unsigned short sensor_overflow;
	unsigned short filter_status;
	unsigned short gps_svn;
	unsigned short cnt;
} imu_body;

#pragma pack()

typedef union _CAN_HEAD_U
{
	char szHead[4];
	unsigned int uHead;
} CAN_HEAD;

typedef union _CAN_TAIL_U
{
	char szTail[4];
	unsigned int uTail;
} CAN_TAIL;

class CAbstractCanCtrl
{
public :
	
	CAbstractCanCtrl();
	
	virtual ~CAbstractCanCtrl();
	
	virtual int Initialize(unsigned short canId, unsigned short cmd);
	
	virtual int Process(struct can_frame* pFrame);
	
	virtual int GetContent(char*& ptr);
	
	virtual int SetContent(char* ptr, int len, int cmd = 0);
	
	virtual bool CheckHead(char* ptr, int len);
	
	virtual bool CheckTotal(char* ptr, int len);
	
	virtual void SetKey(int key = 0);
	
protected :
	
	bool m_begin;

	unsigned short m_canId;
	unsigned short m_cmdCode;
	unsigned int m_size;
	
	unsigned int m_pos;
	
	CAN_HEAD m_uCanHead;
	CAN_TAIL m_uCanTail;
	
	char m_buf[256]; 

	int m_key;
};

#endif

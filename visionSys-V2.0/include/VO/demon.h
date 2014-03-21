#ifndef __DEMON_H__
#define __DEMON_H__

#pragma pack(1)

typedef struct stereo_information
{
	short LeftPointsNum;
	short RightPointsNum;
	short StereoMatchNum;
	short FrameMatchNum;
	short InlierNum;
	
	float tbx;
	float tby;
	float tbz;
	float weight;
	float disparity;
} Stereo_Info;

typedef struct tVoInfo
{
	short ctrl_pitch;
	short ctrl_roll;
	short ctrl_alti;
	
	bool HoverFlag;
	bool KeyFrameFlag;
	bool isRotOK;
	bool isVelocityOK;
	bool isPositionOK;
	bool KeyFrameUpdateFlag;
	int  Keyframe_Update_Cnt;
	int  Keyframe_Failure_Cnt;
	int  Quit_Sig_Cnt;
} VO_Info;

#pragma pack()

bool Register(int CameraNum);

bool RunVelocity(char* pData, int num, const char* imu, char* ptr, 
	Stereo_Info* Branch, VO_Info* Fusion);

#endif
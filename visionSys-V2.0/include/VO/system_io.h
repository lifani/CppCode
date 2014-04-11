#ifndef _SYSTEM_IO_
#define _SYSTEM_IO_

#include <string>
using namespace std;

#ifndef _DJI_VO_OUTPUT_
#define _DJI_VO_OUTPUT_

typedef struct Branch_information
{
	short n_left_kpoint;
	short n_right_kpoint;
	short n_stereo_match;
	short n_frame_match;
	short n_inlier_match;

	float tbx;
	float tby;
	float tbz;

	float vo_weight;
	float v_weight;
	float p_weight;
	float disparity;
} branch_info;

typedef struct tVoInfo
{
	short ctrl_pitch;
	short ctrl_roll;
	short ctrl_alti;

	bool is_svo;

	bool hover_command;
	bool hover_prepared;
	bool rotaion_ok;
	bool velocity_ok;
	bool position_ok;
	bool keyframe_update_flag;

	short  keyframe_unchanged_cnt;
	short  position_failure_cnt;
	short  hover_quit_signal_cnt;
} vo_info;

typedef struct _CAN_VELOCITY_DATA
{
	float dx;			//m
	float dy;			//m
	float dz;			//m

	short vx;			//mm
	short vy;			//mm
	short vz;			//mm

	unsigned char uc1;
	unsigned char uc2;

	unsigned short cnt;
} vo_can_output;

#endif



class iosd_writter
{
public:
	iosd_writter() { };
	~iosd_writter() { };

	void open (string file_name);
	void save (vo_can_output* vo_output, vo_info* vo_state, branch_info* branches, int n_branch);
	void release (void);
private:
	FILE *fout;
};



// interface functions
bool init_vo(string file_path, int camera_num);		// odometer initialization
bool run_vo(char* points, const char* imu, vo_info* vo_state, branch_info* branches, vo_can_output* vo_output);		// on-line 
void run_vo(string file_path);			// off-line

#endif
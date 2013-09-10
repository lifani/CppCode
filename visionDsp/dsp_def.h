#ifndef _DSP_DEF_H_
#define _DSP_DEF_H_


#define DESCRIPTOR_BYTE		32
#define MAX_FEATURE_SIZE	1000
#define TIME_FILTER_SIZE	1

typedef struct _kpoint
{
	short u, v;							//image pixel  (u,v) -> (col,row)
	unsigned char desc[DESCRIPTOR_BYTE];		//feature descriptor
}KPoint;

typedef struct tSearch_range
{
	int up_dist;
	int down_dist;
	int left_dist;
	int right_dist;
} Search_range;

typedef struct frame_dsp
{
	KPoint kpoint_left[MAX_FEATURE_SIZE];
	KPoint kpoint_right[MAX_FEATURE_SIZE];
	
	Search_range range;
	
	short n_kpoint_left;
	short n_kpoint_right;
	
	short best_choice[MAX_FEATURE_SIZE];
	
}Frame_dsp;

#endif
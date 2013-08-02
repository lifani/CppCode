/*
 * Copyright (c) 2011-2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== test_omx.c ========
 *
 *  Example of setting up an "OMX" service with the ServiceMgr, allowing clients
 *  to instantiate OMX instances.
 *
 *  Works with the test_omx.c Linux user space application over the rpmsg_omx\
 *  driver.
 */

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Diags.h>

#include <ti/ipc/MultiProc.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Cache.h>

#include <ti/grcm/RcmTypes.h>
#include <ti/grcm/RcmServer.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <c6x.h>

#include <ti/srvmgr/ServiceMgr.h>
#include <ti/srvmgr/rpmsg_omx.h>
#include <ti/srvmgr/omx_packet.h>

/* Turn on/off printf's */
#define CHATTER 0

#define DESCRIPTOR_BYTE		32
#define MAX_FEATURE_SIZE	1000
#define TIME_FILTER_SIZE	1

#define IMG_WIDTH 328
#define IMG_HEIGHT 246

//unsigned char bit_num[1 << 8];
int bit_num[1 << 8];

const int brief_match_threshold = 64;
const int second_match_threshold = 25;

/* Legacy function to allow Linux side rpmsg sample tests to work: */
extern void start_ping_tasks();
extern void start_resmgr_task();
extern void start_hwSpinlock_task();

/*
 * OMX packet expected to have its data payload start with a payload of
 * this value. Need to export this properly in a meaningful header file on
 * both HLOS and RTOS sides
 */
typedef enum {
    RPC_OMX_MAP_INFO_NONE       = 0,
    RPC_OMX_MAP_INFO_ONE_BUF    = 1,
    RPC_OMX_MAP_INFO_TWO_BUF    = 2,
    RPC_OMX_MAP_INFO_THREE_BUF  = 3,
    RPC_OMX_MAP_INFO_MAX        = 0x7FFFFFFF
} map_info_type;

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

typedef struct {
    int addr;
    int size;
} buffer_one;

static void inline BRIEF_match (KPoint *kp1, int nkp1, KPoint *kp2, int nkp2, Search_range range, short *best_choice, int width, int height);
static Int32 fxnDouble(UInt32 size, UInt32 *data);
/* ==========================================================================
 * OMX Fxns, adapted from rpc_omx_skel.c.
 *
 * These defines are to illustrate reuse of RPC_SKEL fxns with ServiceMgr.
 *===========================================================================*/

#define H264_DECODER_NAME   "H264_decoder"

#define OMX_VIDEO_THREAD_PRIORITY    5

typedef Int32  RPC_OMX_ERRORTYPE;
typedef UInt32 OMX_HANDLETYPE;

static RPC_OMX_ERRORTYPE RPC_SKEL_GetHandle(Void *, UInt32 size, UInt32 *data);
static RPC_OMX_ERRORTYPE RPC_SKEL_SetParameter(UInt32 size, UInt32 *data);
static RPC_OMX_ERRORTYPE RPC_SKEL_GetParameter(UInt32 size, UInt32 *data);
static RPC_OMX_ERRORTYPE RPC_SKEL_ShareRegion(UInt32 size, UInt32 *data);

/* RcmServer static function table */
static RcmServer_FxnDesc OMXServerFxnAry[] = {
//    {"RPC_SKEL_GetHandle"   , RPC_SKEL_GetHandle},  // Set at runtime.
    {"RPC_SKEL_GetHandle"   , NULL},
    {"RPC_SKEL_SetParameter", RPC_SKEL_SetParameter},
    {"RPC_SKEL_GetParameter", RPC_SKEL_GetParameter},
	{"fxnDouble", fxnDouble },
    {"RPC_SKEL_ShareRegion",  RPC_SKEL_ShareRegion },
};

#define OMXServerFxnAryLen (sizeof OMXServerFxnAry / sizeof OMXServerFxnAry[0])

static const RcmServer_FxnDescAry OMXServer_fxnTab = {
    OMXServerFxnAryLen,
    OMXServerFxnAry
};


static RPC_OMX_ERRORTYPE RPC_SKEL_SetParameter(UInt32 size, UInt32 *data)
{
#if CHATTER
    System_printf("RPC_SKEL_SetParameter: Called\n");
#endif

    return(0);
}

static RPC_OMX_ERRORTYPE RPC_SKEL_GetParameter(UInt32 size, UInt32 *data)
{
#if CHATTER
    System_printf("RPC_SKEL_GetParameter: Called\n");
#endif

    return(0);
}

#define CALLBACK_DATA      "OMX_Callback"
#define PAYLOAD_SIZE       sizeof(CALLBACK_DATA)
#define CALLBACK_DATA_SIZE (HDRSIZE + OMXPACKETSIZE + PAYLOAD_SIZE)

/*
 *  ======== fxnDouble ========
 */
Int32 fxnDouble(UInt32 size, UInt32 *data)
{
	return 0;
}

static RPC_OMX_ERRORTYPE RPC_SKEL_GetHandle(Void *srvc, UInt32 size,
                                           UInt32 *data)
{
    char              cComponentName[128] = {0};
    OMX_HANDLETYPE    hComp;
    Char              cb_data[HDRSIZE + OMXPACKETSIZE + PAYLOAD_SIZE] =  {0};

    /*
     * Note: Currently, rpmsg_omx linux driver expects an omx_msg_hdr in front
     * of the omx_packet data, so we allow space for this:
     */
    struct omx_msg_hdr * hdr = (struct omx_msg_hdr *)cb_data;
    struct omx_packet  * packet = (struct omx_packet *)hdr->data;


    //Marshalled:[>offset(cParameterName)|>pAppData|>offset(RcmServerName)|>pid|
    //>--cComponentName--|>--CallingCorercmServerName--|
    //<hComp]

    strcpy(cComponentName, (char *)data + sizeof(map_info_type));

#if CHATTER
    System_printf("RPC_SKEL_GetHandle: Component Name received: %s\n",
                  cComponentName);
#endif

    /* Simulate sending an async OMX callback message, passing an omx_packet
     * structure.
     */
    packet->msg_id  = 99;   // Set to indicate callback instance, buffer id, etc.
    packet->fxn_idx = 5;    // Set to indicate callback fxn
    packet->data_size = PAYLOAD_SIZE;
    strcpy((char *)packet->data, CALLBACK_DATA);

#if CHATTER
    System_printf("RPC_SKEL_GetHandle: Sending callback message id: %d, "
                  "fxn_id: %d, data: %s\n",
                  packet->msg_id, packet->fxn_idx, packet->data);
#endif
    ServiceMgr_send(srvc, cb_data, CALLBACK_DATA_SIZE);

    /* Call OMX_Get_Handle() and return handle for future calls. */
    //eCompReturn = OMX_GetHandle(&hComp, (OMX_STRING)&cComponentName[0], pAppData,&rpcCallBackInfo);
    hComp = 0x5C0FFEE5;
    data[0] = hComp;

#if CHATTER
    System_printf("RPC_SKEL_GetHandle: returning hComp: 0x%x\n", hComp);
#endif

    return(0);
}

void inline BRIEF_match (KPoint *kp1, int nkp1, KPoint *kp2, int nkp2, Search_range range, short *best_choice, int width, int height)
{
	memset (best_choice, 0xffff, MAX_FEATURE_SIZE);

	// const限定
	const int up_dist = range.up_dist;
	const int down_dist = range.down_dist;
	const int left_dist = range.left_dist;
	const int right_dist = range.right_dist;

	int start_nkp2 = 0, end_nkp2 = 0;
	
	int min_cnt, second_min_cnt, tmp_cnt, min_pos = -1;
	
	int tmp[16];
	
	int cur_kp1_row, cur_kp1_col;
	int min_kp2_row = 0, max_kp2_row = height - 1;
	int cur_kp2_col = 0;
	
	int i, j, k, b, b1, b2;
	
	unsigned char *v1, *v2;
	unsigned char v[32];

	for ( i = 0; i < nkp1; i ++)
	{
		cur_kp1_row = kp1[i].v;
		cur_kp1_col = kp1[i].u;

		v1 = kp1[i].desc;
		min_cnt = second_min_cnt = 256;

		if (i == 0 || cur_kp1_row > kp1[i - 1].v)
		{
			min_kp2_row = 0, max_kp2_row = height - 1;
			if (cur_kp1_row - up_dist > 0) min_kp2_row = cur_kp1_row - up_dist;
			if (cur_kp1_row + down_dist < height) max_kp2_row = cur_kp1_row + down_dist;

			while (start_nkp2 < nkp2 && kp2[start_nkp2].v < min_kp2_row) start_nkp2 ++;
			while (end_nkp2 < nkp2 && kp2[end_nkp2].v <= max_kp2_row) end_nkp2 ++;
		}

		for (j = start_nkp2; j < end_nkp2; ++j)
		{
			cur_kp2_col = kp2[j].u;
			if (cur_kp2_col < cur_kp1_col - left_dist || cur_kp2_col > cur_kp1_col + right_dist)
				continue;

			v2 = kp2[j].desc;
			tmp_cnt = 0;

			// 循环展开
			//for ( k = 0; k < 32; k ++)
			//	tmp_cnt += bit_num[v1[k]^v2[k]];
			
			const int* pbit = &bit_num[0];
			
			const unsigned char* pv1 = &v[0];
			const unsigned char* pv2 = &v[16];
			
			int* pv = (int*)&v[0];
			const int* piV1 = (int*)&v1[0];
			const int* piV2 = (int*)&v2[0];
			
			for (k = 0; k < 8; k++)
			{
				*(pv + k) = *(piV1 + k) ^ *(piV2 + k);
			}
			
			for (k = 0; k < 16; ++k)
			{
				tmp[k] = pbit[*(pv1 + k)] + pbit[*(pv2 + k)];
			}
			
			for (k = 0; k < 16; ++k)
			{
				tmp_cnt += tmp[k];
			}
			
			b1 = tmp_cnt < min_cnt;
			if (b1)
			{
				second_min_cnt = min_cnt;
				min_cnt = tmp_cnt;
				min_pos = j;
				
				tmp_cnt = 256;
			}
			
			b2 = tmp_cnt < second_min_cnt;
			if (b2)
			{
				second_min_cnt = tmp_cnt;
			}
		}

		if ((min_cnt < brief_match_threshold && second_min_cnt - min_cnt > second_match_threshold) || min_cnt * 4 < brief_match_threshold)
		{
			best_choice[i] = min_pos;
		}
	}
}

static RPC_OMX_ERRORTYPE RPC_SKEL_ShareRegion(UInt32 size, UInt32 *data)
{
    buffer_one  *buffer_args;
	
	Frame_dsp *pFrame = NULL;

    buffer_args = (buffer_one *)((UInt32)data + sizeof(map_info_type) + sizeof(int));

	pFrame = (Frame_dsp*)buffer_args->addr;
	
	Cache_inv(pFrame, buffer_args->size, Cache_Type_ALL, TRUE);

	BRIEF_match(pFrame->kpoint_left, pFrame->n_kpoint_left, pFrame->kpoint_right, pFrame->n_kpoint_right, 
		pFrame->range, pFrame->best_choice, IMG_WIDTH, IMG_HEIGHT);

	Cache_wbInv(pFrame, buffer_args->size, Cache_Type_ALL, TRUE);

    return(0);
}

Int main(Int argc, char* argv[])
{
    RcmServer_Params  rcmServerParams;

    System_printf("%s starting..\n", MultiProc_getName(MultiProc_self()));

    /*
     * Enable use of runtime Diags_setMask per module:
     *
     * Codes: E = ENTRY, X = EXIT, L = LIFECYCLE, F = INFO, S = STATUS
     */
    Diags_setMask("ti.ipc.rpmsg.MessageQCopy=EXLFS");

    /* Setup the table of services, so clients can create and connect to
     * new service instances:
     */
    ServiceMgr_init();

    /* initialize RcmServer create params */
    RcmServer_Params_init(&rcmServerParams);

    /* The first function, at index 0, is a special create function, which
     * gets passed a Service_Handle argument.
     * We set this at run time as our C compiler is not allowing named union
     * field initialization:
     */
    OMXServer_fxnTab.elem[0].addr.createFxn = RPC_SKEL_GetHandle;

    rcmServerParams.priority    = Thread_Priority_ABOVE_NORMAL;
    rcmServerParams.fxns.length = OMXServer_fxnTab.length;
    rcmServerParams.fxns.elem   = OMXServer_fxnTab.elem;

	int i = 0;
	for (i = 0; i < 255; i ++)
		bit_num[i] = ((i&1) != 0) + ((i&2) != 0) + ((i&4) != 0) + ((i&8) != 0) + ((i&16) != 0) + ((i&32) != 0) + ((i&64) != 0) + ((i&128) != 0);
	
    /* Register an OMX service to create and call new OMX components: */
    ServiceMgr_register("OMX", &rcmServerParams);

#if 0
    /* Some background ping testing tasks, used by rpmsg samples: */
    start_ping_tasks();
#endif

#if 0 /* DSP or CORE0 or IPU */
    /* Run a background task to test rpmsg_resmgr service */
    start_resmgr_task();
#endif

#if 0  /* DSP or CORE0 or IPU */
    /* Run a background task to test hwspinlock */
    start_hwSpinlock_task();
#endif

    /* Start the ServiceMgr services */
    ServiceMgr_start(0);

    BIOS_start();


    return (0);
}

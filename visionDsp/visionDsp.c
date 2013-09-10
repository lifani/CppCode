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
 * omx_benchmark.c
 *
 * Benchmark average round trip time for equivalent of SysLink 2 RcmClient_exec.
 *
 * This calls the fxnDouble RcmServer function, similar to the SysLink 2 ducati
 * rcm/singletest.
 *
 * Requires:
 * --------
 * test_omx.c sample, with fxnDouble registered in RcmServer table.
 *
 * To Configure Ducati code for Benchmarking:
 * ------------------------------------------
 * In package.bld:
 *   - Set profile=release in package.bld
 * In benchmark test code:
 *   - Disable printf's, trace.
 * In benchmark .cfg file:
 *   - Defaults.common$.diags_ASSERT = Diags.ALWAYS_OFF
 *   - Defaults.common$.logger = null;
 *
 * Expected Result:
 * ---------------
 * avg time: 110 usecs
 */

#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/eventfd.h>
#include <sys/mman.h>

#include "include/rpmsg_omx.h"
#include "include/omx_packet.h"
#include "include/omx_rpc_utils.h"
#include <ion.h>

#include "visionDsp.h"

#define rpmsg_dev "/dev/rpmsg-omx2"
#define MEM_SIZE  (1024 * 1024)

#define DEVICE_SYS_DSP "/sys/devices/platform/omap2-fpga/dsp"

static int fd = 0;
static int mem_fd = 0;
static int map_fd = 0;

unsigned char *share_ptr = NULL;

static struct ion_handle *handle;
static struct omx_conn_req connreq = { .name = "OMX" };

static int packet_len = 0;
static int reply_len = 0;
static char packet_buf[512] = {0};
static char return_buf[512] = {0};
static omx_packet *packet = (omx_packet *)packet_buf;
static omx_packet *rtn_packet = (omx_packet *)return_buf;

static int exec_cmd(int fd, char *msg, int len, char *reply_msg, int *reply_len)
{
    int ret = write(fd, msg, len);
    if (ret < 0) {
		perror("Can't write to OMX instance");
		return -1;
    }

    // Now, await normal function result from OMX service:
    // Note: len should be max length of response expected.
    ret = read(fd, reply_msg, len);
    if (ret < 0) 
	{
		perror("Can't read from OMX instance");
		return -1;
    }
    else 
	{
		*reply_len = ret;
    }
	
    return(0);
}

static void init_omx_packet(omx_packet *packet, uint16_t desc, uint16_t nFxnIdx)
{
    // initialize the packet structure 
    packet->desc  |= desc << OMX_DESC_TYPE_SHIFT;
    packet->msg_id  = 0;
    packet->flags  = OMX_POOLID_JOBID_NONE;
    packet->fxn_idx = (nFxnIdx | 0x80000000);
    packet->result = 0;
}

static int InitMMap()
{
	int align = 0;
	int alloc_flags = ION_HEAP_CARVEOUT_MASK;
    
   
	// Connect to the OMX ServiceMgr
    fd = open(rpmsg_dev, O_RDWR);
    if (fd < 0) {
        perror("Can't open OMX device");
        return 0;
    }

    // Create an OMX server instance, and rebind its address to this
    // file descriptor.
    int ret = ioctl(fd, OMX_IOCCONNECT, &connreq);
    if (ret < 0) {
        perror("Can't connect to OMX instance");
        return 0;
    }

    // Alloc shared memory for ARM & DSP
    mem_fd = ion_open();
    if ( mem_fd < 0 )
    {
        perror("Can't open ion heap device");
        return 0;	
    }
	
    ret = ion_alloc(mem_fd, MEM_SIZE, align, alloc_flags, &handle);
    if (ret < 0) {
        perror("ion_alloc mem failed");
        return 0;
    }
	
    ret = ion_map(mem_fd, handle, MEM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, 0, &share_ptr, &map_fd);
    if (ret < 0) {
        perror("ion_alloc mem failed");
        return 0;
    }
	
	return 1;
}

static void InitPackage()
{
	int nPos = 0;
	int	noffset = 0;
    int size = sizeof(Frame_dsp);
    
	// Set Packet Header for the RCMServer, synchronous execution: 
	init_omx_packet(packet, OMX_DESC_MSG, RPC_OMX_FXN_IDX_SHARE_REGION);

	// Set data for the OMX function: 
	RPC_SETFIELDVALUE(packet->data, nPos, RPC_OMX_MAP_INFO_ONE_BUF, RPC_OMX_MAP_INFO_TYPE);
	noffset = sizeof(RPC_OMX_MAP_INFO_TYPE) + sizeof(noffset);
	RPC_SETFIELDVALUE(packet->data, nPos, noffset, OMX_U32);

	// Buffer 1 addr
	RPC_SETFIELDVALUE(packet->data, nPos, (int)share_ptr, OMX_U32); 
	// Buffer 1 size
	RPC_SETFIELDVALUE(packet->data, nPos, size, OMX_U32); 		  

	packet->data_size = nPos;
	packet_len = sizeof(omx_packet) + packet->data_size;
}

int InitDsp()
{
	// 设置DSP工作频率
	/*int fd = open(DEVICE_SYS_DSP, O_RDWR);
	if (fd < 0)
	{
		printf("open /sys/devices/platform/omap2-fpga/dsp fail.\n");
		return 0;
	}
	
	if (-1 == write(fd, "1", 1))
	{
		printf("set dsp fail.\n");
		close(fd);
		
		return 0;
	}
	
	close(fd);*/
	
	FILE* pf = popen("echo 1>/sys/devices/platform/omap2-fpga/dsp", "r");
	if (NULL == pf)
	{
		return 0;
	}
	
	pclose(pf);

	if(!InitMMap())
	{
		return 0;
	}
	
	InitPackage();
	
	return 1;
}

static void vision_exec_call(int fd)
{
    uint16_t server_status = 0;
	
	// Exec command:
	exec_cmd(fd, (char *)packet, packet_len, (char *)rtn_packet, &reply_len);

	// Decode reply:
	server_status = (OMX_DESC_TYPE_MASK & rtn_packet->desc) >> OMX_DESC_TYPE_SHIFT;
	if (server_status != OMXSERVER_STATUS_SUCCESS)  
	{
	   printf("omx_benchmark Failed: server status: %d\n", server_status);
	}
}

void destoryDsp()
{
	// Free memory 
    int ret = ion_free(mem_fd, handle);
    ion_close(mem_fd);
	
    munmap(share_ptr, MEM_SIZE);
    close(map_fd);

    /* Terminate connection and destroy OMX instance */
    ret = close(fd);
    if (ret < 0) {
        perror("Can't close OMX fd ??");
		return;
    }

    printf("omx_sample: Closed connection to %s!\n", connreq.name);
}

void vision_dsp_match(KPoint* pleft, short nl, KPoint* pright, short nr, Search_range range, short* best_choice)
{
	if (NULL == pleft || NULL == pright)
	{
		printf("left or right img is null.\n");
		return;
	}

	Frame_dsp* p = (Frame_dsp*)share_ptr;
	if (NULL == p)
	{
		printf("share_ptr is null.\n");
		return;
	}
	
	memcpy((char*)p->kpoint_left, (char*)pleft, nl * sizeof(KPoint));
	memcpy((char*)p->kpoint_right, (char*)pright, nr * sizeof(KPoint));
	
	p->n_kpoint_left = nl;
	p->n_kpoint_right = nr;
	p->range = range;

    vision_exec_call(fd);

	memcpy((char*)best_choice, (char*)p->best_choice, MAX_FEATURE_SIZE * sizeof(short));

}

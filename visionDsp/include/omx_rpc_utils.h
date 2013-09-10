/*
 * Copyright (c) 2010, Texas Instruments Incorporated
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

/**
 *  @file  omx_rpc_utils.h
 *         This file contains methods that provides the functionality for
 *         the OpenMAX1.1 DOMX Framework RPC.
 *
 *  @path \WTSD_DucatiMMSW\framework\domx\omx_rpc\inc
 *
 *  @rev 1.0
 */

/*==============================================================
 *! Revision History
 *! ============================
 *! 29-Mar-2010 Abhishek Ranka : Revamped DOMX implementation
 *!
 *! 19-August-2009 B Ravi Kiran ravi.kiran@ti.com: Initial Version
 *================================================================*/

#ifndef OMX_RPC_UTILSH
#define OMX_RPC_UTILSH

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include "omx_rpc_internal.h"



/******************************************************************
 *   MACROS
 ******************************************************************/
#define RPC_UTIL_GETSTRUCTSIZE(PTR) *((OMX_U32*)PTR)

/******************************************************************
 *   MACROS - COMMON MARSHALLING UTILITIES
 ******************************************************************/
#define RPC_SETFIELDVALUE(MSGBODY, POS, VALUE, TYPE) do { \
    *((TYPE *) ((OMX_U32)MSGBODY+POS)) = (TYPE)VALUE; \
    POS += sizeof(TYPE); \
    } while(0)

#define RPC_SETFIELDOFFSET(MSGBODY, POS, OFFSET, TYPE) do { \
    *((TYPE *) ((OMX_U32)MSGBODY+POS)) = OFFSET; \
    POS += sizeof(TYPE); \
    } while(0)

#define RPC_SETFIELDCOPYGEN(MSGBODY, POS, PTR, SIZE) do { \
    TIMM_OSAL_Memcpy((OMX_U8*)((OMX_U32)MSGBODY+POS), PTR, SIZE); \
    POS += SIZE; \
    } while (0)

#define RPC_SETFIELDCOPYTYPE(MSGBODY, POS, PSTRUCT, TYPE) do { \
    *((TYPE *)((OMX_U32)MSGBODY+POS)) = *PSTRUCT; \
    POS += sizeof(TYPE); \
    } while (0)

/******************************************************************
 *   MACROS - COMMON UNMARSHALLING UTILITIES
 ******************************************************************/
#define RPC_GETFIELDVALUE(MSGBODY, POS, VALUE, TYPE) do { \
    VALUE = *((TYPE *) ((OMX_U32)MSGBODY+POS)); \
    POS += sizeof(TYPE); \
    } while(0)

#define RPC_GETFIELDOFFSET(MSGBODY, POS, OFFSET, TYPE) do { \
    OFFSET = *((TYPE *) ((OMX_U32)MSGBODY+POS)); \
    POS += sizeof(TYPE); \
    } while(0)

#define RPC_GETFIELDCOPYGEN(MSGBODY, POS, PTR, SIZE)  do { \
    TIMM_OSAL_Memcpy(PTR, (OMX_U8*)((OMX_U32)MSGBODY+POS), SIZE); \
    POS += SIZE; \
    } while(0)

#define RPC_GETFIELDCOPYTYPE(MSGBODY, POS, PSTRUCT, TYPE) do { \
    *PSTRUCT = *((TYPE *)((OMX_U32)MSGBODY+POS)); \
    POS += sizeof(TYPE); \
    } while(0)

#define RPC_GETFIELDPATCHED(MSGBODY, OFFSET, PTR, TYPE) \
PTR = (TYPE *) (MSGBODY+OFFSET);


#ifdef __cplusplus
}
#endif
#endif

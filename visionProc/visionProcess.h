#ifndef VISIONPROCESS_H_INCLUDED
#define VISIONPROCESS_H_INCLUDED

#include "typedef.h"
#include "visionNode.h"

// 初始化存储映射
void* InitMMap();

// unmap存储映射
void DestoryMMap();

// 视觉处理线程入口函数
void* process_vision(void* arg);

// 图像数据入处理队列函数
static void enter_vision_queue(VisionNode* pNode);

// 读取图像数据线程入口函数
void* read_vision(void* arg);

static void process_poll(struct pollfd* p);

static bool isReady();

static bool ReadImg(VisionNode*& pNode);

static void writeFlg(int fd);

static void pthread_exit();

static bool writeMapHex();

void exit_process_vision();

void exit_read_vision();

#endif // VISIONPROCESS_H_INCLUDED

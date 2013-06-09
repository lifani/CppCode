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
 void enter_vision_queue(VisionNode* pNode);

// 读取图像数据线程入口函数
 void* read_vision(void* arg);

 void process_poll(struct pollfd* p);

 bool isReady(int fd);

 bool ReadImg(VisionNode*& pNode);

 void writeFlg(int fd);

#endif // VISIONPROCESS_H_INCLUDED

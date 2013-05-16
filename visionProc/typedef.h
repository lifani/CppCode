#ifndef TYPEDEF_H_INCLUDED
#define TYPEDEF_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <pthread.h>
#include <poll.h>

#include <signal.h>
#include <syslog.h>
#include <sys/resource.h>
#include <sys/mman.h>

#include <iostream>

using namespace std;

typedef void* (*FUNC)(void* arg);

#define DEVICE_FILENAME "/dev/fpga"
#define DEVICE_SYS_POLL "/dev/poll"

#define MMAP_SIZE 0x00100000
#define IMG_SIZE (320 * 240)

#endif // TYPEDEF_H_INCLUDED

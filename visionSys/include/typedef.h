#ifndef __TYPE_DEF__
#define __TYPE_DEF__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>

#include <pthread.h>
#include <poll.h>

#include <vector>
#include <map>
#include <iostream>

#include <log/log.h>

using namespace std;

#define LOG_TAG "VISION"
#define CONFIG_XML "./config.xml"

#endif

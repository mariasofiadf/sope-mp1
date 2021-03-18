#ifndef LOG_H_
#define LOG_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

enum event {
    PROC_CREAT,
    PROC_EXIT,
    SIGNAL_RECV,
    SIGNAL_SENT,
    FILE_MODF,
    INIT,
};

struct timeval start_time;


void write_log(enum event event);


#endif  //LOG_H_


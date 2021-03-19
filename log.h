#ifndef LOG_H_
#define LOG_H_
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

enum event {
    PROC_CREAT,
    PROC_EXIT,
    SIGNAL_RECV,
    SIGNAL_SENT,
    FILE_MODF,
    INIT,
};

struct timeval start_time;


void write_log(enum event event, char* info);

double get_time_var();

void set_time_var(double time);

#endif  //LOG_H_


#include "./log.h"

char* events[] = {
    "PROC_CREAT",
    "PROC_EXIT",
    "SIGNAL_RECV",
    "SIGNAL_SENT",
    "FILE_MODF"};


char* infos[] = {
    "PROC_CREAT",
    "PROC_EXIT",
    "SIGINT",
    "SIGNAL_SENT",
    "FILE_MODF"};

void write_log(enum event event, char* info){
    FILE* file;
    int pid = getpid();
    char* log_filename = getenv("LOG_FILENAME");
    if(log_filename == NULL){
        fprintf(stderr, "Error opening log file");
        return;
    }
    
    if(event == INIT){
        file = fopen(log_filename, "w+");
        fclose(file);
        return;
    }

    file = fopen(log_filename, "a+");
        
    if(file == NULL){
        fprintf(stderr, "Error opening file");
        return;       
    }
    
    struct timeval this_time;
    if( gettimeofday(&this_time, NULL)){
        fprintf(stderr, "Error getting time");
        return;
    }
    //Time diference in milliseconds
    //double start_t = ((double)start_time.tv_sec)*1000 + start_time.tv_usec/1000;
    double start_t = start_time.tv_usec/1000;
    //double this_t = ((double)this_time.tv_sec)*1000 + this_time.tv_usec/1000;
    double this_t = this_time.tv_usec/1000;
    double time_diff = this_t - start_t;

    //instant ; pid ; action ; info
    fprintf(file, "%f ; %d ; %s ; %s\n", time_diff, pid, events[event], info);  
    fclose(file);
}


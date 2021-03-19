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

double time_starter() {
        double result;
        int env_value = 0;
        char *old_env = NULL;
        char *new_env = NULL;


        if(getpgrp() == getpid()){
            asprintf (&new_env, "%ld", (start_time.tv_usec/1000));
            result = (double) *new_env;
            setenv ("TIME_START", new_env, !0);
            free (new_env);
            //printf("Set intial time\n");
        }
        else{
            old_env = getenv("TIME_START");
            if (NULL != old_env && 1 == sscanf (old_env, "%d", &env_value)) {
                result = env_value;
            }
        }
        return result;
}

void set_time_var(double time){
    double result;
    int env_value = 0;
    char *new_env = NULL;
    asprintf (&new_env, "%f", time);
    setenv ("TIME_START", new_env, !0);
    free (new_env);
    //printf("Set intial time: %f\n", time);
}

double get_time_var() {
    double result;
    int env_value = 0;
    char *env = NULL;
    env = getenv("TIME_START");
    if (NULL != env && 1 == sscanf (env, "%d", &env_value)) {
        result = env_value;
    }
    //printf("Get initial time: %f\n", result);
    return result/(double)1000;
}

void write_log(enum event event, char* info){
    FILE* file;
    int pid = getpid();
    char* log_filename = getenv("LOG_FILENAME");
    struct timeval this_time;
    if( gettimeofday(&this_time, NULL)){
        fprintf(stderr, "Error getting time");
        return;
    }
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
    
    double start_t;
    double this_t;
    double time_diff;
    
    //First process
    if(getpgrp() == getpid() && event ==PROC_CREAT){
        start_t = this_time.tv_usec;
        this_t = this_time.tv_usec;
        set_time_var(start_t);
    }
    else
    {
        start_t =  get_time_var();
        this_t = this_time.tv_usec/((double)1000);
        time_diff = (this_t - start_t);
    }

    //instant ; pid ; action ; info
    fprintf(file, "%f ; %d ; %s ; %s\n", time_diff, pid, events[event], info);  
    fclose(file);
} 
   

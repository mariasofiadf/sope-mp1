#ifndef XMOD_H_
#define XMOD_H_
#define MASK_LAST_3_OCTAL_DIGITS 0777u;
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "./mode_t_aux.h"
#include "./log.h"

unsigned int nftot = 0, nfmod = 0;
char* pathname = NULL;
extern struct timeval start_time;

int assembleModeInfo(char* modeChar, struct modeInfo* modeInfo, mode_t* mode);

void xmod(const char *pathname, mode_t * mode, char* modeStr);

int is_regular_file(const char *pathname);

void recursive_step(char* pathname, mode_t *mode, int argc, char** argv);

void print_process_info();

void set_sig_action();

int main(int argc, char** argv);

#endif // XMOD_H_
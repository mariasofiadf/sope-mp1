#include <string.h>
#include "./mode_t_aux.h"
#include "./signal_aux.h"
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

extern unsigned int nftot, nfmod;
extern char* pathname;

void xmod(const char *pathname, mode_t mode){
    chmod(pathname, mode);
}

///Assembles modeInfo struct
int assembleModeInfo(char* modeChar, struct modeInfo* modeInfo, mode_t* mode){

    //printf("Assemblying Mode Info\n");
    char user = modeChar[0], symbol = modeChar[1];

    if(modeChar == NULL) {
        printf("Error: modeChar is NULL!");
        return 1;
    }
    if(modeInfo == NULL){
        printf("Error: modeInfo is NULL!");
        return 1;
    }

    //Set User
    switch (user)
    {
        case 'u': modeInfo->user = OWNER; break;
        case 'g': modeInfo->user = GROUP; break;
        case 'o': modeInfo->user = OTHERS; break;
        case 'a': modeInfo->user = ALL; break;
        default: 
            fprintf( stderr, "assembleModeInfo: Invalid user %c", user);
            break;
    }

    //Set Symbol
    switch (symbol)
    {
        case '+': modeInfo->symbol = ADD; break;
        case '-': modeInfo->symbol = RM; break;
        case '=': modeInfo->symbol = SUBST; break;
        default: 
            fprintf( stderr, "assembleModeInfo: Invalid symbol %c", symbol);
            break;
    }

    //HAVE TO VERIFY THAT THE ORDER IS CORRECT
    int i= 2;

    //remove previous permissions
    if(modeInfo->symbol ==  SUBST){
        if(modeInfo->user == ALL)
            *mode &= 0x111000;
        else
        {
            *mode = mode_rm(*mode, modeInfo->user, WRITE);
            *mode = mode_rm(*mode,  modeInfo->user, READ);
            *mode = mode_rm(*mode,  modeInfo->user, EXECUTE);
        }
    }

    while(modeChar[i] != 0){ //not sure is the right condition
        switch (modeChar[i++])
		{
		case 'r':
			*mode = change_perm(*mode, modeInfo->user, READ, modeInfo->symbol);
			break;
		case 'w':
			*mode = change_perm(*mode, modeInfo->user, WRITE, modeInfo->symbol);
			break;
		case 'x':
			*mode = change_perm(*mode, modeInfo->user, EXECUTE, modeInfo->symbol);
			break;
		default:
			return 1;
		}
    }

    return 0;
}

int is_regular_file(const char *pathname) {
    //printf("IsRegFile");
    struct stat sb;
    if (stat(pathname, &sb) == 0 && S_ISDIR(sb.st_mode)){
    return 0;
    }
    return 1;
}

/*
void recursive_func(int argc, char** argv) {
    struct stat fileStat;
    mode_t mode;
    char** argv2;
    char* modeChar = argv[argc- 2]; pathname = argv[argc- 1];
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(pathname);
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
            {
                strcpy(path, pathname);
                strcat(path, "/");
                strcat(path, dp->d_name);
                printf("%s   %i\n",path ,is_regular_file(path));
                if(stat(path, &fileStat) < 0) {

                }
                mode = fileStat.st_mode;
                if(modeChar[0] == '0'){
                    mode = strtol(path,0,8);
                }
                else
                {
                    //printf("Entered else\n");
                    struct modeInfo modeInfo;
                    assembleModeInfo(modeChar, &modeInfo, &mode);
                    //printf("Final mode_t: %o\n", mode);
                }
                xmod(path, mode);
                if (is_regular_file(path) != 0) {
                    printf("FOUND A PASTA \n");

                    
                    if (fork() == 0) {
                        argv2 = argv;
                        argv2[argc- 1] = pathname;
                        execv("./xmod", argv2);
                    } 

                }               
            }
            
    }
    closedir(dir);
}
*/

void recursive_step(char* pathname, mode_t mode, int argc, char** argv){
    if(is_regular_file(pathname)){
        //printf("Current pathname: %s\n", pathname);
        xmod(pathname, mode);
        return;
    }
    else
    {
        xmod(pathname, mode);
        DIR *dir = opendir(pathname); 
        char next_pathname[256];
        struct dirent *dp;
        while ((dp = readdir(dir)) != NULL){
            if(!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
                continue; 
            strcpy(next_pathname, pathname);
            strcat(next_pathname, "/");
            strcat(next_pathname, dp->d_name);
            
            argv[argc -1] = next_pathname;
            int fork_pid = fork();
            switch (fork_pid)
            {
            case 0:
                // Child
                printf("I'm a new process: %d\n", getpid());
                execv("./xmod", argv);
                break;
            case -1:
                //Erro
                printf("Erro");
                break;
            default: 
                //Parent
                printf("I'm the parent: %d\n", getpid());
                wait(NULL);
                break;
            }
            
        }
        closedir(dir);
    }
}

int main(int argc, char** argv){
    set_sig_action();
    
    if(argc < 2) return 1;    
    struct stat fileStat;
    
    char* modeChar = argv[argc- 2]; pathname = argv[argc - 1];

    printf("PID:%d Current pathname: %s\n", getpid(), pathname);

    if(stat(pathname, &fileStat) < 0)    
        return 1;

    mode_t mode = fileStat.st_mode;

    char mode_R = 45;
    
    //printf("Initial mode_t: %o\n", mode);

    if(modeChar[0] == '0'){
        mode = strtol(argv[argc- 2],0,8);
    }
    else
    {
        //printf("Entered else\n");
        struct modeInfo modeInfo;
        assembleModeInfo(modeChar, &modeInfo, &mode);
        //printf("Final mode_t: %o\n", mode);
    }
    if (*(argv[argc - 3]) == mode_R) {
        //printf("%s   %i\n",pathname ,is_regular_file(pathname));
        //recursive_func(argc,argv);
        recursive_step(pathname, mode, argc, argv);
    }
    else{
        xmod(pathname, mode);
    }


    //pause(); // Descomentando esta linha o program vai esperar por um sinal. Clicando Ctrl+C ele chama a função print_process_info() de signal_aux.c
    return 0;
}


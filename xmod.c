#include "./xmod.h"

u_int8_t change_option = 0, verbose_option = 0, recursive_option = 0;

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



    //remove previous permissions
    if(modeInfo->symbol ==  SUBST){
        if(modeInfo->user == ALL)
            *mode &= 0111000;
        else
        {
            *mode = mode_rm(*mode, modeInfo->user, WRITE);
            *mode = mode_rm(*mode,  modeInfo->user, READ);
            *mode = mode_rm(*mode,  modeInfo->user, EXECUTE);
        }
    }

        //HAVE TO VERIFY THAT THE ORDER IS CORRECT
    int i= 2;
    modeInfo->read = 0;
    modeInfo->write = 0;
    modeInfo->execute = 0;
    while(modeChar[i] != 0){ //not sure is the right condition
        switch (modeChar[i++])
		{
		case 'r':
            if((modeInfo->read || modeInfo->write || modeInfo->execute)){
                fprintf(stderr, "Error: Wrong permission given (read)\n");
                return 1;
            }
            *mode = change_perm(*mode, modeInfo->user, READ, modeInfo->symbol);
            modeInfo->read = 1;
			break;
		case 'w':
            if(modeInfo->write || modeInfo->execute){
                fprintf(stderr, "Error: Wrong permission given(write)\n");
                return 1;
            }
            *mode =change_perm(*mode, modeInfo->user, WRITE, modeInfo->symbol);
            modeInfo->write= 1;
			break;
		case 'x':
            if(modeInfo->execute){
                fprintf(stderr, "Error: Wrong permission given(exe)\n");
                return 1;
            }
			*mode =change_perm(*mode, modeInfo->user, EXECUTE, modeInfo->symbol);
            modeInfo->execute = 1;
			break;
		default:
            fprintf(stderr, "Error: Wrong permission given\n");
			return 1;
		}
    }

    return 0;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
int xmod(const char *pathname, mode_t * mode, char* modeStr){

    char first_char = modeStr[0];

    //Save old permisisons used in info
    mode_t oldPerm = *mode;

    //mode_t MASK_LAST_3_OCTAL_DIGITS 0777u;
    oldPerm = oldPerm & MASK_LAST_3_OCTAL_DIGITS;
    
    //if first_char == '0' it means that the mode to be set need no processings
    if(first_char == '0'){
        *mode = strtol(modeStr,0,8);
    } else {
        struct modeInfo modeInfo;
        if(assembleModeInfo(modeStr, &modeInfo, mode)){
            fprintf(stderr, "Error: Assemble Mode\n");
            return 1;
        }
        //printf("mode: %o", *mode);
    }
    *mode = *mode & MASK_LAST_3_OCTAL_DIGITS;

    //Increments total number of files
    nftot += 1;
    
    if(oldPerm == *mode){
        //If vverbose_option is on, it prints files that didn't change
        if(verbose_option)
            printf("modo de %s mantido como %o\n",pathname, *mode);

    } else {

        //Increments number of files modified
        nfmod += 1;

        //If verbose_option or change_option is on, it prints files with changes
        if(verbose_option || change_option)
            printf("modo de %s alterado de %o para %o\n",pathname, oldPerm, *mode);

        //Writes in the log if changes where made
        char info[256] = "";
        snprintf(info, sizeof(info), "%s ; 0%o ; 0%o", pathname, oldPerm, *mode);
        write_log((enum event) FILE_MODF, info);
        chmod(pathname, *mode);
    }
    return 0;

}

///Assembles modeInfo struct

int is_regular_file(const char *pathname) {
    //printf("IsRegFile");
    struct stat sb;
    if (stat(pathname, &sb) == 0 && S_ISDIR(sb.st_mode)){
    return 0;
    }
    return 1;
}

void recursive_step(char* pathname, mode_t *mode, int argc, char** argv){
    char* modeStr= argv[argc - 2];
    if(getpgrp() != getpid()) //Only children enter here
        xmod(pathname, mode, modeStr);
    DIR *dir = opendir(pathname); 
    char next_pathname[1000];
    struct dirent *dp;
    
    
    while ((dp = readdir(dir)) != NULL){
        if(!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
            continue;
        snprintf(next_pathname, sizeof(next_pathname) , "%s/%s", pathname,dp->d_name);
        argv[argc -1] = next_pathname;
        if(is_regular_file(next_pathname)){
            xmod(next_pathname, mode, modeStr);
        } else {
            int fork_pid = fork();
            switch (fork_pid)
            {
            case 0:
                // Child               
                execv("./xmod", argv);
                printf("Chegueiiii!");
                break;
            case -1:
                //Erro
                printf("Erro");
                break;
            default: 
                //Parent
                while (wait(NULL) == 0) //Waits for child to be done
                {
                    // close(pp[ReadEnd]);
                    // write(pp[WriteEnd], nftot, sizeof(nftot));
                    // close(pp[WriteEnd]);

                }   
                break;
            }
        }
            
    }
        //xmod(pathname, mode, modeStr);
    closedir(dir);  
}



void print_process_info(){
    int pid = getpid();
    char answer;
    char * info = "SIGINT";
    write_log((enum event) SIGNAL_RECV, info);
    printf("\n%d ; %s ; %d ; %d\n", pid, pathname, nftot, nfmod);
    do{
        printf("Do you want to terminate the program? (y/n)\n");
        scanf("%c", &answer);
        if(answer == 'n'){
            break;
        }else if(answer == 'y'){
            exit(0);
        }
    }while(answer!= 'y' && answer != 'n');
    
        
}

void set_sig_action(){
	struct sigaction new, old;
	sigset_t smask; // defines signals to block while func() is running

		// prepare struct sigaction
	if (sigemptyset(&smask)==-1)	// block no signal
		perror ("sigsetfunctions");

	new.sa_handler = print_process_info;
	new.sa_mask = smask;
	new.sa_flags = 0;	// usually works

	if(sigaction(SIGINT, &new, &old) == -1)
		perror ("sigaction");
}

int check_options(int argc, char** argv){
    for(int i = 1; i < argc - 2; i++)
    {
        if(!strcmp(argv[i], "-R"))
            recursive_option = 1;
        else if(!strcmp(argv[i], "-v"))
            verbose_option = 1;
        else if(!strcmp(argv[i], "-c"))
            change_option = 1;
        else {
            return 1;
        }
    }
    return 0;
}


void argv_to_str(int argc, char**argv, char* str)
{
    for(int i = 0; i < argc; i++){
        strcat(str, argv[i]);
        strcat(str, " ");
    }
}



int main(int argc, char** argv){

    //Prepares program to respond to SIGINT signal
    set_sig_action();

    //Activates chosen options eg. verbose_option, change_option, recursive_option
    if(check_options(argc, argv)) {
        fprintf(stderr, "Error Checking options\n");
        return 1;
    } 

    
    //For the parent process
    if(getpgrp() == getpid()){

        //Opens and clears log file
        write_log((enum event)INIT, NULL);

        //Saves start time of parent process
        if( gettimeofday(&start_time, NULL)){
            fprintf(stderr, "Error getting time\n");
            return 1;
        }
    } 

    //String that will have the arguments
    char info[256] = "";

    //Transforms argv into a string
    argv_to_str(argc, argv, info);

    //Registers process creation on the logfile
    write_log((enum event)PROC_CREAT, info);

    struct stat fileStat;
    
    char* modeStr = argv[argc - 2]; pathname = argv[argc - 1];

    if(stat(pathname, &fileStat) < 0)    
        return 1;

    mode_t mode = fileStat.st_mode;

    if (getpgrp() == getpid()){
        if(xmod(pathname, &mode, modeStr)){
            
            exit(0);
        }
    }
    if(recursive_option){
        recursive_step(pathname, &mode, argc, argv);   
    }

    write_log((enum event) PROC_EXIT, info);

    if (getpgrp() == getpid()){
        printf("nftot: %d\n", nftot);
        printf("nfmod: %d\n", nftot);
    }

    return 0;
}


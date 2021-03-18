#include "./xmod.h"

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

void xmod(const char *pathname, mode_t * mode, char* modeStr){

    printf("PID:%d Current pathname: %s\n", getpid(), pathname);
    char first = modeStr[0];

    if(first == '0'){
        *mode = strtol(modeStr,0,8);
    }
    else
    {
        //printf("Entered else\n");
        struct modeInfo modeInfo;
        assembleModeInfo(modeStr, &modeInfo, mode);
        //printf("Final mode_t: %o\n", mode);
    }

    chmod(pathname, *mode);
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
    if(is_regular_file(pathname)){
        //printf("Current pathname: %s\n", pathname);
        xmod(pathname, mode, modeStr);
        return;
    }
    else
    {
        if(getpgrp() != getpid()) //Only children enter here
            xmod(pathname, mode, modeStr);
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
                //printf("I'm a new process: %d\n", getpid());
                execv("./xmod", argv);
                break;
            case -1:
                //Erro
                printf("Erro");
                break;
            default: 
                //Parent
                //printf("I'm the parent: %d\n", getpid());
                //writeLog(PROC_CREAT, getpid(), start_time);
                wait(NULL);

                break;
            }
            
        }
        closedir(dir);
    }
    
}

void print_process_info(){
    int pid = getpid();
    char answer;
    printf("\n%d ; %s ; %d ; %d\n", pid, pathname, nftot, nfmod);
    do{
        printf("Do you want to terminate the program? (y/n)\n");
        scanf("%c", &answer);
        if(answer == 'n'){
            
            //writeLog(getpid(), SIGNAL_SENT, "SIGKILL : 0", &processData);
            break;
        }else if(answer == 'y')
            exit(0);
    }while(answer!= 'y' && answer != 'n');
    
        
}

void set_sig_action(){
	struct sigaction new, old;
	sigset_t smask;
			// defines signals to block while func() is running

		// prepare struct sigaction
	if (sigemptyset(&smask)==-1)	// block no signal
		perror ("sigsetfunctions");

	new.sa_handler = print_process_info;
	new.sa_mask = smask;
	new.sa_flags = 0;	// usually works

	if(sigaction(SIGINT, &new, &old) == -1)
		perror ("sigaction");
}


int main(int argc, char** argv){
    enum event ev = INIT;

    set_sig_action();
    if(getpgrp() == getpid()){
        write_log(ev);
        printf("get time\n");
        if( gettimeofday(&start_time, NULL)){
            fprintf(stderr, "Error getting time");
            return 1;
        }
    }
    ev = PROC_CREAT;
    write_log(ev);


    if(argc < 2) return 1;
    struct stat fileStat;
    
    char* modeStr = argv[argc - 2]; pathname = argv[argc - 1];
    char* option1 = (argv[argc - 3]);
    char* option2 = malloc(sizeof(char) * 1024);
    char* option3 = malloc(sizeof(char) * 1024);
    if (argc == 5) {
        option2 = (argv[argc - 4]);
    } else if (argc == 6) {
        option2 = (argv[argc - 4]);
        option3 = (argv[argc - 5]);
    }
    //printf("PID:%d Current pathname: %s\n", getpid(), pathname);

    if(stat(pathname, &fileStat) < 0)    
        return 1;

    mode_t mode = fileStat.st_mode;
    char mode_R = 45;

    if(getpgrp() == getpid()){
        //First process
        xmod(pathname, &mode, modeStr);
    }
    if((*option3 == mode_R) || (*option2 == mode_R) || (*option1 == mode_R)){
        //printf("%s   %i\n",pathname ,is_regular_file(pathname));
        //recursive_func(argc,argv);
        recursive_step(pathname, &mode, argc, argv);    
    }
    ev = PROC_EXIT;
    write_log(ev);
    return 0;
}


#include <string.h>
#include "./mode_t_aux.h"
#include "./signal_aux.h"

extern unsigned int nftot, nfmod;
extern char* pathname;

void xmod(const char *pathname, mode_t mode){
    chmod(pathname, mode);
}

///Assembles modeInfo struct
int assembleModeInfo(char* modeChar, struct modeInfo* modeInfo, mode_t* mode){

    printf("Assemblying Mode Info\n");
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
            if(!(modeInfo->read || modeInfo->execute || modeInfo->write)){
                //printf("All good read!\n");
                modeInfo->read = 1;
                *mode = change_perm(*mode, modeInfo->user, READ, modeInfo->symbol);
            }else{
                printf( "Error: wrong permission input\n");
            }
			
			break;
		case 'w':
            if(!(modeInfo->execute || modeInfo->write)){
                //printf("All good write!\n");
                *mode = change_perm(*mode, modeInfo->user, WRITE, modeInfo->symbol);
                modeInfo->write = 1;
            }else{
                fprintf( stderr, "Error: wrong permission input\n");
            }	
			break;
		case 'x':
            if(!modeInfo->execute){
                //printf("All good execute!\n");
                *mode = change_perm(*mode, modeInfo->user, EXECUTE, modeInfo->symbol);
                modeInfo->execute = 1;
            }else{
                fprintf( stderr, "Error: wrong permission input\n");
            }
			
			break;
		default:
			return 1;
		}
    }

    return 0;
}


int main(int argc, char** argv){
    set_sig_action();
 
    if(argc < 2) return 1;    
    struct stat fileStat;
    
    char* modeChar = argv[1]; pathname = argv[2];


    if(stat(pathname, &fileStat) < 0)    
        return 1;

    mode_t mode = fileStat.st_mode;
    //printf("Initial mode_t: %o\n", mode);

    if(modeChar[0] == '0'){
        mode = strtol(argv[1],0,8);
    }
    else
    {
        printf("Entered else\n");
        struct modeInfo modeInfo;
        assembleModeInfo(modeChar, &modeInfo, &mode);
        printf("Final mode_t: %o\n", mode);
    }
    xmod(pathname, mode);
    //pause(); Descomentando esta linha o program vai esperar por um sinal. Clicando Ctrl+C ele chama a função print_process_info() de signal_aux.c
    return 0;
}


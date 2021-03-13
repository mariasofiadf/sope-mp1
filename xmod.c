#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

enum user { OWNER, GROUP, OTHERS, ALL};
enum symbol {RM, ADD, SUBST};
enum permission {READ, WRITE, EXECUTE};

struct modeInfo{
    enum user user;
    enum symbol symbol;
    u_int8_t read;
    u_int8_t write;
    u_int8_t execute;
};

void xmod(const char *pathname, mode_t mode){
    chmod(pathname, mode);
}

///Assembles modeInfo struct
int assembleModeInfo(char* modeChar, struct modeInfo* modeInfo){

    printf("Assemblying Mode Info\n");

    if(modeChar == NULL) {
        printf("Error: modeChar is NULL!");
        return 1;
    }
    if(modeInfo == NULL){
        printf("Error: modeInfo is NULL!");
        return 1;
    }

    //Set User
    switch (modeChar[0])
    {
    case 'u': modeInfo->user = OWNER; break;
    case 'g': modeInfo->user = GROUP; break;
    case 'o': modeInfo->user = OTHERS; break;
    case 'a': modeInfo->user = ALL; break;
    default: break;
    }

    //Set Symbol
    switch (modeChar[1])
    {
        case '+': modeInfo->symbol = ADD; break;
        case '-': modeInfo->symbol = RM; break;
        case '=': modeInfo->symbol = SUBST; break;
        default: break;
    }

    //Check for Read Permissions
    switch (modeChar[2])
    {
        case 'r': modeInfo->read = 1;
        default: break;
    }

    //Check for Write Permissions
    switch (modeChar[3])
    {
        case 'w': modeInfo->write = 1;
        default: break;
    }

    //Check for Execute Permissions
    switch (modeChar[4])
    {
        case 'x': modeInfo->execute = 1;
        default: break;
    }

    return 0;
}


///Assembles mode_t when user = OWNER
void assembleModeTOwner(struct modeInfo * modeInfo, mode_t* mode){

    printf("Assembly Owner\n");
    switch (modeInfo->symbol)
    {
        case ADD: 
            //TODO
            break;
        case RM: 
            //TODO
            break;
        case SUBST:
            if(modeInfo->read)  *mode = *mode | S_IRUSR;
            if(modeInfo->write) *mode = *mode | S_IWUSR;
            if(modeInfo->execute) *mode = *mode | S_IXUSR;
            break;
        default:
        break;
    }
}


///Assembles mode_t when user = GROUP
void assembleModeTGroup(struct modeInfo * modeInfo, mode_t* mode){

    printf("Assembly Group\n");
    switch (modeInfo->symbol)
    {
        case ADD: 
            //TODO
            break;
        case RM: 
            //TODO
            break;
        case SUBST:
            if(modeInfo->read)  *mode = *mode | S_IRGRP;
            if(modeInfo->write) *mode = *mode | S_IWGRP;
            if(modeInfo->execute) *mode = *mode | S_IXGRP;
            break;
        default:
        break;
    }
}


///Assembles mode_t when user = OTHERS
void assembleModeTOthers(struct modeInfo * modeInfo, mode_t* mode){
    printf("Assembly Others\n");
    switch (modeInfo->symbol)
    {
        case ADD: 
            //TODO
            break;
        case RM: 
            //TODO
            break;
        case SUBST:
            if(modeInfo->read)  *mode = *mode | S_IROTH;
            if(modeInfo->write) *mode = *mode | S_IWOTH;
            if(modeInfo->execute) *mode = *mode | S_IXOTH;
            break;
        default:
        break;
    }
}


///Assembles mode_t when user = ALL
void assembleModeTAll(struct modeInfo * modeInfo, mode_t* mode){
    printf("Assembly All\n");
    switch (modeInfo->symbol)
    {
        case ADD: 
            //TODO
            break;
        case RM: 
            //TODO
            break;
        case SUBST:
            if(modeInfo->read)  *mode = *mode | S_IRUSR | S_IRGRP | S_IROTH;
            printf("mode_t: %o\n", *mode);
            if(modeInfo->write) *mode = *mode | S_IWUSR | S_IWGRP |S_IWOTH;
            printf("mode_t: %o\n", *mode);
            if(modeInfo->execute) *mode = *mode | S_IXUSR | S_IXGRP | S_IXOTH;
            printf("mode_t: %o\n", *mode);
        default:
        break;
    }
}


///Assembles mode_t
///
///Calls one of the following: 
///assembleModeTOwner
///assembleModeTGroup
///assembleModeTOthers
///assembleModeTAll
void assembleModeT(struct modeInfo * modeInfo, mode_t* mode){
    switch (modeInfo->user)
    {
        case OWNER: assembleModeTOwner(modeInfo, mode); break;
        case GROUP: assembleModeTGroup(modeInfo, mode); break; 
        case OTHERS: assembleModeTOthers(modeInfo, mode); break;
        case ALL: assembleModeTAll(modeInfo, mode); break;
        default: break;
    }
}

int main(int argc, char** argv){
    if(argc < 2) return 1;
    mode_t mode = 0;
    char* modeChar = argv[1], *pathname = argv[2];

    if(modeChar[0] == '0'){
        mode = strtol(argv[1],0,8);
    }
    else
    {
        printf("Entered else\n");
        struct modeInfo modeInfo;
        assembleModeInfo(modeChar, &modeInfo);
        /*
        Para confirmar que o modeInfo está a ser inicializado corretamente. Está!
        printf("modeInfo->user: %d\n", modeInfo.user);
        printf("modeInfo->symbol: %d\n", modeInfo.symbol);
        printf("modeInfo->read: %d\n", modeInfo.read);
        printf("modeInfo->write: %d\n", modeInfo.write);
        printf("modeInfo->execute: %d\n", modeInfo.execute);
        */
        assembleModeT(&modeInfo, &mode);
        printf("mode_t: %o\n", mode);
    }

    xmod(pathname, mode);
    return 0;
}

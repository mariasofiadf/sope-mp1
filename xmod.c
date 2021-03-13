#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char** argv){
    mode_t mode = S_IWUSR;
    chmod("./text.txt", mode);
    return 0;
    
}
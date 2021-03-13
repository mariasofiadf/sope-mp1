#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char** argv){
    mode_t mode = S_IWUSR | S_IRUSR | S_IXUSR;
    chmod("./text.txt", mode);
    return 0;
    
}
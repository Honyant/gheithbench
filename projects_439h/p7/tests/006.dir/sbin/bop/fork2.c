#include "../libc.h"

int charToInt(char* str){
    int len = 0;
    int num = 0;
    while(str[len] != '\0'){
        len++;
    }
    for(int i = 0; i < len; i++){
        num = num*10 + (int)str[i] - 48;
    }
    return num;
}

int main(int argc, char** argv){
    for(int i = 0; i < argc - 1; i++){
        if(i < argc - 2){
            printf("%s ", argv[i]);
        }
        else{
            printf("%s", argv[i]);
        }
    }

    int sem_fd = charToInt(argv[argc-1]);

    up(sem_fd); //sync with parent

    exit(0);
    return 0;
}
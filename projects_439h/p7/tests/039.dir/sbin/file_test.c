#include "stdint.h"
#include "libc.h"

int main(int argc, char** argv){
    int fd = open("/sbin/doc1.txt", 0);
    int id = fork();

    if (id < 0) {
        printf("*** fork failed\n");
    }
    else if (id == 0) {
        close(fd);
        return 0;
    } 

    uint32_t status = 42;
    wait(id, &status);

    char* c = (char*)malloc(5);
    int a = 0;
    while (a != 5) a = read(fd, c, 5);
    
    // if we never reach this line, then read may be stuck in an 
    // infinite loop or the process may have been killed. 
    // neither of these should happen

    // "We're" is the first 5 chars in doc1.txt
    char* s = "We're";
    for (int i = 0; i < 5; i++){
        if (c[i] != s[i]) 
            printf("*** read the wrong thing\n"); 
    }
    free(c);
    return 0;
}
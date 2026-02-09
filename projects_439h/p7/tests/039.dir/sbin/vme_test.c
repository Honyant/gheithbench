#include "stdint.h"
#include "libc.h"
#include "sys.h"

int main(int argc, char** argv){
    // We first want to create VME entries for both processes to share
    int fd = open("/sbin/doc1.txt", 0);
    int fd2 = open("/sbin/doc2.txt", 0);

    void* p = naive_mmap(5, 0, fd, 0); // we're
    void* p2 = naive_mmap(4, 1, fd2, 0); // This

    int id = fork();
    if (id < 0) {
        printf("*** fork failed in vme_test\n");
        return 0;
    }
    else if (id == 0) {
        char* s = "We're";
        for (int i = 0; i < 5; i++)
            if (((char*)p)[i] != s[i]) 
                printf("*** fork didn't copy private mappings\n");
        
        char* s2 = "This";
        for (int i = 0; i < 4; i++)
            if (((char*)p2)[i] != s2[i])
                printf("*** fork didn't copy shared mappings\n");
        
        return 0;
    } else {
        uint32_t status = 42;
        wait(id, &status);
        return 0;
    }
}
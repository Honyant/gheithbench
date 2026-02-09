#include "libc.h"
// #include "heap.c"


void one(int fd) {
    printf("*** fd = %d\n",fd);
    printf("*** len = %d\n",len(fd));

    cp(fd,2);
}


int main(int argc, char** argv) {
    printf("*** %d\n",argc);
    for (int i=0; i<argc; i++) {
        printf("*** %s\n",argv[i]);
    }
    
    int basic_cat = open("/etc/BASIC_CAT.txt",0);
    one(basic_cat);
    
    printf("*** start!\n");
    int rc = execl("/sbin/child", "child", "1", 0);
    printf("*** execl failed, rc = %d\n",rc);

    shutdown();
    return 0;
}

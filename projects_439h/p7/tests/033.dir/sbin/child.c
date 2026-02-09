#include "libc.h"
// #include "heap.c"


void one(int fd) {
    printf("*** fd = %d\n",fd);
    printf("*** len = %d\n",len(fd));

    cp(fd,2);
}

int main(int argc, char** argv) {
    int child_id = (int) (argv[1][0] - '0');
    printf("*** child process: %d\n", child_id);
    
    // recursively calls execl and fork!
    if (child_id < 15) {
        int id = fork();
        if (id < 0) {
            printf("*** fork failed");
        } else if (id == 0) {
            /* child */
            printf("*** in child\n");
            const char N[2] = {child_id + '0' + 1, '\0'};
            int rc = execl("/sbin/child", "child", N, 0);
            printf("*** execl failed, rc = %d\n",rc);
        } else {
            /* parent */
            uint32_t status = 42;
            wait(id,&status);
            printf("*** [process %d] back from wait %ld\n", child_id, status);
            if (child_id == 1) {
                int meow = open("/etc/MEOW.txt",0);
                one(meow);
                shutdown();
            }
        }
    }

    exit(child_id);
    return 0;
}

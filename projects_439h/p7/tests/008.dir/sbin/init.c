#include "libc.h"

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

    // This testcase will essentially just start a process that will calculate the nth fibonacci number.
    // This process will recursively fork to figure out the number

    int n = 10;
    int child1 = fork();

    if (child1 < 0) {
        printf("*** Failed\n");
        exit(-1);
    }
    else if (child1 == 0) {
        char arg[2];
        arg[0] = (n - 1) + '0';
        arg[1] = '\0';
        int rc = execl("/sbin/fib","fib", arg, 0);        
        printf("*** exec failed %d\n", rc);
        exit(-1);
    }
    
    uint32_t ans = 0;
    wait(child1,&ans);

    int child2 = fork();
    if (child2 < 0) {
        printf("*** Failed\n");
        exit(-1);
    }
    else if (child2 == 0) {
        char arg[2];
        arg[0] = (n - 2) + '0';
        arg[1] = '\0';
        int rc = execl("/sbin/fib","fib", arg, 0);
        printf("*** exec failed %d\n", rc);
        exit(-1);
    }

    uint32_t p2 = 0;
    wait(child2, &p2);
    ans += p2;

    printf("*** The answer is: %d\n", (int)ans);

    shutdown();
    return 0;
}

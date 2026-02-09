#include "libc.h"

int main(int argc, char** argv) {
    // n will be a one digit number
    // there is just 1 additional argument, n

    int n = argv[1][0] - '0';
    printf("Computing %d\n", n);

    if (n <= 2) {
        exit(1);
    }

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
    exit(ans);
}

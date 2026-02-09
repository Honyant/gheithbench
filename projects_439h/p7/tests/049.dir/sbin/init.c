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
    int fd = open("/etc/data.txt",0);
    one(fd);

    printf("*** close = %d\n",close(fd));

    one(fd);
    one(100);


    printf("*** open again %d\n",open("/etc/data.txt",0));
    printf("*** seek %ld\n",seek(3,17));
    
    int id = fork();

    if (id < 0) {
        printf("fork failed");
    } else if (id == 0) {
        /* child */
        int id2 = fork();
        if(id2 < 0) {
            printf("fork2 failed");
        } else if(id2 == 0) {
            /* grand child */
            printf("*** in grandchild\n");
            int rc2 = execl("/sbin/shell","shell","e","f","g",0);
            printf("*** execl2 failed, rc = %d\n",rc2);
        } else {
            uint32_t status2 = 42;
            wait(id2, &status2);
            printf("*** back from wait2 %ld\n", status2);
            printf("*** in child\n");
            int rc = execl("/sbin/shell","shell","a","b","c",0);
            printf("*** execl failed, rc = %d\n",rc);
        }
    } else {
        /* parent */
        uint32_t status = 42;
        wait(id,&status);
        printf("*** back from wait %ld\n",status);

        int fd = open("/etc/panic.txt",0);
        cp(fd,1);
    }

    shutdown();
    return 0;
}

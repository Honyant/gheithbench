#include "libc.h"

// This test will focus on execl edge cases
int main(int argc, char** argv) {
    printf("*** Test 1: Sanity Check\n");
    {
        int id = fork();

        if (id == 0) {
            int err = execl("/sbin/normal", "*** this", "works!", 0);
            printf("*** execl failed with error code %d\n", err);
        }
        else if (id > 0) {
            uint32_t code = 439;
            wait(id, &code);
            printf("*** Child exited with code %ld\n", code);
        }
        else {
            printf("*** fork failed\n");
        }
    }
    

    printf("*** Test 2: Early Arg Parsing End\n");
    {
        int id = fork();

        if (id == 0) {
            int err = execl("/sbin/normal", "*** this", "is", "the", 0, "end??", 0);
            printf("*** execl failed with error code %d\n", err);
        }
        else if (id > 0) {
            uint32_t code = 439;
            wait(id, &code);
            printf("*** Child exited with code %ld\n", code);
        }
        else {
            printf("*** fork failed\n");
        }
    }

    printf("*** Test 3: Malicious Arguments??\n");
    {
        int id = fork();

        if (id == 0) {
            int err = execl("/sbin/malicious", (const char *)0x10000000, (const char *)0x43728, 0);
            printf("*** execl failed with error code %d\n", err);
            exit(-10);
        }
        else if (id > 0) {
            uint32_t code = 439;
            wait(id, &code);
            printf("*** Child exited with code %ld\n", code);
        }
        else {
            printf("*** fork failed\n");
        }
    }

    printf("*** Test 4: Long String Overflow\n");
    {
        int id = fork();

        if (id == 0) {
            int err = execl("/sbin/normal", "*** this is a long string that might cause a page fault and then exit but what it should not do is overflow and start overriding things in areas that it should not originally be allowed to access. You should also not be limiting the max num of characters to 256 since this doesn't have to be a file name", 0);
            printf("*** execl failed with error code %d\n", err);
            exit(-10);
        }
        else if (id > 0) {
            uint32_t code = 439;
            wait(id, &code);
            printf("*** Child exited with code %ld\n", code);
        }
        else {
            printf("*** fork failed\n");
        }
    }

    printf("*** Test 5: Children Handling\n");
    {
        int id = fork();

        if (id == 0) {
            int children_ids[50];

            for (int i = 0; i < 50; ++i) {
                int id2 = fork();
                if (id == 0) {
                    int err = execl("/sbin/check_child", 0);
                    printf("*** execl failed with error code %d\n", err);
                    exit(-10);
                }
                else if (id > 0) {
                    children_ids[i] = id2;
                }
                else {
                    printf("*** fork failed\n");
                }
            }

            for (int i = 0; i < 50; ++i) {
                uint32_t code = 0;
                wait(children_ids[i], &code);
                
                if (code != -12) {
                    printf("*** Child %d exited with code %ld\n", i, code);
                }
            }
            exit(0);
        }
        else if (id > 0) {
            uint32_t code = 439;
            wait(id, &code);
            printf("*** Child exited with code %ld\n", code);
        }
        else {
            printf("*** fork failed\n");
        }
    }

    printf("*** Test 6: Failed Execl Doesn't Change Parent\n");
    {
        char * ptr = (char *)naive_mmap(4, 0, 5, 0);  // anonymous mmap
        ptr[0] = 'y';
        ptr[1] = 'a';
        ptr[2] = 'y';
        ptr[3] = '\0';
        int id = fork();

        if (id == 0) {
            int err = execl("/sbin/dasfadljafdsjlk", 0);
            printf("*** execl failed with error code %d\n", err);

            err = execl("/sbin/normal", (char *)(0x10000) , 0);
            printf("*** execl failed with error code %d\n", err);
            
            exit(-10);
        }
        else if (id > 0) {
            uint32_t code = 439;
            wait(id, &code);
            printf("*** Child exited with code %ld\n", code);

            // this ptr should still be valid
            printf("*** ptr: %s\n", ptr);
        }
        else {
            printf("*** fork failed\n");
        }
    }

    shutdown();
    return 0;
}

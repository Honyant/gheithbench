#include "libc.h"
#define MAX_SEMAPHORES 100
#define MAX_FILES 97
#define MAX_PROCESSES 100

#define KCONFIG_LAPIC 4276092928
#define KCONFIG_IOAPIC 4273995776
void int_to_string(uint32_t num, char* str) {
    int i = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void testEdgeCases() {

    printf("*** Starting edge case tests ***\n");
    // Edge case for open: file does not exist
    int fd = open("/etc/non_existent_file.txt", 0);
    printf("*** Open non-existent file: %s\n", fd < 0 ? "Failed as expected" : "Unexpected Success");

    // Edge case for read: invalid file descriptor
    char buffer[20];
    int read_result = read(-1, buffer, 20);
    printf("*** Read from invalid fd: %s\n", read_result < 0 ? "Failed as expected" : "Unexpected Success");

    // Edge case for len: invalid file descriptor
    int len_result = len(-1);
    printf("*** Length of invalid fd: %s\n", len_result < 0 ? "Failed as expected" : "Unexpected Success");

    // Edge case for write: invalid file descriptor
    int write_result = write(-1, "Test", 4);
    printf("*** Write to invalid fd: %s\n", write_result < 0 ? "Failed as expected" : "Unexpected Success");

    // Edge case for close: invalid file descriptor
    int close_result = close(-1);
    printf("*** Close invalid fd: %s\n", close_result < 0 ? "Failed as expected" : "Unexpected Success");

    // Edge case for seek: either negative offset or really big number. regardless, it
    // should be a success (seeking outside the file doesn't return an error)
    int seek_result = seek(1, -10);
    printf("*** Seek with negative offset: %s\n", seek_result < 0 ? "Failed as expected" : "Unexpected Success");

    // Edge case for execl: executing a non-existent program
    int execl_result = execl("/sbin/non_existent_program", "non_existent_program", 0);
    printf("*** Execl non-existent program: %s\n", execl_result < 0 ? "Failed as expected" : "Unexpected Success");

    printf("*** Edge case tests complete\n");

}

//Testing that you allow 100 semaphore creations and no more
void testMaxSemaphores() {
    int sem_ids[MAX_SEMAPHORES];
    int sem_id;

    printf("*** Semaphore creation test\n");

    for (int i = 0; i < MAX_SEMAPHORES + 1; i++) {
        sem_id = sem(1);

        if (i < MAX_SEMAPHORES) {
            if (sem_id < 0) {
                printf("*** Unexpected failure creating semaphore #%d\n", i);
                shutdown();
            }
            sem_ids[i] = sem_id;
        } else {
            if (sem_id < 0) {
                printf("*** Passed (Semaphore #%d creation failed as expected)\n", i);
            } else {
                printf("*** Failed (Semaphore #%d created when it should not have been)\n", i);
                shutdown();
            }
        }
    }

    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (close(sem_ids[i]) < 0) {
            printf("*** Failed to close semaphore #%d (ID %d)\n", i, sem_ids[i]);
            shutdown();
        }
    }
}

//Testing that you allow 100 creations and no more
void testMaxFiles() {
    int file_ids[MAX_FILES];
    int fd;

    printf("*** File opening test\n");
    for (int i = 0; i < MAX_FILES + 1; i++) {
        char filename[32];
        int base_length = 21;  // Length of "/hundredonefiles/file"

        // Copy the base path
        memcpy(filename, "/hundredonefiles/file", base_length);

        // Calculate the length of the number part
        int num = i;
        int num_length = 0;
        int num_offset = base_length;

        // Find the number of digits
        if (num == 0) {
            filename[num_offset++] = '0';
            num_length = 1;
        } else {
            while (num > 0) {
                num /= 10;
                num_length++;
            }
            num = i;

            // Insert each digit into filename from back to front
            for (int j = num_length - 1; j >= 0; j--) {
                filename[num_offset + j] = '0' + (num % 10);
                num /= 10;
            }
            num_offset += num_length;
        }

        // Append ".txt" after the number
        memcpy(filename + num_offset, ".txt", 5);
        filename[num_offset + 4] = '\0';  // Null-terminate the string

        fd = open(filename, 0);

        if (i < MAX_FILES) {
            if (fd < 0) {
                printf("*** Unexpected failure opening file #%d\n", i);
                shutdown();
            }
            file_ids[i] = fd;
        } else {
            if (fd < 0) {
                printf("*** Passed (File #%d open failed as expected)\n", i);
            } else {
                printf("*** Failed (File #%d opened when it should not have been)\n", i);
                shutdown();
            }
        }
    }

    // Close all opened files
    for (int i = 0; i < MAX_FILES; i++) {
        if (close(file_ids[i]) < 0) {
            printf("*** Failed to close file #%d (FD %d)\n", i, file_ids[i]);
            shutdown();
        }
    }
}


//tries to write 0's to kernel
void probeAddress(uint32_t address) {
    char stringNum[20];
    int id = fork();
    if (id < 0) {
        printf("fork failed");
    } else if (id == 0) {
        /* child */
        int_to_string(address, stringNum);
        int rc = execl("/sbin/shell","shell", stringNum, 0);
        printf("*** execl failed, rc = %d\n",rc);
    } else {
        /* parent */
        uint32_t status = 42;
        wait(id,&status);
        if (status != -1) {
            printf("*** accessed kernel memory successfully. should not happen\n");
        }
    }
}

void testKernelProtected() {
    printf("*** Testing Kernel Memory is Protected\n");
    //for (uint32_t i = 0x00001000; i < 0x80000000; i += 1000) {
    probeAddress((uint32_t)(0x00001000));
    //}
    printf("*** Kernel safe\n");
}

void testLAPICProtected() {
    printf("*** Testing Lapics Memory is Protected\n");
    probeAddress((uint32_t)KCONFIG_LAPIC);
    probeAddress((uint32_t)KCONFIG_IOAPIC);
    printf("*** Lapics safe\n");
}

int main(int argc, char** argv) {
    testEdgeCases();

    testMaxSemaphores();

    testMaxFiles();

    testKernelProtected();

    testLAPICProtected();

    printf("*** Finished all tcs!\n");
    shutdown();
    return 0;
}

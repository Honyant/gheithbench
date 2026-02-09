#include "libc.h"
#include <stdarg.h>
#include "stdbool.h"

/*
This test verifies:

- You follow the agreed upon specifications for the ID numbering system
- You do not special case stdout/stderr by checking for file descriptor numbers 1 and 2
    - e.g. you are able to close stdout/stderr
- You perform input validation for syscalls
    - e.g. entry type validation for up/down (sems only), write (files only), wait (procs only)
    - e.g. existence checks for the ID passed in to write/up/down/close/wait/len/read/seek/
    - e.g. free space checks for fork/sem/open

For easy debugging, this test will not use execl.
*/

#define MAX_CHILD_PROCESSES 100

int get_unique_id(int count, int ids[]);

int main(int argc, char** argv) {
    printf("*** %d\n",argc);
    for (int i=0; i<argc; i++) {
        printf("*** %s\n",argv[i]);
    }

    printf("*** -----------------------------------------\n");
    printf("*** PART 1: ID Numbering System\n");
    printf("*** -----------------------------------------\n");

    /*
    ID Numbering Requirements:
    - File Descriptors (FDs) are numbered from 0 to 99.
    - You return the lowest available file descriptor.
    - Semaphores and Processes have IDs >= 100.

    My recommendation for a simple implementation:
    - File Descriptor Table (FDT) entries are numbered from 0 to 299.
    - File Descriptors (FDs) are numbered from 0 to 99.
    - Semaphores and Processes are numbered from 100 to 299.

    Your semaphores and processes can have any IDs >= 100.
    A valid implementation could place them from 400-599 for example.
    However, I think that 100-299 is the simplest and most logical choice.

    I will assert that your semaphores and processes are >= 100.
    */

    // Confirm process IDs are >= 100
    int child_id = fork();
    if (child_id < 0) {
        printf("*** fork failed\n");
    } else if (child_id == 0) {
        // Child
        printf("*** I am the child\n");
        exit(0); // No-op: child exits immediately
    } else {
        // Parent
        uint32_t status = 42;
        wait(child_id, &status);
        printf("*** I am the parent. The child returned: %ld\n", status);
        printf("*** child_id is >= 100 ? %s\n", child_id >= 100 ? "yes" : "no");
        close(child_id); 
    }

    // Confirm semaphore IDs are >= 100
    int sem_id = sem(2);
    printf("*** sem_id is >= 100 ? %s\n", sem_id >= 100 ? "yes" : "no");

    printf("*** -----------------------------------------\n");
    printf("*** PART 2: Entry Type Validation\n");
    printf("*** -----------------------------------------\n");

    // Up/Down should only work on semaphores

    // 1 should be the file descriptor for stdout so it should not be a semaphore
    printf("*** up(file ID) fails correctly ? %s\n", up(1) < 0 ? "yes" : "no");
    printf("*** down(file ID) fails correctly ? %s\n", down(1) < 0 ? "yes" : "no");

    // Write should only work on files

    printf("*** write(sem ID) fails correctly ? %s\n", write(sem_id, "hello", 5) < 0 ? "yes" : "no");

    // Wait should only work on processes

    printf("*** wait(sem ID) fails correctly ? %s\n", wait(sem_id, 0) < 0 ? "yes" : "no");

    printf("*** -----------------------------------------\n");
    printf("*** PART 3: Existence Checks\n");
    printf("*** -----------------------------------------\n");

    // Write 
    printf("*** write(nonexistent file ID) fails correctly ? %s\n", write(95, "hello", 5) < 0 ? "yes" : "no");

    // The only active ID is the sem_id as we closed the child process.
    int sem_ids[] = {sem_id};

    int non_existent_sem_id = get_unique_id(1, sem_ids);
    // Up
    printf("*** up(nonexistent sem ID) fails correctly ? %s\n", up(non_existent_sem_id) < 0 ? "yes" : "no");
    // Down
    printf("*** down(nonexistent sem ID) fails correctly ? %s\n", down(non_existent_sem_id) < 0 ? "yes" : "no");

    // Close
    printf("*** close(nonexistent sem ID) fails correctly ? %s\n", close(non_existent_sem_id) < 0 ? "yes" : "no");

    int curr_ids[] = {sem_id, non_existent_sem_id};
    int non_existent_proc_id = get_unique_id(2, curr_ids);
    // Wait
    printf("*** wait(nonexistent proc ID) fails correctly ? %s\n", wait(non_existent_proc_id, 0) < 0 ? "yes" : "no");
    
    // Len
    printf("*** len(nonexistent file ID) fails correctly ? %s\n", len(95) < 0 ? "yes" : "no");
    // Read
    printf("*** read(nonexistent file ID) fails correctly ? %s\n", read(95, 0, 0) < 0 ? "yes" : "no");
    // Seek
    printf("*** seek(nonexistent file ID) fails correctly ? %s\n", seek(95, 0) < 0 ? "yes" : "no");

    printf("*** Cleaning up our semaphore before we move on\n");
    printf("*** closing semaphore works? %s\n", close(sem_id) == 0 ? "yes" : "no");

    printf("*** -----------------------------------------\n");
    printf("*** PART 4: Stdout/Stderr Special Case\n");
    printf("*** -----------------------------------------\n");

    /*
    This tests that you do not special case file descriptor numbers 1 and 2 for stdout/stderr.
    */
    close(2);

    int return_value = write(2, "*** This should not print! stderr has been closed.\n", 60);
    printf("*** closing stderr correctly results in a failed write? %s\n", return_value < 0 ? "yes" : "no");

    int fd = open("random_file", 0);
    printf("*** opening a file returns the lowest available number? %s\n", fd == 2 ? "yes" : "no");
    return_value = write(2, "*** This should not print! This is a random file, not stderr.\n", 70);
    printf("*** write to random file works? %s\n", return_value < 0 ? "yes" : "no");

    child_id = fork();
    if (child_id < 0) {
        printf("*** fork failed \n");
    } else if (child_id == 0) {
        // Child process
        close(1);
        return_value = write(1, "*** This should not print! stdout has been closed.\n", 60);
        fd = open("random_file", 0);
        if (fd != 1) {
            shutdown();
        }
        return_value = write(1, "*** This should not print! This is a random file, not stdout.\n", 70);
        if (return_value >= 0) {
            shutdown();
        }
        exit(0);
    } else {
        // Parent process
        uint32_t status = 42;
        wait(child_id, &status);
        if (status != 0) {
            shutdown();
        }
        close(child_id);
        printf("*** stdout was successfully closed \n");
    }

    printf("*** -----------------------------------------\n");
    printf("*** PART 5: Free Space Checks\n");
    printf("*** -----------------------------------------\n");

    // Sem
    printf("*** Attempting to create 100 semaphores\n");
    for (int i = 0; i < 100; i++) {
        sem_id = sem(2);
        if (sem_id < 0) {
            printf("*** sem failed at attempt %d\n", i);
            break;
        }
        // if (i > 0 && i % 10 == 0) {
        //     printf("*** created %d semaphores\n", i);
        // }
    }
    printf("*** created 100 semaphores\n");
    printf("*** Attempting to create one more semaphore\n");
    sem_id = sem(2);
    if (sem_id < 0) {
        printf("*** sem failed as expected\n");
    } else {
        printf("*** sem succeeded unexpectedly\n");
    }

    // Open
    printf("*** Attempting to open 100 files\n");
    // We already have 3 files open: stdin, stdout, stderr
    for (int i = 0; i < 100 - 3; i++) {
        int fd = open("random_file", 0);
        if (fd < 0) {
            printf("*** open failed at attempt %d\n", i);
            break;
        }
        // if (i > 0 && i % 10 == 0) {
        //     printf("*** opened %d files\n", i);
        // }
    }
    printf("*** opened 100 files\n");
    printf("*** Attempting to open one more file\n");
    fd = open("random_file", 0);
    if (fd < 0) {
        printf("*** open failed as expected\n");
    } else {
        printf("*** open succeeded unexpectedly\n");
    }

    printf("*** Attempting to fork 100 processes\n");
    // Fork
    // We should not be able to fork if there is no free space for a new process
    for (int i = 0; i < MAX_CHILD_PROCESSES; i++) {
        child_id = fork();
        if (child_id < 0) {
            printf("*** fork failed at attempt %d\n", i + 1);
            break;
        } else if (child_id == 0) {
            // Child
            exit(0); // No-op: child exits immediately
        }
        // if (i > 0 && i % 10 == 0) {
        //     printf("*** forked %d processes\n", i);
        // }
    }
    printf("*** forked 100 processes\n");
    printf("*** Attempting to fork one more process\n");
    child_id = fork();
    if (child_id < 0) {
        printf("*** fork failed as expected\n");
    } else if (child_id == 0) {
        // Child process
        exit(0); // No-op: child exits immediately
    } else {
        // Parent process
        printf("*** fork succeeded unexpectedly\n");
    }

    shutdown();
    return 0;
}

#define MIN_ID 100
#define MAX_ID 299

int get_unique_id(int count, int ids[]) {
    for (int id = MIN_ID; id <= MAX_ID; id++) {
        bool is_unique = true;
        for (int i = 0; i < count; i++) {
            if (id == ids[i]) {
                is_unique = false;
                break;
            }
        }
        if (is_unique) {
            return id;
        }
    }
    return -1;
}
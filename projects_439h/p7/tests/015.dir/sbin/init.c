#include "libc.h"
#include "stdint.h"
#include "sys.h"

void one(int fd) {
    printf("*** fd = %d\n",fd);
    printf("*** len = %d\n",len(fd));

    cp(fd,2);
}

// calculate this section of the factorial
int fact(int start, int end) {
    int result = 1;
    for (int i = start; i <= end; i++) {
        result *= i;
    }
    return result;
}

void calculate_factorial(int n, int* shared_result, int sem_id) {
    // lets calculate a factorial but divide up the work among 4 processes to do this in parallel.
    // tests semaphores and fork.
    uint32_t num_processes = 4;
    uint32_t chunk_size = n / num_processes;
    uint32_t ids[num_processes];

    *shared_result = 1;

    for (uint32_t i = 0; i < num_processes; i++) {
        uint32_t start = i * chunk_size + 1; // figure out where to start calculating from
        uint32_t end = (i == num_processes - 1) ? n : start + chunk_size - 1;

        ids[i] = fork();
        if (ids[i] == 0) { // is the child
            uint32_t partial_result = fact(start, end);
            down(sem_id);
            *shared_result *= partial_result;
            up(sem_id);
            exit(0);
        }
    }

    for (uint32_t i = 0; i < num_processes; i++) {
        // wait for everyone to finish
        uint32_t status = 42;
        wait(ids[i], &status);
    }
}

int main(int argc, char** argv) {
    printf("*** Welcome to test case 15.\n");

    // open a file
    int file1 = open("data/data.txt", 0);
    if (file1 != 3) {
        printf("*** Lowest descriptor not given to new file.\n");
    }

    
    int sem1 = sem(1);

    close(file1);

    // open another file
    int file2 = open("data/panic.txt", 0);
    if (file2 != 3) {
        printf("*** Lowest descriptor not given to a new file.\n");
    }

    printf("*** Simple tests done.\n");

    uint32_t* factorial = (uint32_t*) naive_mmap(sizeof(uint32_t), 1, 1000, 0);
    *factorial = 1;
    
    calculate_factorial(10, (int*)factorial, sem1);
    printf("*** Calculated factorial is %ld\n", *factorial);
    
    shutdown();
    return 0;
}
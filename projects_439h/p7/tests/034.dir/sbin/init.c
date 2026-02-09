#include "libc.h"

void test_file_operations() {
    int fd = open("/etc/data.txt", 0);
    if (fd < 0) {
        printf("Failed to open file\n");
    } else {
        char buffer[100];
        int bytes_read = read(fd, buffer, sizeof(buffer));
        printf("*** bytes read: %d\n", bytes_read);

        // Can only write to stdout (fd = 1) and stderr (fd = 2)
        ssize_t bytes_written_stdout = write(1, buffer, bytes_read);
        printf("*** bytes written to stdout: %d\n", bytes_written_stdout);

        int offset = seek(fd, 17);
        printf("*** seek offset: %d\n", offset);

        ssize_t bytes_written_stderr = write(2, buffer, bytes_read);
        printf("*** bytes written to stderr: %d\n", bytes_written_stderr);

        ssize_t bytes_not_written_stdinn = write(0, buffer, bytes_read);
        printf("*** bytes not written to stdinn: %d\n", bytes_not_written_stdinn);

        ssize_t bytes_not_written_fd = write(fd, buffer, bytes_read);
        printf("*** bytes not written to data.txt: %d\n", bytes_not_written_fd);

        int close_status = close(fd);
        printf("*** close status: %d\n", close_status);
    }
}

void test_process_management() {
    int pid = fork();
    if (pid == 0) {
        printf("*** in child process\n");
        int exec_status = execl("/sbin/shell", "shell", 0);
        printf("*** exec failed, status: %d\n", exec_status);
        exit(1);
    } else if (pid > 0) {
        uint32_t status;
        wait(pid, &status);
    } else {
        printf("*** fork failed\n");
    }
}

void test_semaphore_operations() {
    int sem_id = sem(1);
    printf("*** semaphore id: %d\n", sem_id);
    if (sem_id < 0) {
        printf("*** semaphore creation failed\n");
    } else {
        if (down(sem_id) == 0) {
            printf("*** semaphore down succeeded\n");
        }

        if (up(sem_id) == 0) {
            printf("*** semaphore up succeeded\n");
        }

        int status = close(sem_id);
        printf("*** semaphore close status: %d\n", status);

        int status2 = close(sem_id);
        printf("*** invalid semaphore close status: %d\n", status2);
    }
}

int main(int argc, char** argv) {
    printf("*** starting tests...\n");

    test_file_operations();
    test_semaphore_operations();
    test_process_management();
    
    printf("*** all tests completed.\n");
    shutdown();
    return 0;
}

#include "libc.h"


// helper function to compare strings
int compare_strings(const char* str1, const char* str2, int length) {
    for (int i = 0; i < length; i++) {
        if (str1[i] != str2[i]) {
            return 0; // strings do not match
        }
    }
    return 1; // strings match
}

int main() {
    // test data
    char* data = "*** stdout test!";
    char* err_data = "*** stderr test!";
    int data_len = 16;
    
    // 1. basic write to stdout (file descriptor 1)
    int stdout_written = write(1, data, data_len);
    if (stdout_written > data_len) {
        printf("*** error: expected to write %d bytes to stdout, but wrote %d\n", data_len, stdout_written);
        shutdown();
        return -1;
    }

    // 2. basic write to stderr (file descriptor 2)
    int stderr_written = write(2, err_data, data_len);
    if (stderr_written > data_len) {
        printf("*** error: expected to write %d bytes to stderr, but wrote %d\n", data_len, stderr_written);
        shutdown();
        return -1;
    }

    // 3. writing zero bytes to stdout
    int zero_write = write(1, data, 0);
    if (zero_write != 0) {
        printf("*** error: expected to write 0 bytes to stdout, but wrote %d\n", zero_write);
        shutdown();
        return -1;
    }

    // 4. writing partial data to stdout
    int partial_write = write(1, data, 5); // should write the first 5 bytes
    if (partial_write > 5) {
        printf("*** error: expected to write 5 bytes to stdout, but wrote %d\n", partial_write);
        shutdown();
        return -1;
    }

    printf("*** stdout and stderr write tests passed\n");

    // read test
    // open a file for reading
    int fd = open("/etc/test.txt", 0);
    
    // check if the file opened successfully
    if (fd < 0) {
        printf("*** error: couldn't open file\n");
        return -1;
    }

    // prepare a buffer and read from the file ("can you read this?")
    char buffer[50];
    int bytes_read = read(fd, buffer, 50);
    if (bytes_read > 50) {
        printf("*** error: didn't read correctly\n");
        return -1;
    }
    buffer[bytes_read] = '\0';

    // print the read data
    printf("*** data read from file: %s\n", buffer);

    
    close(fd);

    shutdown();
    return 0;
}

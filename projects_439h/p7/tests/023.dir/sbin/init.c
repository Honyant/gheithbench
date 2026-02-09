#include "libc.h"

void check_file(int fd) {
    printf("*** fd = %d\n", fd);
    printf("*** len = %d\n", len(fd));
}

void print_file(int fd, int size, int read_sz) {
    int i = 0;

    char buffer [size];
    while(i < size) {
        int num_read = read(fd, buffer + i, read_sz < size - i ? read_sz : size - i);
        if(num_read < 0) {
            printf("*** error reading from fd = %d\n", fd);
            return;
        }
        i += num_read;
    }
    
    i = 0;
    while(i < size) {
        int num_written = write(1, buffer, size - i);
        if(num_written < 0) {
            printf("*** error writing to fd = %d\n", fd);
            return;
        }
        i += num_written;
    }
}

int main(int argc, char** argv) {
    int file1 = open("data/whale.txt", 0);
    check_file(file1);
    print_file(file1, len(file1), 5);

    // seek and read
    int offset = (int)seek(file1, 109);
    print_file(file1, len(file1) - offset, 5);

    // seek and read at invalid offset
    seek(file1, 0);
    offset = (int)seek(file1, 500);
    printf("*** seek %d\n", offset);
    print_file(file1, 10, 5);

    print_file(1, 5, 5);

    // close fd 2 (stderr) and open another file
    printf("*** close %d\n", close(2));
    int file2 = open("data/dolphin.txt", 0);
    check_file(file2);
    printf("*** write %d\n", write(2, "*** shouldn't print this\n", 25));
    
    int id = fork();

    if (id < 0) printf("*** fork failed");
    else if (id == 0) {
        // child
        print_file(file2, 143, 143);
        int file3 = open("data/whale.txt", 0);
        if(file3 != 4) printf("*** file 3 at incorrect fd = %d\n", file3);
        close(file2);
        exit(1);
    } 
    else {
        // parent
        uint32_t status = 42;
        wait(id,&status);

        if(id < 100) printf("*** id of child = %d, should be greater than 100\n", id);
        if(status != 1) printf("*** incorrect status = %d\n", (int)status);
        print_file(file2, 82, 82);
        print_file(4, 5, 5);
    }

    shutdown();
    return 0;
}

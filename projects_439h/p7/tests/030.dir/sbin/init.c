#include "libc.h"

/* this is a comprehensive test of functionality and edge cases related to files, file descriptors, reading, writing, etc.
* you don't need fork, wait, or execl for this test case!
* hopefully this helps you implement all the random validity checks needed for the file syscalls :) */
void fileTests() {
    //basic open
    printf("*** various open, read, seek tests\n");
    int fd = open("data/../data/data.txt",0);
    printf("*** fd = %d\n",fd);

    //open same file again
    int fd2 = open("data/../data/data.txt",0);
    printf("*** fd = %d\n",fd2);

    //open nonexistent file, should return -1
    int fd3 = open("/data/doesnotexist.txt",0);
    printf("*** fd = %d\n",fd3);

    //basic read
    char buf[200];
    int bytesRead = read(fd, buf, 41);
    printf("*** total bytes read = %d\n",bytesRead);

    //read should continue from offset saved after last read
    bytesRead += read(fd, buf+bytesRead, 38);
    printf("*** total bytes read = %d\n",bytesRead);
    buf[bytesRead] = 0;
    printf("%s\n",buf); //print out what we read
    printf("*** \n");

    //read more bytes than file size, should return less than 100
    bytesRead += read(fd, buf+bytesRead, 100);
    printf("*** total bytes read = %d\n",bytesRead);
    buf[bytesRead] = 0;
    printf("%s\n",buf); 
    printf("*** \n");
    
    //seek to middle of file, then read from there
    char buf2[100];
    seek(fd, 41);
    bytesRead = read(fd, buf2, 38);
    buf2[bytesRead] = 0;
    printf("%s\n",buf2); 
    printf("*** \n");

    //invalid seek in console devices
    printf("*** invalid seek tests\n");
    printf("*** %ld\n", seek(0, 5)); //should return -1
    printf("*** %ld\n", seek(1, 2)); //should return -1
    printf("*** %ld\n", seek(2, 0)); //should return -1
    printf("*** \n");

    //read from the same file but different fd, should start from beginning
    char buf3[100];
    bytesRead = read(fd2, buf3, 100);
    buf3[bytesRead] = 0;
    printf("%s\n",buf3);
    printf("*** \n");

    //read from invalid fds, should all return -1
    printf("*** invalid fd read tests\n");
    printf("*** %d\n", read(10, buf, 100)); //fd not opened yet
    printf("*** %d\n", read(-1, buf, 100)); //fd out of range
    printf("*** %d\n", read(200, buf, 100)); //fd out of range
    printf("*** %d\n", read(0, buf, 100)); //stdin
    printf("*** %d\n", read(1, buf, 100)); //stdout
    printf("*** %d\n", read(2, buf, 100)); //stderr
    printf("*** \n");

    //read into invalid buffer address (0x0 and where ioapic and localapic are), should all return -1
    printf("*** invalid buffer address tests\n");
    printf("*** %d\n", read(fd, (char*)0x0, 100));
    printf("*** %d\n", read(fd, (char*)0xfec00001, 100));
    printf("*** %d\n", read(fd, (char*)0xfee00001, 100));
    printf("*** \n");

    //basic close
    printf("*** close tests\n");
    printf("*** %d\n", close(fd)); //return 0 on success

    //invalid closes
    printf("*** %d\n", read(fd, buf, 100)); //try to read from closed fd (-1)
    printf("*** %ld\n", seek(fd, 0)); //try to seek in closed fd (-1)
    printf("*** %d\n", len(fd)); //try to get len of closed fd (-1)
    printf("*** %d\n", close(fd)); //close already closed fd (-1)
    printf("*** %d\n", open("/data/other.txt",0)); //should use first free fd (3)
    printf("*** \n");

    //close stderr (fd=2), then read and seek in a file at fd 2
    printf("*** close stderr tests\n");
    printf("*** %d\n", close(2)); //return 0 on success
    printf("*** fd = %d\n", open("/data/other.txt",0)); //should use first free fd (2)
    printf("*** total bytes read = %d\n", read(2, buf, 100)); //should read normally from other.txt
    printf("*** %ld\n", seek(2, 0)); //seek to beginning of file, should return 0 on success
    printf("*** total bytes read = %d\n", read(2, buf, 100)); //should read normally from beginning of other.txt
    printf("*** len=%d\n", len(2)); //should return length of other.txt
    printf("*** \n");

    //basic chdir on relative path
    printf("*** chdir tests\n");
    printf("*** %d\n", chdir("data_v2")); //should return 0 on success
    char buf4[100];
    int fd5 = open("data.txt",0); //should open data.txt from /data_v2
    bytesRead = read(fd5, buf4, 100);
    printf("*** bytes read = %d\n", bytesRead);
    buf4[bytesRead] = 0;
    printf("%s\n",buf4); //print out what we read
    printf("*** %d\n", chdir("/")); //absolute chdir to root (0)

    //invalid chdir
    printf("*** %d\n", chdir("doesnotexist")); //chdir to nonexistent folder (-1)
    /*the test below is important! if you change cwd to a file it'll mess up future opens*/
    printf("*** %d\n", chdir("data/data.txt")); //chdir to file (-1) 
    printf("*** \n");

    //basic write to stdout
    printf("*** write tests\n");
    int bytesWritten = write(1, "*** hello world\n", 16); //should print hello world to stdout
    printf("*** bytesWritten = %d\n", bytesWritten);
    bytesWritten = write(1, "*** hello world\n", 9); //should print hello to stdout
    printf("\n*** bytesWritten = %d\n", bytesWritten);
    printf("*** \n");

    //writes to invalid fds
    printf("*** invalid write tests\n");
    printf("*** %d\n", write(10, "hello world\n", 12)); //fd not opened yet (-1)
    printf("*** %d\n", write(200, "hello world\n", 12)); //fd out of range (-1)
    printf("*** %d\n", write(fd5, "hello world\n", 12)); //writing to a file (-1)
    printf("*** %d\n", write(0, "hello world\n", 12)); //stdin (-1)
    printf("*** %d\n", write(2, "hello world\n", 12)); //stderr was previously closed, so this should be a write to file (-1)

    //writes from invalid buffers
    printf("*** %d\n", write(1, (char*)0x0, 12)); //writing from invalid buffer address (-1)
    printf("*** %d\n", write(1, (char*)0xfec00001, 12)); //writing from invalid buffer address (iopaic) (-1)
    printf("*** %d\n", write(1, (char*)0xfee00001, 12)); //writing from invalid buffer address (localapic) (-1)
    printf("*** \n");

    //open 94 files, should return 6-99 because 0-5 is already used
    printf("*** opening many files test\n");
    for (int i = 0; i < 94; i++) {
        int fd = open("/data/data.txt",0);
        if (fd != i+6) {
            printf("*** wrong fd returned, we got %d and expected %d\n", fd, i+5);
            return;
        }
    }
    //open another file, should return -1 because we ran out of file descriptors
    int fd6 = open("/data/data.txt",0);
    printf("*** fd = %d\n",fd6);
}

int main(int argc, char** argv) {
    printf("*** %d\n",argc);
    for (int i=0; i<argc; i++) {
        printf("*** %s\n",argv[i]);
    }

    fileTests();
    shutdown();
    return 0;
}

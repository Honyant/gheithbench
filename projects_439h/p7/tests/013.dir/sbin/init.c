#include "libc.h"

/* This is a comprehensive test on syscalls related to the file system and includes
checks for various error cases. */

char* invalid(int value) {
    return value < 0 ? "error" : "unknown";
}

int main(int argc, char** argv) {
    /***** TEST 1: open *****/
    printf("*** TEST 1\n");

    // file
    int file = open("file.txt", 0);
    printf("*** open(\"file.txt\") = %d\n", file);

    // nested file
    int nested_file = open("directory/nested_file.txt", 0);
    printf("*** open(\"nested_file.txt\") = %d\n", nested_file);

    // invalid
    printf("*** open(\"nonexistent\") = %s\n", invalid(open("nonexistent", 0)));

    printf("***\n");


    /***** TEST 2: len *****/
    printf("*** TEST 2\n");

    // file
    ssize_t file_size = len(file);
    printf("*** len(%d) = %d\n", file, file_size);

    // nested file
    ssize_t nested_file_size = len(nested_file);
    printf("*** len(%d) = %d\n", nested_file, nested_file_size);

    // invalid
    printf("*** len(stdin) = %s\n", invalid(len(0)));
    printf("*** len(stdout) = %s\n", invalid(len(1)));
    printf("*** len(stderr) = %s\n", invalid(len(2)));
    printf("*** len(unopened) = %s\n", invalid(len(5)));
    printf("*** len(out_of_range) = %s\n", invalid(len(100)));
    printf("*** len(negative) = %s\n", invalid(len(-1)));

    printf("***\n");


    /***** TEST 3: read *****/
    printf("*** TEST 3\n");

    // file
    char file_contents[file_size];
    ssize_t nbytes = read(file, file_contents, file_size);
    printf("*** read(%d) = %d\n", file, nbytes);

    // nested_file
    char nested_file_contents[nested_file_size];
    nbytes = read(nested_file, nested_file_contents, nested_file_size);
    printf("*** read(%d) = %d\n", nested_file, nbytes);

    // invalid
    char buffer[1];
    printf("*** read(stdin) = %s\n", invalid(read(0, buffer, 1)));
    printf("*** read(stdout) = %s\n", invalid(read(1, buffer, 1)));
    printf("*** read(stderr) = %s\n", invalid(read(2, buffer, 1)));
    printf("*** read(unopened) = %s\n", invalid(read(5, buffer, 1)));
    printf("*** read(out_of_range) = %s\n", invalid(read(100, buffer, 1)));
    printf("*** read(negative) = %s\n", invalid(read(-1, buffer, 1)));

    printf("***\n");

    
    /***** TEST 4: write *****/
    printf("*** TEST 4\n");

    // stdout
    printf("*** write file.txt to stdout: ");
    nbytes = write(1, file_contents, file_size);
    printf("*** write(1) = %d\n", nbytes);

    // stderr
    printf("*** write nested_file.txt to stderr: ");
    nbytes = write(2, nested_file_contents, nested_file_size); 
    printf("*** write(2) = %d\n", nbytes);

    // write with size limit
    printf("*** write 4 bytes of file.txt: ");
    nbytes = write(1, file_contents, 4);
    write(1, "\n", 1);
    printf("*** write(1) = %d\n", nbytes);

    // invalid
    printf("*** write(stdin) = %s\n", invalid(write(0, buffer, 1)));
    printf("*** write(3) = %s\n", invalid(write(3, buffer, 1)));
    printf("*** write(4) = %s\n", invalid(write(4, buffer, 1)));
    printf("*** write(unopened) = %s\n", invalid(write(5, buffer, 1)));
    printf("*** write(out_of_range) = %s\n", invalid(write(100, buffer, 1)));
    printf("*** write(negative) = %s\n", invalid(write(-1, buffer, 1)));

    printf("***\n");


    /***** TEST 5: seek *****/
    printf("*** TEST 5\n");

    // seek to beginning
    printf("*** seek(%d, 0) = %ld\n", file, seek(file, 0));
    memset(file_contents, 0, sizeof(file_contents));
    read(file, file_contents, 5);
    printf("*** read file.txt: ");
    write(1, file_contents, 5);
    printf("\n*** read file.txt: ");
    memset(file_contents, 0, sizeof(file_contents));
    read(file, file_contents, file_size - 5);
    write(1, file_contents, file_size - 5);

    // seek to middle
    printf("*** seek(%d, 5) = %ld\n", file, seek(file, 5));
    memset(file_contents, 0, sizeof(file_contents));
    read(file, file_contents, file_size - 5);
    printf("*** read file.txt: ");
    write(1, file_contents, file_size - 5);

    // seek past end of file
    printf("*** seek(%d, 15) = %ld\n", file, seek(file, 15));
    
    // invalid
    printf("*** seek(stdin) = %s\n", invalid(seek(0, 0)));
    printf("*** seek(stdout) = %s\n", invalid(seek(1, 0)));
    printf("*** seek(stderr) = %s\n", invalid(seek(2, 0)));
    printf("*** seek(unopened) = %s\n", invalid(seek(5, 0)));
    printf("*** seek(out_of_range) = %s\n", invalid(seek(100, 0)));
    printf("*** seek(negative) = %s\n", invalid(seek(-1, 0)));

    printf("***\n");


    /***** TEST 6: close *****/
    printf("*** TEST 6\n");

    // close and reopen file
    printf("*** close(%d) = %d\n", file, close(file));
    file = open("file.txt", 0);
    printf("*** open(\"file.txt\") = %d\n", file);

    // close stdin
    printf("*** close(stdin) = %d\n", close(0));

    // close and reopen nested file
    printf("*** close(%d) = %d\n", nested_file, close(nested_file));
    nested_file = open("directory/nested_file.txt", 0);
    printf("*** open(\"nested_file.txt\") = %d\n", nested_file);

    // invalid
    printf("*** close(unopened) = %s\n", invalid(close(5)));
    printf("*** close(out_of_range) = %s\n", invalid(close(100)));
    printf("*** close(negative) = %s\n", invalid(close(-1)));

    // close stdout and stderr
    close(1);
    close(2);
    printf("*** should never see this\n");

    shutdown();
    
    return 0;
}

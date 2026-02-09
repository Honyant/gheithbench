#include "libc.h"

//checking syscall validation behavior
int main(int argc, char** argv) {
    //**** write ****
    char * hello = "hello there\n";
    int bitsWritten = 0;
    bitsWritten += write(0, hello, 1); // -1
    bitsWritten += write(0, hello, -1); // -1
    bitsWritten += write(1, hello, 6); // 6
    bitsWritten += write(2, hello + 6, 6); //6

    // challenge -> per linux spec -1 should be returned for buffer that is outside accessible user space.
    bitsWritten += write(2, (char*) 0x7fffffff, 1); //-1
    bitsWritten += write(2, (char*) 0x0, 6); //-1
    bitsWritten += write(2, (char*) 0xfec00000 + 1, 6); //-1 (kConfig.ioAPIC)

    printf("*** Write check: %s\n", bitsWritten == 7 ? "passed": "failed");

    //**** len ****

    int count = 0;
    count += len(2); //-1 (console device)
    count += len(5); //-1 (no file here)
    int sem_id = sem(1);
    count += len(sem_id); // -1 (not a file)

    printf("*** Len test: %s\n", count == -3 ? "passed": "failed");
    //printf("*** Expected -3, got %d\n", count);

    //**** read ****
    count = 0;
    count += read(sem_id, (char*) 0x80000000, 2); // -1 (not a file)    
    count += read(0, (char*) 0x7fffffff, 1); //-1
    count += read(0, (char*) 0x0, 1); //-1
    count += read(0, (char*) 0xfec00000 + 1, 1); //-1 (kConfig.ioAPIC)

    printf("*** Read test: %s\n", count == -4 ? "passed": "failed");
    //printf("*** Expected -4, got %d\n", count);

    //**** seek ****
    count = 0;
    count += seek(1, 0); // -1 invalid fd (no file exists)
    count += seek(2, 0); // < 0 invalid fd (console device)
    open("data/data.txt", 0);
    count += seek(3, 10); // 10
    count += seek(3, 500); // 500

    printf("*** Seek test: %s\n", count < 509 ? "passed": "failed");

    //**** chdir ****
    count = 0;
    count += chdir("sbin/sdtint.h"); // -1 (not a directory)
    count += chdir("sbin/init"); //-1 (not a directory)
    count += chdir("sbin"); //0
    count += chdir("../data"); //0
    count += chdir(".."); //0
    printf("*** Chdir test: %s\n", count == -2 ? "passed": "failed");

    //**** naive_unmap & naive_mmap ****
    
    naive_unmap(0);
    char* addressThree  = (char*) naive_mmap(1, 0, 5, 0); //anonymous mmap
    printf("*** Mmapping test: %s\n", *addressThree == 0 ? "passed": "failed");

    //**** wait ****
    count = 0;
    uint32_t status = 0;
    count += wait(0, &status); //-1
    count += wait(100, &status); //-1
    count += wait(200, &status); //-1
    count += wait(300, &status); //-1
    wait(300, (uint32_t*) 0); //Should not dereference nullptr here to fill the status code
    printf("*** Wait test: %s\n", count == -4 ? "passed": "failed");

    close(sem_id);
    shutdown();
    return 0;
}


#include "lib/libc.h"

// I need this apparently >:(
#define bool int
#define true 1
#define false 0

static int normalize(int result) {
    return result < 0 ? -1 : 1;
}

static void testOverflowFlagFork(bool overflow);
static void testOverflowFlagExecl(bool overflow);



// Alright, prepare for a boatload of edge cases

int main(int argc, char** argv) {
    printf("*** Begin!\n");

    int id, fd, s, result;
    char c = 0;
    uint32_t code;

    // Close fd 0 and open a file in its place
    printf("*** Replacing cin with /data/fruit.txt:\n");
    close(0);
    fd = open("/data/fruit.txt", 0);
    printf("***     Opened fruit.txt at fd %d\n", fd);

    printf("***     Reading (then closing) fruit.txt in separate processes:\n");
    s = sem(0);
    for (int i = 0; i < 5; i++) { // Fork 5 times
        id = fork();

        if (id == 0) {
            // Print out one line of the file (read 1 char then write it until we get a newline)
            printf("***         ");
            while (c != '\n') {
                result = read(0, &c, 1);
                if (result != 1) { printf("***!*** read returned %d\n", result); }

                result = write(1, &c, 1);
                if (result != 1) { printf("***!*** write returned %d\n", result); }
            }

            // Close the file and make sure we can't read from it anymore
            // It should still be open on the other processes
            close(0);
            result = read(0, &c, 1);
            if (result != -1) { printf("***!*** read worked after closing file\n"); }
            close(1); // Close stdout for good measure

            up(s);
            exit(123);
        }

        down(s); // Wait for the child to finish before forking again
    }

    printf("***     Wait with invalid pointer: ");
    printf("%d\n", normalize(wait(id, (uint32_t*) 0))); // We shouldnt be able to pass a nullptr to wait

    wait(id, &code); // Do a real wait this time
    printf("***     Last child exited with code %ld\n", code);


    // Open a new file at fd 0!
    close(0);
    fd = open("/data/concurrency.txt", 0);
    printf("*** Opened concurrency.txt at fd %d\n", fd);

    // Seeking past the end of a file is valid, but we shouldn't be able to read it
    printf("***     Seeking to 100: ");
    result = seek(fd, 100); // Should succeed
    printf("%d\n", result);

    printf("***         Reading: ");
    result = normalize(read(fd, &c, 1)); // Should fail
    printf("%d\n", result);

    printf("***     Seeking back to 0: ");
    result = seek(fd, 0); // Should succeed
    printf("%d\n", result);


    // Test forking and closing a file at the same time in different processes
    printf("***     Closing it concurrently on several processes\n");
    for (int i = 0; i < 20; i++) {
        id = fork(); // Increment the file descriptor's reference count
        if (id == 0) {
            close(0); // Decrement the file descriptor's reference count
            exit(0);
        }
    }
    wait(id, &code); // Wait again (not really necessary)
    if (code != 0) { printf("***!*** Last child exited with code %ld\n", code); }


    printf("***     MMapping it\n");

    int l = len(fd); // Get the file length
    if (l < 0) { printf("***!*** len returned %d\n", l); }

    char* addr = (char*) naive_mmap(l, 0, fd, 0); // mmap a file!
    if (addr == 0) { printf("***!*** mmap failed\n"); }

    printf("***         Reading file: %s", addr); // print from memory

    naive_unmap(addr); // unmap it again

    printf("***     Reading after unmap: ");
    result = normalize(read(fd, addr, 1)); // should not be able to read anymore
    printf("%d\n", result);


    // Here comes a barrage of edge cases!

    printf("*** Accessing nonexistent file: ");
    fd = normalize(open("/fake/news.txt", 0)); // Search a missing dir
    printf("%d\n", fd);

    printf("*** Chdir-ing to nonexistent directory: ");
    fd = normalize(chdir("/fake/news"));  // Go to a missing dir
    printf("%d\n", fd);

    printf("*** Execl-ing non-executable (/data/fruit.txt): ");
    result = normalize(execl("/data/fruit.txt", 0)); // execl() a non-ELF file
    printf("%d\n", result);

    printf("*** Execl-ing executable with bad memory address (/sbin/invalid): ");
    result = normalize(execl("/sbin/invalid", 0)); // execl() an ELF file that wants to live at address 0x10000000 (which is kernel space)
    printf("%d\n", result);

    printf("*** Execl-ing with bad string address (/sbin/flags): ");
    result = normalize(execl("/sbin/flags", (const char*) 0x10000000, 0)); // execl() with argv[0] = 0x10000000 (which is kernel space)
    printf("%d\n", result);


    // Test that unresolvable page faults in user mode call exit(-1)

    id = fork();
    if (id == 0) {
        printf("*** Pagefaulting at kernel address\n");
        *(int*)0x10000 = 0;
        printf("***!*** Shouldn't get here!");
    }
    wait(id, &code);
    printf("***     Child exited with code %ld\n", code);

    id = fork();
    if (id == 0) {
        printf("*** Pagefaulting at unmapped shared address\n");
        *(int*)0xF0000000 = 0;
        printf("***!*** Shouldn't get here!");
    }
    wait(id, &code);
    printf("***     Child exited with code %ld\n", code);


    // This test loosk real complicated, but all it checks is that when you call fork()
    // you save the user's flags register and restore it in the new process.

    // Instead of simply calling switchToUser(), consider copying the 5 important stack values yourself
    // so that they're actually saved. Or you could just make it take the flags value as a parameter...

    // I don't think I can actually test that the CS and SS values are saved :(

    testOverflowFlagFork(true);
    testOverflowFlagFork(false);

    // Apparently execl is allowed to clobber the flags register...
//    testOverflowFlagExecl(true);
//    testOverflowFlagExecl(false);


    // I wanna test that people can handle protection faults but I don't think that's valid :(
    // It actually isn't as hard as you might think to deal with (just register a handler that calls exit(-1))

//    id = fork();
//    if (id == 0) {
//        printf("*** Executing invalid instruction: will cause a general protection fault (code 13)\n");
//        __asm__ volatile("mov %eax, %cr3");
//        exit(0);
//    }
//
//    wait(id, &code);
//    printf("*** Child exited\n");


    printf("*** Done with test case!\n");
    shutdown();
    return 0;
}

static void testOverflowFlagFork(bool overflow) {
    int id;
    uint32_t flags;

    printf("*** Forking with overflow flag %s\n", overflow ? "SET" : "UNSET");
    __asm__ volatile(
        "mov %2, %%ebx\n"    // Set %ebx to a big value
        "add %%ebx, %%ebx\n" // Add %ebx to itself to cause an overflow

        "call fork\n"        // Call fork()
        "mov %%eax, %0\n"    // Store the return value in the id variable

        "pushf\n"
        "pop %1\n"           // Transfer the flags register into the flags variable

        : "=r" (id), "=r" (flags)
        : "r" (overflow ? 0xF0000000 : 0x10000000) // If overflow is true, make %ebx a big number, otherwise make it a smaller number
        : "ebx"
    );

    if (id == 0) {
        bool oFlag = flags & 0x1; // Is the overflow flag set?
        printf("***     In forked child: overflow flag is %s\n", oFlag ? "SET" : "UNSET");
        exit(321);
    }

    uint32_t code;
    wait(id, &code); // Wait for it to finish
    printf("***     Child exited with code %ld\n", code);
}

//static void testOverflowFlagExecl(bool overflow) {
//    printf("*** Execl-ing /sbin/flags with overflow flag %s\n", overflow ? "SET" : "UNSET");
//    int id = fork();
//
//    if (id == 0) {
//        const char* path = "/sbin/flags"; // flags.c has code that prints the flags register
//        int result;
//        __asm__ volatile(
//            "mov %1, %%ebx\n"    // Set %ebx to a big value
//            "add %%ebx, %%ebx\n" // Add %ebx to itself to cause an overflow
//
//            "push $0\n"          // Push a 0 for argv
//            "push %2\n"          // Push the path string pointer
//            "call execl\n"       // Call execl()
//            "mov %%eax, %0\n"    // Store the return value the result variable
//
//            : "=r" (result)
//            : "r" (overflow ? 0xF0000000 : 0x10000000), "r" (path) // If overflow is true, make %ebx a big number, otherwise make it a smaller number
//            : "ebx"
//        );
//        printf("***!*** Execl returned %d\n", result); // If execl returned, print its return value
//        exit(-1);
//    }
//
//    uint32_t code;
//    wait(id, &code); // Wait for it to finish
//    printf("***     Child exited with code %ld\n", code);
//}

#include "libc.h"

/*
This tc pokes a bit into edge cases that exist for open, read and len. These should be simple to fix 
and are a prerequisite for other parts of my tests.

The meat of this test is testing that you handle chdir and execl pathing correctly. If not, this
could result in you executing the incorrect program, and while I don't do anything malicious, someone
else very well could. In all, you will need

- shutdown (to end the program)
- read (to read files)
- write (to print)
- open (to put files in our fd table)
- close (to remove processes and files from our fd table)
- mmap (to get space for our string buffers)
- execl (to get to the correct or incorrect processes)
- fork (to spawn children that will go to these other processes)
- sem, up, down (to synchronize between parents and their children in their separate processes so no race conditions)
- exit (so we can terminate the children correctly)
- chdir (so we can change which files we are loading from)

to pass this testcase. I don't test any super weird or edge case behaviors in these syscalls, this more just validates
a mostly working implementation with an emphasis on chdir/execl.
*/

void one(int fd) {
    printf("*** fd = %d\n",fd);
    printf("*** len = %d\n",len(fd));

    cp(fd,2);
}

//helpful utility so we can pass fd #s into exec
char* IntToString(int num){
    int copy = num;
    int len = 0;
    while(copy > 0){
        copy /= 10;
        len++;
    }
    char* buffer = (char*)naive_mmap(len, 0, 0, 0); //mmap region for our buffer
    for(int i = len-1; i >= 0; i--){
        buffer[i] = (char)(num % 10 + 48);
        num /= 10;
    }
    buffer[len] = '\0';
    return buffer;
}


int main(int argc, char** argv) {
    /*
    Open, Read, Len Test
    */
    printf("Begin open, read and len test \n");

    int fd = open("/bop/data.txt",0); //can you read normally?
    if(fd != 3){
        printf("*** rip wrong fd returned, we got %d and expected 3", fd);
        shutdown();
    }
    one(fd); //let's do the normal behavior for len
    cp(fd,2); //now let's read it

    //what if the file doesn't exist?
    int fd2 = open("yothisfiledoesnotexistyo", 0);
    if(fd2 >= 0){
        printf("*** rip wrong fd returned, we got %d and expected < 0", fd2);
        shutdown();
    }
    printf("*** len = %d\n",len(fd)); //what happens here? you should't panic but rather fail gracefully

    //let's get another fd to make sure your behavior is consistent
    int fd3 = open("/bop/data2.txt",0);
    if(fd3 != 4){
        printf("*** rip wrong fd returned, we got %d and expected 4", fd3);
        shutdown();
    }
    one(fd3); //let's do the normal behavior for len
    cp(fd3,2); //now let's read it

    //let's now test basic edge case handling, we can't assert anything about these returns just that they don't panic or die
    cp(100, 2); //this shouldn't be able to read
    cp(-1, 2); //did you check the other way?
    len(100);
    len(-1);
    open("\0", 0); //hm?

    /*
    let's add close now
    */
    printf("*** Open, read and len basic test finished!\n");
    printf("*** __________________________________\n");
    printf("*** Spicing it up with close \n");

    //basic behavior?
    close(fd);
    one(fd); //you should know what this does

    int status = close(300); //can this work?
    if(status >= 0){
        printf("*** rip wrong status returned, we got %d and expected < 0", status);
        shutdown();
    }
    //what about an fd that doesn't exist?

    status = close(6);
    if(status >= 0){
        printf("*** rip wrong status returned, we got %d and expected < 0", status);
        shutdown();
    }

    printf("*** Basic close test finished! \n");
    printf("*** __________________________________ \n");
    printf("*** Let's do something more interesting now\n");

    chdir("/sbin/"); //move us inside of sbin

    int sem1 = sem(0);
    int sem2 = sem(0);

    char* sem1arg = IntToString(sem1);
    char* sem2arg = IntToString(sem2);

    int proc1 = fork(); //lets test an absolute chdir

    if(proc1 == 0){
        chdir("/bop/"); //don't make this relative, this should be the bop at the top of the directory
        execl("fork1", "*** you", "read", "the", "right", "file", "fork1!\n", sem1arg, 0);
        printf("*** rip exec failed \n");
    }
    else if(proc1 > 0){
        uint32_t status = 100;
        wait(proc1,&status);
        if(status == 100 || (int)status < 0){
            printf("*** absolute chdir test failed with status %d\n", (int)status);
            shutdown();
        }
        printf("*** absolute chdir test passed! \n");

        down(sem1);
    }

    

    chdir("/sbin/"); //move us inside of sbin

    int proc2 = fork(); //let's test a relative chdir

    if(proc2 == 0){
        chdir("bop"); //make this relative, this should be the bop in sbin
        execl("fork2", "*** you", "read", "the", "right", "file", "fork2!\n", sem2arg, 0);
        printf("*** rip exec failed \n");
    }
    else if(proc2 > 0){
        uint32_t status = 100;
        wait(proc2,&status);
        if(status == 100 || (int)status < 0){
            printf("*** relative chdir test failed with status %d\n", (int)status);
            shutdown();
        }
        printf("*** relative chdir test passed! \n");

        down(sem2);
    }


    close(1); //hm, what does this do just for fun?
    printf("*** we shouldn't see this :( \n");

    shutdown();
    return 0;
}

#include "libc.h"

/**
 * This tests that you verify your ELF file before running!
 *
 * Why is this important? -- Cool info below!
 *  The OS must make sure it is able to run an executable.
 *  Otherwise, bad things can happen!
 *  What do we do when loading an ELF file? We perform writes.
 *  If we can construct a bad ELF, this means we can perform
 *  arbitrary writes to a system. If you can perform arbitrary writes,
 *  you essentially gain arbitrary code execution! (scary ahhh)
 *
 * How was this test built?
 *  I had to do some special compilation with different compilers to 
 *  get the intended effect. I also used xelfviewer to inspect the ELF, 
 *  which is nice for debugging!
 */

int main(int argc, char** argv) {
    /**
     * NOTE: We will NOT stop if an individual test fails,
     * but we could by checknig if r=-1. Your excel should return -1
     * if the ELF fails to load. Check the diff for what tests you're failing, 
     * the name of the test will be printed!
     */
  
    /**
     * ARE YOU FAILING? Useful resource! https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.eheader.html
     */

    printf("*** Let's see if you can handle bad elfs!\n");
    
    /**
     * Not an ELF file! You need to check the magic number
     */
    execl("/sbin/not_elf","not_elf",0);

    /**
     * You should only allow 32_bit mode
     */
    execl("/sbin/64_bit_mode","64_bit_mode",0);

    /**
     * You should only allow little endian
     */
    execl("/sbin/not_little_endian","not_little_endian",0);
    
    /**
     * Invalid start address, this should start in private memory 
     */
    execl("/sbin/invalid_start_address","invalid_start_address",0);
    
    /**
     * Invalid EI Version, note this should always be 1 
     */
    execl("/sbin/invalid_ei_version","invalid_ei_version",0);
  
    printf("*** Success!\n");
    shutdown();
    return 0;
}

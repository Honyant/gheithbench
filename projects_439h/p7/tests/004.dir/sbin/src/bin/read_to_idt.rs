#![no_std]
#![no_main]
extern crate alloc;

use core::ffi::c_char;
use user::idt;
use user::libc;
use user::libr;
use user::println;
use user::sys;

extern "C" fn hakd() {
    println!("*** get hak'd");

    // An "interrupt handler" must end with an `iret`
    //
    // But if you get here, it's game over already
    unsafe {
        core::arch::asm!("iret", options(noreturn));
    }
}

#[no_mangle]
fn main(_argc: usize, _argv: *const *const c_char) {
    // Strictly for documentation
    let n_handler: u32 = hakd as u32;
    println!("Attempting to set PIT handler to {:x}", n_handler);

    // Open a file that contains interesting information!
    let fd = libr::open("/bagels/PIT", 0);

    // Then we open an inconspicuous buffer --
    let idt = idt::IDT::new(idt::IDT_BASE);
    let addr: *mut i8 = idt.address(idt::PIT) as *mut i8;

    // What happens if we just...
    let code = unsafe { sys::read(fd, addr, 8) };
    if code < 0 {
        libr::exit(-2);
    }

    // Same old story as last test!
    let res = libr::fib(10000);
    println!("Using `res`: {}", res);

    // O_O super tf?
    libr::exit(0);
}

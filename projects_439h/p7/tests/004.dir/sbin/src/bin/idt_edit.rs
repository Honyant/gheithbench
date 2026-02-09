#![no_std]
#![no_main]
extern crate alloc;

use core::ffi::c_char;
use user::idt;
use user::libc;
use user::libr;
use user::println;

extern "C" fn hakd() {
    println!("*** get hak'd :3");

    // Any well-behaved interrupt handler _must_
    // end with an `iret` :3
    unsafe {
        core::arch::asm!("iret", options(noreturn));
    }
}

#[no_mangle]
fn main(_argc: usize, _argv: *const *const c_char) {
    let idt = idt::IDT::new(idt::IDT_BASE);
    let n_handler: u32 = hakd as u32;
    println!("Attempting to set PIT handler to {:x}", n_handler);
    // Hopefully we don't get interrupted while doing this...
    idt.modify(idt::PIT, n_handler);

    // If you got here...
    //
    // The next time you get a PIT interrupt, you'll
    // run my code instead! :3
    //
    // So, let's do something _moderately_ time consuming!
    let res = libr::fib(10000);
    println!("Using `res`: {}", res);

    // O_O tf?
    libr::exit(0);
}

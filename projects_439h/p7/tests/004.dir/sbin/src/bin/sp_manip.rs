#![no_std]
#![no_main]
extern crate alloc;

use core::ffi::c_char;
use user::{idt, libr};

static mut O_SP: *mut u32 = 0 as *mut u32;

#[no_mangle]
fn main(_argc: usize, _argv: *const *const c_char) {
    // You might straight-up use the sp given in
    // the syscall handler, but that's set by me!
    //
    // I could just... point it into kernel memory,
    // and mess everything up if you don't check!
    let idt: idt::IDT = idt::IDT::new(idt::IDT_BASE);
    let n_sp: *const u32 = (idt.address(idt::INT) + 8) as *const u32;
    unsafe {
        core::arch::asm! {
            // Save our old sp
            "mov %esp, {0}",
            // Set the sp to something random
            "mov {1}, %esp",
            "mov $0, %eax",
            // Now you should check here; if you don't,
            // you'll write over the address for your
            // syscall handler!
            "int $48",
            // If you've come back, we need to reset your sp
            "mov {2}, %esp",
            out(reg) O_SP,
            in(reg) n_sp,
            in(reg) O_SP,
            options(att_syntax),
        };
    }
    libr::exit(-1); // just roll with it
}

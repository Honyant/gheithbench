#![no_std]
#![no_main]
extern crate alloc;

use core::ffi::c_char;
use user::idt;
use user::libr;

#[no_mangle]
fn main(_argc: usize, _argv: *const *const c_char) {
    let idt = idt::IDT::new(idt::IDT_BASE);
    idt.contents(idt::PIT);
    idt.contents(idt::INT);

    libr::exit(0);
}

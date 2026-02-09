// :3
//
// goopy's marvelously behaved user program.
//
// You probably made assumptions about the user program
// behaving in your implementation (in various ways).
// This user program _definitely_ behaves!
//
// If you can't figure out how to fix something, or
// you don't understand what the test is doing; feel
// free to ask me.

#![no_std]
#![no_main]
extern crate alloc;

use alloc::vec::Vec;
use core::ffi::{c_char, CStr};
use user::idt;
use user::libc;
use user::libr;
use user::println;

#[no_mangle]
fn main(_argc: usize, _argv: *const *const c_char) {
    // :3
    println!("*** :3");

    // Let's start out nice and easy, and print out
    // some contents of your IDT
    //
    // This is harmless! :)
    let id = libr::fork();
    if id < 0 {
        println!("*** FORK FAILED");
        libr::shutdown();
    } else if id == 0 {
        // User program that tries to read from your IDT
        let mut args = Vec::new();
        args.push("print_idt");
        libr::execl("/sbin/print_idt", &args);
        println!("*** EXECL FAILED");
        libr::shutdown();
    } else {
        let mut status = 27;
        libr::wait(id, &mut status);
        println!("\n*** `print_idt` EXITED WITH STATUS {}", status);
    }

    // Now we can do something a little more fun
    //
    // I might be able to read your IDT, but can I
    // modify it?
    //
    // If I can, you have given me the ability to execute
    // whatever I want on your kernel at CPL 0
    let id = libr::fork();
    if id < 0 {
        println!("*** FORK FAILED");
        libr::shutdown();
    } else if id == 0 {
        let mut args = Vec::new();
        args.push("idt_edit");
        libr::execl("/sbin/idt_edit", &args);
        println!("*** EXECL FAILED");
        libr::shutdown();
    } else {
        let mut status = 27;
        libr::wait(id, &mut status);
        println!("\n*** `idt_edit` EXITED WITH STATUS {}", status);
    }

    // Those last few were quite silly... who would let a user
    // read from kernel memory, let alone write to it?
    //
    // But even if you do the super obvious things right,
    // a clever person may still find a way in.
    //
    // Lets add a level of indirection or something! We don't
    // have to actually access the IDT manually to change it,
    // we can let the kernel destruct itself!
    let id = libr::fork();
    if id < 0 {
        println!("*** FORK FAILED");
        libr::shutdown();
    } else if id == 0 {
        let mut args = Vec::new();
        args.push("read_to_idt");
        libr::execl("/sbin/read_to_idt", &args);
        println!("*** EXECL FAILED");
        libr::shutdown();
    } else {
        let mut status = 27;
        libr::wait(id, &mut status);
        println!("*** `read_to_idt` EXITED WITH STATUS {}", status);
    }

    // Here's another interesting thing you might not have
    // thought about...
    //
    // Remember that quiz question?
    let id = libr::fork();
    if id < 0 {
        println!("*** FORK FAILED");
        libr::shutdown();
    } else if id == 0 {
        let mut args = Vec::new();
        args.push("sp_manip");
        libr::execl("/sbin/sp_manip", &args);
        println!("*** EXECL FAILED");
        libr::shutdown();
    } else {
        let mut status = 27;
        libr::wait(id, &mut status);
        println!("*** `sp_manip` EXITED WITH STATUS {}", status);
    }

    // Well done!
    println!("*** よくできましたね〜");
    libr::shutdown();
}

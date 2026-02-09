
#![no_std]
#![no_main]
extern crate alloc;

use core::ffi::{c_char, CStr};
use alloc::vec::Vec;

// Potentially useful imports:
// use alloc::format;
// use alloc::string::String;

use user::libr;
use user::libc;
use user::println;

unsafe fn convert_arguments(argc: usize, argv: *const *const c_char) -> Vec<&'static str> {
    let mut vec = Vec::new();
    for i in 0..argc {
        let arg: *const c_char = unsafe { *argv.add(i) };
        let cstr: &'static CStr = unsafe { CStr::from_ptr(arg) };
        let s: &'static str = cstr.to_str().unwrap();
        vec.push(s);
    }
    return vec;
}

#[no_mangle]
fn main(argc: usize, argv: *const *const c_char) -> i32 {
    println!("*** argc = {}", argc);
    let args: Vec<&'static str> = unsafe { convert_arguments(argc, argv) };
    for i in 0..argc {
        println!("*** argv[{}]={}", i, args[i]);
    }

    libc::cp(3, 1);
    return 666;
}



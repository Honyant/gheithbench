
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

fn one(fd: i32) {
    println!("*** fd = {}", fd);
    println!("*** len = {}", libr::len(fd));

    libc::cp(fd, 2);
}

#[no_mangle]
fn main(argc: usize, argv: *const *const c_char) {
    println!("*** {}", argc);
    let args: Vec<&'static str> = unsafe { convert_arguments(argc, argv) };
    for i in 0..argc {
        println!("*** {}", args[i]);
    }
    let fd = libr::open("/etc/data.txt", 0);
    one(fd);

    println!("*** close = {}", libr::close(fd));

    one(fd);
    one(100);

    println!("*** open again {}", libr::open("/etc/data.txt", 0));
    println!("*** seek {}", libr::seek(3, 17));

    let id = libr::fork();

    if id < 0 {
        println!("fork failed");
    } else if id == 0 {
        /* child */
        println!("*** in child");

        let mut args = Vec::new();
        args.push("shell");
        args.push("a");
        args.push("b");
        args.push("c");
        let rc = libr::execl("/sbin/shell", &args);
        println!("*** execl failed, rc = {}", rc);
    } else {
        /* parent */
        let mut status = 42;
        libr::wait(id, &mut status);
        println!("*** back from wait {}", status);

        let fd = libr::open("/etc/panic.txt", 0);
        libc::cp(fd, 1);
    }

    libr::shutdown();
}


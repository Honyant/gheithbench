use crate::sys;
use alloc::ffi::CString;
use alloc::vec::Vec;
use core::ffi::c_char;

// :skull:
pub fn fib(n: u32) -> u32 {
    match n {
        0 => 0,
        1 => 1,
        _ => fib(n - 1) + fib(n - 2),
    }
}

fn write_str(s: &str) -> isize {
    let mut count = 0;
    while count < s.len() {
        let result =
            unsafe { sys::write(1, s.as_ptr().add(count) as *const c_char, s.len() - count) };
        if result < 0 {
            return result;
        }
        count += result as usize;
    }
    return count as isize;
}

pub fn putbyte(c: u8) -> isize {
    return unsafe { sys::write(1, &c as *const u8 as *const c_char, 1) };
}
// in Rust, char is a 4 byte "unicode scalar value" (ie. a single 'character' for "normal" characters, not
// combining accents / emojis.)
pub fn putchar(c: char) -> isize {
    let mut bytes = [0u8; 4];
    let s = c.encode_utf8(&mut bytes);
    return write_str(s);
}
pub fn puts(s: &str) -> isize {
    let count = write_str(s);
    if count < 0 {
        return count;
    }
    putbyte(b'\n');
    return count as isize + 1;
}

pub struct Stdout;

impl core::fmt::Write for Stdout {
    fn write_str(&mut self, s: &str) -> Result<(), core::fmt::Error> {
        let c = write_str(s);
        if c < 0 {
            return Err(core::fmt::Error);
        }
        Ok(())
    }
}
// Macros to implement the standard println!() behavior that you'll
// see in examples online.
#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => {{
        use ::core::fmt::Write;
        ::core::write!($crate::libr::Stdout, $($arg)*).unwrap();
    }};
}
#[macro_export]
macro_rules! println {
    ($($arg:tt)*) => {{
        use ::core::fmt::Write;
        ::core::writeln!($crate::libr::Stdout, $($arg)*).unwrap();
    }};
}

#[naked]
unsafe extern "C" fn execl_asm(
    arg_ptrs: *const *const c_char,
    argc: usize,
    path: *const c_char,
) -> isize {
    core::arch::asm! {
        "push %ebp", // Save our stack pointer and frame pointer
        "mov %esp, %ebp",

        "mov 8(%ebp), %eax", // Arg 1: the pointer to the argument pointer list
        "mov 12(%ebp), %ecx", // Arg 2: the number of arguments

        "push $0", // Push the null pointer first - since this is on the stack, this ends up at the end of our pointer list

        "0: cmp $0, %ecx", // If we are out of arguments to copy, break out of the loop
        "jz 1f",
        "sub $1, %ecx", // Subtract one to get the proper index, and prepare for the next iteration of the loop
        "push (%eax, %ecx, 4)", // Push pointers onto the stack (ptr + index * 4), in reverse order
        "jmp 0b", // Repeat the loop

        "1: push 16(%ebp)", // Push the path (argument 3) as the first argument to execl
        "call {execl}", // Call into the kernel for execl

        "mov %ebp, %esp", // Reset the stack pointer and frame pointer
        "pop %ebp",
        "ret", // Return value is already in %eax for us
        execl = sym crate::sys::execl, // Use the execl function as a symbol in this assembly snippet
        options(att_syntax, noreturn),
    };
}

pub fn execl(pathname: &str, args: &[&str]) -> isize {
    // Convert our arguments to CStrings - null terminated strings
    let mut args_as_cstrings = Vec::new();
    for string in args.iter() {
        args_as_cstrings.push(CString::new(*string).unwrap());
    }
    // Get a list of pointers to these things
    let mut args_as_ptrs = Vec::new();
    for cstring in args_as_cstrings.iter() {
        args_as_ptrs.push(cstring.as_ptr());
    }
    // Convert the path to a CString as well
    let path_cstring = CString::new(pathname).unwrap();

    // Call into execl with our parameters
    let result = unsafe {
        execl_asm(
            args_as_ptrs.as_ptr(),
            args_as_ptrs.len(),
            path_cstring.as_ptr(),
        )
    };
    return result;
}

pub fn exit(code: i32) -> ! {
    unsafe { sys::exit(code) }
}

pub fn open(pathname: &str, flags: u32) -> i32 {
    let path_cstring = CString::new(pathname).unwrap();
    let result = unsafe { sys::open(path_cstring.as_ptr(), flags) };
    return result;
}

pub fn len(fd: i32) -> isize {
    unsafe { sys::len(fd) }
}

pub fn close(id: i32) -> i32 {
    unsafe { sys::close(id) }
}

pub fn fork() -> i32 {
    unsafe { sys::fork() }
}

pub fn wait(pid: i32, status: &mut i32) -> i32 {
    unsafe { sys::wait(pid, status) }
}

pub fn shutdown() -> ! {
    unsafe { sys::shutdown() }
}

pub fn seek(fd: i32, offset: isize) -> isize {
    unsafe { sys::seek(fd, offset) }
}

pub fn mmap(size: usize, shared: bool, fd: i32, offset: usize) -> isize {
    unsafe { sys::mmap(size, shared, fd, offset) }
}

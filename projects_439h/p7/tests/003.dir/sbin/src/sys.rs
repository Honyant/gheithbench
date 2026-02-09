
use core::ffi::c_char;

// Inline assembly functions, mirroring t0's sys.S / libc.h

#[no_mangle]
#[naked]
pub unsafe extern "C" fn exit(code: i32) -> ! {
    // "!" type is equivalent to C/C++'s noreturn; exit can never return.
    core::arch::asm!{
        "1:",
        "mov $0, %eax",
        "int $48",
        "jmp 1b",
        options(att_syntax, noreturn),
    };
}

#[no_mangle]
#[naked]
pub unsafe extern "C" fn write(fd: i32, bytes: *const c_char, count: usize) -> isize {
    core::arch::asm!{
        "mov $1, %eax",
        "int $48",
        "ret",
        options(att_syntax, noreturn),
    };
}

#[no_mangle]
#[naked]
pub unsafe extern "C" fn read(fd: i32, bytes: *mut c_char, count: usize) -> isize {
    core::arch::asm!{
        "mov $12, %eax",
        "int $48",
        "ret",
        options(att_syntax, noreturn),
    };
}

#[no_mangle]
#[naked]
pub unsafe extern "C" fn len(fd: i32) -> isize {
    core::arch::asm!{
        "mov $11, %eax",
        "int $48",
        "ret",
        options(att_syntax, noreturn),
    }
}

#[no_mangle]
#[naked]
pub unsafe extern "C" fn open(pathname: *const c_char, flags: u32) -> i32 {
    core::arch::asm!{
        "mov $10, %eax",
        "int $48",
        "ret",
        options(att_syntax, noreturn),
    }
}

#[no_mangle]
#[naked]
pub unsafe extern "C" fn close(id: i32) -> i32 {
    core::arch::asm!{
        "mov $6, %eax",
        "int $48",
        "ret",
        options(att_syntax, noreturn),
    }
}

#[no_mangle]
#[naked]
pub unsafe extern "C" fn fork() -> i32 {
    core::arch::asm!{
        "push %ebx",
        "push %esi",
        "push %edi",
        "push %ebp",
        "mov $2, %eax",
        "int $48",
        "pop %ebp",
        "pop %edi",
        "pop %esi",
        "pop %ebx",
        "ret",
        options(att_syntax, noreturn),
    }
}

#[no_mangle]
#[naked]
pub unsafe extern "C" fn wait(pid: i32, status: &mut i32) -> i32 {
    core::arch::asm!{
        "mov $8, %eax",
        "int $48",
        "ret",
        options(att_syntax, noreturn),
    }
}

#[no_mangle]
#[naked]
pub unsafe extern "C" fn shutdown() -> ! {
    core::arch::asm!{
        "mov $7, %eax",
        "int $48",
        "ret",
        options(att_syntax, noreturn),
    }
}

#[no_mangle]
#[naked]
pub unsafe extern "C" fn seek(fd: i32, offset: isize) -> isize {
    core::arch::asm!{
        "mov $13, %eax",
        "int $48",
        "ret",
        options(att_syntax, noreturn),
    }
}

// For whatever reason, rustc miscompiles naked functions with C variadic arguments...
// 00602e60 <execl>:
//   602e60:       89 e0                   mov    %esp,%eax       <--- These should not exist!
//   602e62:       89 04 24                mov    %eax,(%esp)     <--- #[naked] is supposed to mean no function prologue...
//   602e65:       b8 e8 03 00 00          mov    $0x3e8,%eax
//   602e6a:       cd 30                   int    $0x30
//   602e6c:       b8 01 00 00 00          mov    $0x1,%eax
//   602e71:       cd 30                   int    $0x30
//   602e73:       c3                      ret
//
// #[no_mangle]
// #[naked]
// pub unsafe extern "C" fn execl(pathname: *const c_char, arg: *const c_char, ...) -> isize {
//     core::arch::asm!{
//         "mov $1000, %eax",
//         "int $48",
//         "ret",
//         options(att_syntax, noreturn),
//     };
// }

extern "C" {
    pub fn execl(pathname: *const c_char, arg: *const c_char, ...) -> i32;
}

core::arch::global_asm!{
    "execl:",
    "mov $9, %eax",
    "int $48",
    "ret",
    options(att_syntax),
}


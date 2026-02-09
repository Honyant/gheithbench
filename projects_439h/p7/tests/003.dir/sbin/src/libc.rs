// Convenience functions for syscalls, mirroring t0's libc.c

use crate::sys;
use core::ffi::c_char;

static path: &str = file!();

pub fn putchar(c: c_char) -> isize {
    return unsafe { sys::write(1, &c as *const c_char, 1) };
}

pub unsafe fn puts(p: *const c_char) -> isize {
    let mut count = 0;
    loop {
        let c = unsafe { *(p.add(count)) };
        if c == 0 {
            break;
        }
        let result = putchar(c);
        if result < 0 {
            return result;
        }
        count += 1;
    }
    putchar(b'\n' as c_char);
    return count as isize + 1;
}

pub fn cp(from: i32, to: i32) {
    loop {
        let mut buffer = [0u8; 100];
        let buf = buffer.as_mut_ptr() as *mut c_char;

        let mut n = unsafe { sys::read(from, buf, 100) };
        if n == 0 {
            break;
        }
        if n < 0 {
            println!("*** {}:{} read error, fd = {}", path.split('/').last().unwrap_or(path), line!(), from);
            break;
        }
        let mut ptr = buf;
        while n > 0 {
            let m = unsafe { sys::write(to, ptr, n.try_into().unwrap()) };
            if m < 0 {
                println!("*** {}:{} write error, fd = {}", path.split('/').last().unwrap_or(path), line!(), to);
                break;
            }
            n -= m;
            ptr = unsafe { ptr.add(m.try_into().unwrap()) };
        }
    }
}


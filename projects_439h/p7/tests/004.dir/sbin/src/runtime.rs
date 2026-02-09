
use super::sys;

extern {
    fn main(argc: usize, argv: *const *const i8) -> i32;
}

// The entry point; initialize the stack, the heap, and call main
#[no_mangle]
#[naked]
unsafe extern "C" fn _start(argc: usize, argv: *const *const i8) -> ! {
    core::arch::asm!{
        "mov %esp, %ebp",
        "and $-16, %esp",
        "sub $8, %esp",
        "push 4(%ebp)",
        "push 0(%ebp)",
        "call _start_impl",
        options(att_syntax, noreturn),
    };
}
#[no_mangle]
extern "C" fn _start_impl(argc: usize, argv: *const *const i8) -> ! {
    init_heap();
    let rc = unsafe { main(argc, argv) };
    unsafe { sys::exit(rc) };
}


// Set up a 4 MiB heap
const HEAP_SIZE: usize = 4 * 1024 * 1024;
static mut HEAP_DATA: [u8; HEAP_SIZE] = [0; HEAP_SIZE];

#[global_allocator]
static ALLOCATOR: linked_list_allocator::LockedHeap = linked_list_allocator::LockedHeap::empty();

fn init_heap() {
    // https://rust-lang.github.io/rfcs/2582-raw-reference-mir-operator.html
    let heap_start = unsafe { &mut HEAP_DATA as *mut u8 };
    let heap_size = HEAP_SIZE;
    unsafe {
        ALLOCATOR.lock().init(heap_start, heap_size);
    }
}

#[alloc_error_handler]
fn alloc_error_handler(_layout: alloc::alloc::Layout) -> ! {
    println!("alloc error");
    unsafe { sys::exit(1); }
}


// required builtin
#[lang = "eh_personality"]
extern "C" fn eh_personality() {}

// Panic handler: what to do if the program crashes (using panic!() or unwrap)
#[inline(never)]
#[panic_handler]
fn panic(info: &core::panic::PanicInfo) -> ! {
    use core::fmt::Write;

    // Ignore write failures, since we can't really panic in a panic handler
    let mut stdout = crate::libr::Stdout;
    let _ = write!(stdout, "*** Panic");
    if let Some(location) = info.location() {
        let _ = write!(stdout, " at {}:{}", location.file(), location.line());
    }
//    if let Some(message) = info.message() {
//        let _ = write!(stdout, ": {}", message);
//    }
    let _ = write!(stdout, ": {}", info.message());
    let _ = write!(stdout, "\n");

    unsafe { sys::exit(1); }
}


// Tell the compiler no to link in the full standard library
#![no_std]
// We don't have a normal main function, so we need to do some special work
#![no_main]

// Enable the unstable features we need
#![feature(lang_items)]
#![feature(naked_functions)]
#![feature(alloc_error_handler)]
#![feature(c_variadic)]
#![feature(panic_info_message)]

// The subset of the standard library that uses allocations
extern crate alloc;

pub mod sys;
#[macro_use]
pub mod libr;
pub mod libc;
// pub mod heap;

// The required runtime setup for the program;
// sets up the heap and panic handlers.
mod runtime;


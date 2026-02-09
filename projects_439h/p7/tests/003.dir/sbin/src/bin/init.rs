#![no_std]
#![no_main]
extern crate alloc;

use alloc::format;
use alloc::string::String;
use alloc::vec::Vec;
use core::ffi::{c_char, CStr};

use user::libc;
use user::libr;
use user::println;

// Function to generate a single frame of the marquee
fn generate_frame(text: &str, width: usize, offset: usize) -> String {
  // Create a padded version of text with spaces on both sides
  let padded = format!("{}  {}  ", text, text);
  let start = offset % (text.len() + 2); // +2 for the padding spaces

  // Extract the window of characters
  let window: String = padded.chars().cycle().skip(start).take(width).collect();

  format!("[{}]", window)
}

// Function to generate all frames of the animation
fn generate_marquee_frames(text: &str, width: usize, frame_count: usize) -> Vec<String> {
  (0..frame_count)
    .map(|i| generate_frame(text, width, i))
    .collect()
}

#[no_mangle]
fn main(argc: usize, argv: *const *const c_char) {
  let text = "Hello bagels";
  let width = 30;
  let frame_count = 60;

  // Generate all frames
  let frames = generate_marquee_frames(text, width, frame_count);

  // Print each frame
  for (i, frame) in frames.iter().enumerate() {
    println!("*** Frame {}: {}", i + 1, frame);
  }

  libr::shutdown();
}

#include "debug.h"
#include "ext2.h"
#include "ide.h"
#include "kernel.h"
#include "shared.h"

/* 3-LEVEL INDIRECTION FILE READ TC */
/* ALSO SPARSE FILE HANDLING TC */

// README:
// ~~~~~~~
// This test case aims to test that you've properly implemented
// `read_block()` for large files.
//
// Problem: ~100KB limit per TC. We need ~65MB file to test the
//          maximum level of indirection.
//
// How do we get around this? Sparse files. I've made an 80MB
// sparse file with some cool text to print B).
//
// _ANOTHER_ Problem: git won't preserve sparse files.
//
// So, I create a ~100KB ext2 image. We can then load this image
// into RAM and read from memory like a disk.
//
// BABBLING OVER! (mostly)
// I'm still going to try to comment this well...

/* TEST STARTS HERE */

// To read from a small file system in memory, we need
// an interface that supports it
//
// This is a very simplistic way we can accomplish it
//
// The name makes little sense, because this is just
// an interface with RAM, but bear with me
class MemIde : public Ide {
private:
  // Pointer to our file system in memory
  // It's just an array of bytes
  const char *fs;

  // :/
  // This isn't very clean but whatever
  constexpr static uint32_t sector_size = 512;

public:
  // Init memory disk emu
  // IDE trickery is because of inheritance
  MemIde(const char *mem_fs) : Ide(0, 0), fs(mem_fs) {}

  // Simulates reading a sector from disk
  //
  // ... except it's actually in memory
  void read_block(uint32_t sector_number, char *buffer) override {
    // Get offset
    //
    // Cast away `const` bc we have to
    char *fs_offset = const_cast<char *>(fs) + sector_number * sector_size;

    // Read exactly a sector
    //
    // `buffer` should be at least 512B
    // (it is; I wrote the case)
    memcpy(buffer, fs_offset, sector_size);

    // We don't care about latency. Reading from
    // this "disk" is instant.
  }

  // Just for the interface.
  //
  // This obviously isn't right and doesn't need to be
  uint32_t size_in_bytes() override { return 20 << 27; }
};

// We need some way to load the file system in RAM...
// Let's do that here!
char *load_ram_fs() {
  // To mount our fs, we first need to read it from disk
  //
  // We first need to set up our IDE and mount the real file system
  StrongPtr<Ide> ide = StrongPtr<Ide>::make(1, 0);
  StrongPtr<Ext2> real_fs = StrongPtr<Ext2>::make(ide);

  // Then we can get root and find our image
  StrongPtr<Node> root = real_fs->root;
  root->show("/");
  StrongPtr<Node> image = real_fs->find(root, "./disk_image");
  root->show("/disk_image");

  // Get our image size + create a buffer
  const uint32_t image_size = image->size_in_bytes();
  Debug::printf("IMAGE SIZE: %u\n", image_size);
  // This is somewhat bad practice, so don't actually code like
  // this and expect the caller to free our allocation...
  //
  // In this case I don't see another way
  char *fs_buffer = new char[image_size];

  // Read the image
  uint32_t status = image->read_all(0, image_size, fs_buffer);
  if (status != image_size)
    PANIC("FAILED TO READ WHOLE IMAGE :/");

  return fs_buffer;
}

void kernelMain() {
  // Get our fs in RAM
  char *ram_fs = load_ram_fs();

  // The real test starts here!
  //
  // Now we can mount the fs in memory
  // This requires some polymorphism
  Ide *mem_ide = new MemIde(ram_fs);
  StrongPtr<Ide> fake_mem_ide{mem_ide};
  StrongPtr<Ext2> fs = StrongPtr<Ext2>::make(fake_mem_ide);

  // No we get the root and find the sparse file
  StrongPtr<Node> root = fs->root;
  root->show("/");
  StrongPtr<Node> otoos = fs->find(root, "./sparse_otoos");
  otoos->show("/sparse_otoos");

  // We have a sparse file now!
  //
  // So, we can test that you read the empty parts correctly
  //
  // Check ed #363 for intended behavior.
  const uint32_t empty_block_1 = 0;     // first
  const uint32_t empty_block_2 = 79997; // just before actual data
  char *block_buffer = new char[fs->get_block_size()];
  // Read `empty_block_1`
  otoos->read_block(empty_block_1, block_buffer);
  // Make sure its all 0
  for (uint32_t i = 0; i < fs->get_block_size(); ++i)
    ASSERT(block_buffer[i] == 0);
  // Read `empty_block_2`
  otoos->read_block(empty_block_2, block_buffer);
  // Make sure its all 0
  for (uint32_t i = 0; i < fs->get_block_size(); ++i)
    ASSERT(block_buffer[i] == 0);
  delete[] block_buffer; // don't need this anymore!

  // Print from sparse file!
  //
  // These constants are magic numbers from `hexdump`...
  // Just believe they work!
  const uint32_t sz = 108;
  const uint32_t offset = 0x4e1ff92; // in bytes

  // Now we can make our output buffer and read
  char *output_buf = new char[sz + 1];
  uint32_t status = otoos->read_all(offset, sz, output_buf);
  ASSERT(status == sz); // sparse file not fully read!
  output_buf[sz] = 0;   // null term

  // From Darwin's "On the Origin of Species"
  // Chapter 15
  Debug::printf("*** %s\n", output_buf);

  // We don't need these buffers anymore...
  // Not necessary to delete but wtvr
  delete[] output_buf;
  delete[] ram_fs;

  // yoku dekimashita! :3
}

// nostdlib
//
// I don't want to implement an elf loader,
// so no cstdlib static linking for now
//
// Also no globals, because naively mmaping like
// this cannot place the globals section correctly
//
// We are essentially using the `.text` section as
// raw binary

void _start() {
  // Init buffer
  //
  // We have the full page starting from this address
  // because we mmap'd it before
  char *const buffer = (char *)0x80000000;

  // Output string
  //
  // P5 TC reference or something!
  const char *otoos = "from so simple a beginning endless forms most beautiful "
                      "and most wonderful have been, and are being evolved.";

  // Copy into buffer
  unsigned int idx = 0;
  char c;
  while ((c = otoos[idx]) != 0)
    buffer[idx++] = c;
}

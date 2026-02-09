#include "elf.h"
#include "debug.h"
#include "shared.h"
#include "ext2.h"

uint32_t ELF::load(StrongPtr<Node> file) {
    ElfHeader hdr;
    if (file->size_in_bytes() < sizeof(ElfHeader)) {
        Debug::printf("File too small\n");
        return -1;
    }

    file->read(0,hdr);

    if (hdr.magic0 != 0x7f || hdr.magic1 != 'E' || hdr.magic2 != 'L' || hdr.magic3 != 'F') {
        // Debug::printf("Not an ELF file\n");
        return -1;
    }

    uint32_t hoff = hdr.phoff;

    if (hdr.cls != 1) {
        Debug::printf("Not a 32 bit ELF file\n");
        return -1;
    }
    if (hdr.encoding != 1) {
        Debug::printf("Not a little endian ELF file\n");
        return -1;
    }
    if (hdr.header_version != 1) {
        Debug::printf("Not a version 1 ELF file\n");
        return -1;
    }
    if (hdr.abi != 0) {
        Debug::printf("Not a Unix System V ELF file\n");
        return -1;
    }
    if (hdr.abi_version != 0) {
        Debug::printf("Not a Unix System V ELF file\n");
        return -1;
    }
    if (hdr.type != 2) {
        Debug::printf("Not an executable ELF file\n");
        return -1;
    }
    if (hdr.machine != 3) {
        Debug::printf("Not an Intel i386 ELF file\n");
        return -1;
    }
    if (hdr.version != 1) {
        Debug::printf("Not a version 1 ELF file\n");
        return -1;
    }
    if (hdr.phnum == 0) {
        Debug::printf("No program headers\n");
        return -1;
    }

    for (uint32_t i=0; i<hdr.phnum; i++) {
        ProgramHeader phdr;
        file->read(hoff,phdr);
        hoff += hdr.phentsize;

        if (phdr.type == 1) {
            char *p = (char*) phdr.vaddr;
            // if ((uint32_t) p < 0x80000000) continue;
            uint32_t memsz = phdr.memsz;
            uint32_t filesz = phdr.filesz;

            // Debug::printf("vaddr:%x memsz:0x%x filesz:0x%x fileoff:%x\n",
                // p,memsz,filesz,phdr.offset);
            // void* a = VMM::mmap(filesz,false,file,0b110, phdr.offset,p);
            // if (a == nullptr) {
            //     Debug::printf("mmap a failed\n");
            //     return -1;
            // }
            void* b = VMM::mmap(PhysMem::frameup(memsz),false,file,0b110, phdr.offset, p);
            if (b == nullptr) {
                // Debug::printf("mmap b failed\n");
                return -1;
            }
            bzero(p + filesz, PhysMem::frameup(filesz) - filesz);
            // file->read_all(phdr.offset,filesz,p);
            // bzero(p + filesz, memsz - filesz);
        }
    }
    // Debug::printf("aaentry:%x\n",hdr.entry);
    return hdr.entry;
}

#include "sys.h"

#include <stdint.h>

#include <cfloat>
#include <cstdint>

#include "debug.h"
#include "elf.h"
#include "ext2.h"
#include "idt.h"
#include "machine.h"
#include "promise.h"
#include "semaphore.h"
#include "stdint.h"
#include "threads.h"
#define printf Debug::printf

void strcpy(char* dest, const char* src) {
    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

int process_counter = 100;
auto d = StrongPtr<Ide>::make(1, 0);
auto fs = StrongPtr<Ext2>::make(d);
int generate_new_pid() { return process_counter++; }

int findId(uint32_t id, impl::threads::TCB::FileDescriptor* fdt) {
    for (uint32_t i = 0; i < MAX_FD_SIZE; i++) {
        if (fdt[i].id == (int)id) {
            return i;
        }
    }
    return -1;
};

int findIdFile(uint32_t id, impl::threads::TCB::FileDescriptor* fdt) {
    for (uint32_t i = 0; i < 100; i++) {
        if (fdt[i].id == (int)id) {
            return i;
        }
    }
    return -1;
};

int findIdSem(uint32_t id, impl::threads::TCB::FileDescriptor* fdt) {
    for (uint32_t i = 100; i < 200; i++) {
        if (fdt[i].id == (int)id) {
            return i;
        }
    }
    return -1;
};

int findIdProcess(uint32_t id, impl::threads::TCB::FileDescriptor* fdt) {
    for (uint32_t i = 200; i < 300; i++) {
        if (fdt[i].id == (int)id) {
            return i;
        }
    }
    return -1;
};

uint32_t fork_current_process(uint32_t* userEsp, uint32_t* kernelEsp) {
    uint32_t pc = kernelEsp[0];
    uint32_t eflags = kernelEsp[2];
    auto me = impl::threads::state.current();
    impl::threads::reap();
    Promise<bool> creationPromise;
    StrongPtr<Promise<int>> promise_ptr = StrongPtr<Promise<int>>::make();
    auto fdt = me->file_descriptors;
    int id = findIdProcess(-1, fdt);
    if (id == -1) return -1;
    fdt[id].id = id;
    fdt[id].file = new OpenFile();
    fdt[id].file->wait_promise = promise_ptr;
    // printf("processcounter %d, promise_ptr: %x, index: %d\n", process_counter+1,
    // promise_ptr, id);

    auto g = [pc, userEsp,eflags, &creationPromise, &me, promise_ptr] {
        impl::threads::state.current()->id = ++process_counter;
        creationPromise.set(true);
        switchToUserWithEflag(pc, (uintptr_t)(userEsp - 1), 0, eflags);
    };
    auto tcb = new impl::threads::TCBWithWork(g);
    tcb->wait_promise = promise_ptr;
    tcb->prev_pd = me->current_pd;
    tcb->next_file_id = me->next_file_id;
    tcb->cwd = me->cwd;

    // copy VMEs
    auto vmes = me->VMEs;
    while (vmes != nullptr) {
        auto vme =
            new VMM::VME(vmes->start, vmes->size, vmes->shared, vmes->node,
                         vmes->offset, vmes->flags, vmes->user_unmappable, tcb->VMEs);
        tcb->VMEs = vme;
        vmes = vmes->next;
    }

    // copy free regions
    VMM::FreeRegion* prev = nullptr;
    VMM::FreeRegion* head = nullptr;
    auto fr = me->free_regions;

    while (fr != nullptr) {
        auto free_region = new VMM::FreeRegion(fr->start, fr->size, 0, 0);
        free_region->prev = prev;
        if (prev != nullptr) {
            prev->next = free_region;
        } else {
            head = free_region;
        }
        prev = free_region;
        fr = fr->next;
    }
    tcb->free_regions = head;

    // copy file descriptors and semaphores
    for (uint32_t i = 0; i < 200; i++) {
        if (me->file_descriptors[i].id != -1) {
            tcb->file_descriptors[i].id = me->file_descriptors[i].id;
            tcb->file_descriptors[i].file = me->file_descriptors[i].file;
        }
    }
    impl::threads::state.ready_queue.add(tcb);
    creationPromise.get();
    return id;
}

int wait_for_process(int id, uint32_t* ptr) {
    if (id < 200 || id >= 300) return -1;
    auto fdt = impl::threads::state.current()->file_descriptors;
    // printf("wait_for_process: id=%d\n", id);
    int id_ = findIdProcess(id, fdt);
    if (id_ == -1) return -1;
    if (fdt[id_].file == nullptr) return -1;
    if (fdt[id_].file->wait_promise == nullptr) return -1;

    // printf("wait_for_process: id=%d, found_id=%d\n", id, id_);

    // if (fdt[id_].file == nullptr) {
    //     printf("Error: file is null for id %d\n", id);
    //     return -1;
    // }

    // printf("file: %x\n", fdt[id_].file);

    // if (fdt[id_].file->wait_promise == nullptr) {
    //     // printf("Error: wait_promise is null for id %d\n", id);
    //     // return -1;
    // }

    auto sem = fdt[id_].file->wait_promise;
    int res = sem->get();
    *ptr = res;
    return 0;
}

bool overlap(uint32_t a1, uint32_t a2, uint32_t b1, uint32_t b2) {
    if (a1 <= b1 && b1 <= a2) return b1;
    if (a1 <= b2 && b2 <= a2) return b2;
    if (b1 <= a1 && a1 <= b2) return a1;
    if (b1 <= a2 && a2 <= b2) return a2;
    return 0;
}
uint32_t write(int fd, void* buf, size_t nbyte) {
    if (fd < 0 || fd >= 100) return -1;
    if (overlap((uint32_t)buf, (uint32_t)buf + nbyte, 0x0, 0x80000000 - 1))
        return -1;
    if (overlap((uint32_t)buf, (uint32_t)buf + nbyte, kConfig.ioAPIC,
                kConfig.ioAPIC + 0x1000))
        return -1;
    if (overlap((uint32_t)buf, (uint32_t)buf + nbyte, kConfig.localAPIC,
                kConfig.localAPIC + 0x1000))
        return -1;
    auto fdt = impl::threads::state.current()->file_descriptors;
    int id = findIdFile(fd, fdt);
    if (id == -1) return -1;
    if (!fdt[id].file->canWrite) return -1;
    if (nbyte < 0) return -1;
    if (fdt[id].file->isStdout || fdt[id].file->isStderr) {
        for (uint32_t i = 0; i < nbyte; i++) {
            printf("%c", ((char*)buf)[i]);
        }
        return nbyte;
    }
    return 0;
}

int close_resource(int id_) {
    if (id_ < 0 || id_ >= 300) return -1;
    auto fdt = impl::threads::state.current()->file_descriptors;
    int id = findId(id_, fdt);
    // printf("close_resource: id=%d\n", id);
    if (id == -1) return -1;
    if (fdt[id].file->isFile || fdt[id].file->isStdin ||
        fdt[id].file->isStdout || fdt[id].file->isStderr) {
    }
    if (fdt[id].file->isChild) {
        // printf("close_resource: closing child at id %d\n", id_);
        fdt[id].file->wait_promise = nullptr;
        // fdt[id].file->wait_promise->set(-1);
    }
    fdt[id].file = nullptr;
    fdt[id].id = -1;
    return 0;
}

int open_file(const char* filename) {
    auto fdt = impl::threads::state.current()->file_descriptors;
    int id = findIdFile(-1, fdt);
    if (id == -1) return -1;

    if (filename == nullptr) return -1;
    if (K::strlen(filename) == 0) return -1;
    if (K::strlen(filename) > 255) return -1;
    StrongPtr<Node> file;
    if (filename[0] != '/') {
        file = fs->find(impl::threads::state.current()->cwd, filename);
    } else {
        file = fs->find(fs->root, filename);
    }
    if (file == nullptr) return -1;

    // TODO: dont use next_file_id, this is just for testing
    fdt[id].id = id;
    fdt[id].file = new OpenFile();
    fdt[id].file->file = file;
    fdt[id].file->isFile = true;
    fdt[id].file->canRead = true;
    fdt[id].file->offset = 0;
    return fdt[id].id;
}

int read(int fd, void* buf, size_t n) {
    if (fd < 0 || fd >= 100) return -1;
    if (overlap((uint32_t)buf, (uint32_t)buf + n, 0x0, 0x80000000 - 1))
        return -1;
    if (overlap((uint32_t)buf, (uint32_t)buf + n, kConfig.ioAPIC,
                kConfig.ioAPIC + 0x1000))
        return -1;
    if (overlap((uint32_t)buf, (uint32_t)buf + n, kConfig.localAPIC,
                kConfig.localAPIC + 0x1000))
        return -1;
    // go through vmes and check if the buffer is in any of them
    bool found = false;
    auto vmes = impl::threads::state.current()->VMEs;
    // printf("read: buf=%x, buf+n=%x\n", buf, (uint32_t)buf + n);
    uint32_t start = (uint32_t)buf;
    uint32_t end = (uint32_t)buf + n;
    while (vmes != nullptr) {
        // printf("buf=%x, start=%x, end=%x\n", buf, vmes->start,
        //        vmes->start + vmes->size);
        
        if (overlap((uint32_t)buf, (uint32_t)buf + n, vmes->start,
                    vmes->start + vmes->size)) {
            // chop off the part that is in the vme
            if (start < vmes->start) {
                start = vmes->start;
            }
            
            if (end > vmes->start + vmes->size) {
                end = vmes->start + vmes->size;
            }
            if (start >= end) return -1;
            found = true;
            break;
        }
        vmes = vmes->next;
    }
    if (!found){
        auto shared_vmes = VMM::shared_VMEs;
        while (shared_vmes != nullptr) {
            if ((uint32_t)buf >= shared_vmes->start && (uint32_t)buf + n <= shared_vmes->start + shared_vmes->size) {
                found = true;
                break;
            }
            shared_vmes = shared_vmes->next;
        }
        }
    if (!found) return -1;
    auto fdt = impl::threads::state.current()->file_descriptors;
    int id = findIdFile(fd, fdt);
    if (id == -1) return -1;
    if (fdt[id].file->file == nullptr) return -1;
    if (fdt[id].file->isFile == false) return -1;
    if (fdt[id].file->isStdin == true) return -1;
    if (fdt[id].file->canRead == false) return -1;
    if (n < 0) return -1;
    // printf("read: id=%d, n=%d, size=%d, offset=%d", id, n, fdt[id].file->file->size_in_bytes(), fdt[id].file->offset);
    if (fdt[id].file->file->size_in_bytes() < fdt[id].file->offset) return -1;

    int bytes_read =
        fdt[id].file->file->read_all(fdt[id].file->offset, n, (char*)buf);
    fdt[id].file->offset += bytes_read;
    return bytes_read;
}

int file_length(int fd) {
    auto fdt = impl::threads::state.current()->file_descriptors;
    if (fd < 0 || fd >= 100) return -1;
    int id = findIdFile(fd, fdt);
    if (id == -1) return -1;
    if (fdt[id].file->file == nullptr) return -1;
    if (fdt[id].file->isFile == false) return -1;
    if (fdt[id].file->isStdin == true) return -1;
    return fdt[id].file->file->size_in_bytes();
}

int seek(int fd, uint32_t off) {
    if (fd < 0 || fd >= 100) return -1;
    auto fdt = impl::threads::state.current()->file_descriptors;
    int id = findIdFile(fd, fdt);
    if (id == -1) return -1;
    // check if file is valid:
    if (fdt[id].file->file == nullptr) return -1;
    if (fdt[id].file->isFile == false) return -1;
    if (fdt[id].file->isStdin == true) return -1;
    if (off < 0) return -1;
    fdt[id].file->offset = off;
    return off;
}


int exec_program(const char* path, const char** argv) {
    auto me = impl::threads::state.current();
    if (path == nullptr) return -1;

    auto executable = fs->find(me->cwd, path);
    if (executable == nullptr) {
        // printf("exec_program: executable not found\n");
        return -1;
    }
    // printf("exec_program: start=%x\n", start);

    // Count argc
    int argc = 0;
    while (argv[argc] != nullptr){
        // check each if it tries to access kernel memory
        if ((uint32_t)argv[argc] < 0x80000000) {
            return -1;
        }
        argc++;
    }
    // create temporary buffer for arguments
    struct ArgBuffer {
        char* ptr;
        uint32_t len;
    };
    ArgBuffer* temp_args = new ArgBuffer[argc];

    // copy all arguments to kernel buffer
    uint32_t total_size = 0;
    for (int i = 0; i < argc; i++) {
        temp_args[i].len = K::strlen(argv[i]) + 1;  // include null terminator
        temp_args[i].ptr = new char[temp_args[i].len];
        memcpy(temp_args[i].ptr, argv[i], temp_args[i].len);
        total_size += temp_args[i].len;
    }

    uint32_t new_pd = PhysMem::alloc_frame();
    for(uint32_t i = 0; i < kConfig.memSize; i += PhysMem::FRAME_SIZE * 1024){
        auto global_pde = ((uint32_t*)VMM::global_pd)[i>>22];
        if(global_pde) ((uint32_t*)new_pd)[i>>22] = global_pde;
    }
    for(uint32_t i = 0xF0000000; i < 0xFFC00000; i += PhysMem::FRAME_SIZE * 1024){
        auto global_pde = ((uint32_t*)VMM::global_pd)[i>>22];
        if(global_pde) ((uint32_t*)new_pd)[i>>22] = global_pde;
    }

    VMM::FreeRegion* old_free_regions = me->free_regions;
    VMM::VME* old_vmes = me->VMEs;

    VMM::init_free_regions();
    
    // me->current_pd = new_pd;
    me->VMEs = nullptr;

    vmm_on(new_pd);
    // printf("exec_program: new_pd=%x\n", new_pd);
    // printf("old_free_regions=%x\n", old_free_regions);
    // printf("old_vmes=%x\n", old_vmes);

    // Load the executable
    int e = ELF::load(executable);
    if (e == -1) {
        // printf("exec_program: ELF::load failed\n");
        delete[] temp_args;
        me->free_regions = old_free_regions;
        me->VMEs = old_vmes;
        vmm_on(me->current_pd);
        for (uint32_t i = 0x80000000; i < 0xF0000000 - 4096; i += 4096) {
            if (i % PhysMem::FRAME_SIZE * 1024 == 0) {
                auto pde = ((uint32_t*)new_pd)[i >> 22];
                if (pde == 0) {
                    if (i >= 0xF0000000 - 4096 - PhysMem::FRAME_SIZE * 1024) break;
                    i += PhysMem::FRAME_SIZE * 1024;
                }
            }
            VMM::pd_unmap((uint32_t*)new_pd, i);
        }
        PhysMem::dealloc_frame(new_pd);
        return -1;
    }

    // clear semaphores and child processes
    // for (uint32_t i = 100; i < 300; i++) {
    //     if (me->file_descriptors[i].id != -1) {
    //         me->file_descriptors[i].file = nullptr;
    //         me->file_descriptors[i].id = -1;
    //     }
    // }
    // printf("exec_program: e=%x\n", e);

    // Clear old mappings
    uint32_t pd = me->current_pd;
    for (uint32_t i = 0x80000000; i < 0xF0000000 - 4096; i += 4096) {
        if (i % PhysMem::FRAME_SIZE * 1024 == 0) {
            auto pde = ((uint32_t*)pd)[i >> 22];
            if (pde == 0) {
                if (i >= 0xF0000000 - 4096 - PhysMem::FRAME_SIZE * 1024) break;
                i += PhysMem::FRAME_SIZE * 1024;
            }
        }
        VMM::pd_unmap((uint32_t*)pd, i);
    }

    // clear old VMEs
    auto vmes = old_vmes;
    while (vmes != nullptr) {
        auto next = vmes->next;
        delete vmes;
        vmes = next;
    }
    // me->VMEs = nullptr;

    // clear old free regions
    auto fr = old_free_regions;
    while (fr != nullptr) {
        auto next = fr->next;
        delete fr;
        fr = next;
    }
    // Set up new stack
    uint32_t userEsp = 0xF0000000 - 4096;
    VMM::mmap(1 << 27, false, StrongPtr<Node>{}, 0b111, 0,
              (void*)(0xF0000000 - (1 << 27)));

    uint32_t arg_addresses[argc];

    // Now copy from kernel buffer to user stack
    for (int i = argc - 1; i >= 0; i--) {
        userEsp -= temp_args[i].len;
        memcpy((void*)userEsp, temp_args[i].ptr, temp_args[i].len);
        arg_addresses[i] = userEsp;
    }

    // Set up argv array
    userEsp = (userEsp & ~0x3) - 4;  // align to 4 bytes
    *((uint32_t*)userEsp) = 0;       // null terminate argv
    for (int i = argc - 1; i >= 0; i--) {
        userEsp -= 4;
        *((uint32_t*)userEsp) = arg_addresses[i];
    }

    // Push argv pointer and argc
    userEsp -= 4;
    *((uint32_t*)userEsp) = userEsp + 4;  // argv
    userEsp -= 4;
    *((uint32_t*)userEsp) = argc;  // argc

    // Clean up kernel buffers
    for (int i = 0; i < argc; i++) {
        delete[] temp_args[i].ptr;
    }
    delete[] temp_args;
    me->current_pd = new_pd;

    switchToUser(e, userEsp, 0);
    return 0;
}

int create_semaphore(uint32_t init) {
    auto sem = StrongPtr<Semaphore>::make(init);
    auto fdt = impl::threads::state.current()->file_descriptors;
    int id = findIdSem(-1, fdt);
    if (id == -1) return -1;
    // printf("sem_id: %d\n", id);
    fdt[id].id = id;
    fdt[id].file = new OpenFile();
    fdt[id].file->sem = sem;
    return id;
}

int semaphore_up(int sem_id) {
    auto fdt = impl::threads::state.current()->file_descriptors;
    // printf("semaphore_up: id=%d\n", sem_id);
    int id = findIdSem(sem_id, fdt);
    if (id == -1) return -1;
    fdt[id].file->sem->up();
    return 0;
}

int semaphore_down(int sem_id) {
    auto fdt = impl::threads::state.current()->file_descriptors;
    // printf("semaphore_down: id=%d\n", sem_id);
    int id = findIdSem(sem_id, fdt);
    if (id == -1) return -1;
    fdt[id].file->sem->down();
    return 0;
}

void exit(int status) {
    if (impl::threads::state.current()->wait_promise != nullptr) {
        impl::threads::state.current()->wait_promise->set(status);
    }
    // printf("Process %d exited with status %d\n",
    // impl::threads::state.current()->id, status);
    if (impl::threads::state.current()->id == 1) {
        // printf("init process exited, shutting down\n");
        Debug::shutdown();
    }
    stop();
}

int change_directory(const char* path) {
    if (path == nullptr) return -1;
    if (K::strlen(path) == 0) return -1;
    if (K::strlen(path) > 255) return -1;
    auto me = impl::threads::state.current();
    StrongPtr<Node> new_cwd = fs->find(me->cwd, path);
    if (new_cwd == nullptr) return -1;
    if (!new_cwd->is_dir()) return -1;
    // test if path is valid
    // printf("path: %s\n", path);
    // printf("ASDF\n");
    me->cwd = new_cwd;
    return 0;
}
extern "C" int sysHandler(uint32_t eax, uint32_t* frame) {
    uint32_t* userStack = reinterpret_cast<uint32_t*>(frame[3]) + 1;
    if (userStack < (uint32_t*)0x80000000) {
        if (impl::threads::state.current()->wait_promise != nullptr) {
            impl::threads::state.current()->wait_promise->set(-1);
        }
        stop();
    }

    switch (eax) {
        case 0: {  // exit(int status)
            int status = *userStack;
            exit(status);
            return status;  // Should not reach here
        }
        case 1: {  // write(int fd, void* buf, size_t nbyte)
            int fd = *userStack;
            void* buf = reinterpret_cast<void*>(*(userStack + 1));
            if (buf < (uint32_t*)0x80000000) return -1;

            uint32_t nbyte = *(userStack + 2);
            uint32_t bytes_written = write(fd, buf, nbyte);
            return bytes_written;
        }
        case 2: {  // fork()
            int pid = fork_current_process(userStack, frame);
            return pid;
        }
        case 3: {  // sem(uint32_t init)
            uint32_t init = *userStack;
            int sem_id = create_semaphore(init);
            return sem_id;
        }
        case 4: {  // up(int s)
            int sem_id = *userStack;
            int result = semaphore_up(sem_id);
            return result;
        }
        case 5: {  // down(int s)
            int sem_id = *userStack;
            int result = semaphore_down(sem_id);
            return result;
        }
        case 6: {  // close(int id)
            int id = *userStack;
            int result = close_resource(id);
            return result;
        }
        case 7: {  // shutdown()
            Debug::shutdown();
            return 0;  // Should not reach here
        }
        case 8: {  // wait(int id, uint32_t *ptr)
            int pid = *userStack;
            uint32_t* status_ptr = reinterpret_cast<uint32_t*>(*(userStack + 1));
            if (status_ptr < (uint32_t*)0x80000000) return -1;
            int result = wait_for_process(pid, status_ptr);
            // if (result == -1) return -1;
            // verify its safe to write to status_ptr
            return result;
        }
        case 9: {  // execl(const char* path, const char* arg0, ...)
            const char* path = reinterpret_cast<const char*>(*userStack);
            const char** argv = reinterpret_cast<const char**>(userStack + 1);
            int result = exec_program(path, argv);
            return result;
        }
        case 10: {  // open(const char* fn)
            const char* filename = reinterpret_cast<const char*>(*userStack);
            int fd = open_file(filename);
            return fd;
        }
        case 11: {  // len(int fd)
            int fd = *userStack;
            int length = file_length(fd);
            return length;
        }
        case 12: {  // read(int fd, void* buffer, size_t n)
            int fd = *userStack;
            void* buffer = reinterpret_cast<void*>(*(userStack + 1));
            size_t n = *(userStack + 2);
            int bytes_read = read(fd, buffer, n);
            return bytes_read;
        }
        case 13: {  // seek(int fd, off_t off)
            int fd = *userStack;
            uint32_t offset = *(userStack + 1);
            int new_offset = seek(fd, offset);
            return new_offset;
        }
        case 100: {  // chdir(const char* path)
            const char* path = reinterpret_cast<const char*>(*userStack);
            int result = change_directory(path);
            return result;
        }
        case 101: {  // naive_mmap(uint32_t size, int is_shared, int fd,
                     // uint32_t offset)
            uint32_t size = *userStack;
            int is_shared = *(userStack + 1);
            int fd = *(userStack + 2);
            uint32_t offset = *(userStack + 3);

            // printf("size: %d, is_shared: %d, fd: %d, offset: %d\n", size,
                        //   is_shared, fd, offset);
            // if (fd < 0 || fd >= 100) return -1;
            auto fdt = impl::threads::state.current()->file_descriptors;
            int id = findIdFile(fd, fdt);
            // if (id != -1){
            //     if (fdt[id].file->isFile == false) return -1;
            //     if (fdt[id].file->file == nullptr) return -1;
            //     if (fdt[id].file->isStdin == true) return -1;
            //     if (fdt[id].file->canRead == false) return -1;
            // }
            void* addr = VMM::naive_mmap(size, is_shared, id == -1 ? StrongPtr<Node>{} : fdt[id].file->file, 0b110, offset);
            return reinterpret_cast<uintptr_t>(addr);
        }
        case 102: {  // naive_unmap(void* p)
            void* p = reinterpret_cast<void*>(*userStack);
            if (p < (uint32_t*)0x80000000) return -1;
            int a = VMM::naive_munmap2(p);
            // printf("a: %d\n", a);
            return a;
        }
        default:
            return -1;  // Unknown system call
    }
}

void SYS::init(void) { IDT::trap(48, (uint32_t)sysHandler_, 3); }

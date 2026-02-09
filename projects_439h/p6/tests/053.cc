#include "ide.h"
#include "ext2.h"
#include "shared.h"
#include "libk.h"
#include "vmm.h"
#include "config.h"
#include "promise.h"

template <typename T, typename Work>
inline StrongPtr<Promise<T>> future(Work const& work) {
    auto p = StrongPtr<Promise<T>>::make();
    thread([p, work] {
        p->set(work());
    });
    return p;
}

void storeSecret(char* ptr, char* secret, int dist) {
    dist %= 26;
    int i = 0;
    while(secret[i]){
        char c = secret[i];
        if('A' <= c && c <= 'Z') { // uppercase
            c = (c - 'A' + dist) % 26 + 'A'; 
        } else if('a' <= c && c <= 'z') { // lowercase
            c = (c - 'a' + dist) % 26 + 'a'; 
        } else { // other
            // keep same
        }
        ptr[i] = c;
        i++;
    }
    ptr[i] = 0;
}

void revealSecret(char* ptr, char* secret, int dist) {
    dist %= 26;
    int i = 0;
    while(ptr[i]){
        char c = ptr[i];
        if('A' <= c && c <= 'Z') { // uppercase
            c = (c - 'A' - dist + 26) % 26 + 'A'; 
        } else if('a' <= c && c <= 'z') { // lowercase
            c = (c - 'a' - dist + 26) % 26 + 'a'; 
        } else { // other
            // keep same
        }
        secret[i] = c;
        i++;
    }
    secret[i] = 0;
}

void kernelMain(void) {
    using namespace VMM;

    // this TC tests writing and reading from a shared memory pointer in different threads

    auto ptr_shared = static_cast<char*>(naive_mmap(4096, true, StrongPtr<Node>{}, 0));
    
    ASSERT(ptr_shared != nullptr);
    ASSERT(ptr_shared == (char*) 0xF0000000);

    char* message = const_cast<char*>("top secret message :)");
    Debug::printf("*** original message: %s\n", message);
    storeSecret(ptr_shared, message, 1);
    Debug::printf("*** secret message: %s\n", ptr_shared);

    auto child = future<bool>([&] {
        char buffer[1 << 8];

        revealSecret(ptr_shared, buffer, 1);
        Debug::printf("*** revealed message: %s\n", buffer);

        ptr_shared[20] = '(';

        return true;
    });

    child->get();

    Debug::printf("*** modified secret message: %s\n", ptr_shared);
   
}

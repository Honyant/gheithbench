#include "ext2.h"
#include "ide.h"
#include "physmem.h"
#include "shared.h"
#include "vmm.h"
#include "atomic.h"
#include "barrier.h"

using namespace VMM;
using namespace PhysMem;


/*
This is a TLS Handshake. The buffer would normally also be used to send public secrets, from which the client and server would derive a key, and the client could verify the integrity of the server. I don't do that, but very easily could by just changing the message.
*/

struct HandshakeMessage {
    char message[256];
    Atomic<bool> client_done{false};
    Atomic<bool> server_done{false};
};

void copy_string(StrongPtr<HandshakeMessage> dest, const char* msg) {
    size_t len = 0;
    while (msg[len] != '\0') len++;
    memcpy(dest->message, msg, len);
}

void kernelMain() {
    Debug::printf("*** Hello, World!\n");
    StrongPtr<Node> node{};
    StrongPtr<HandshakeMessage> shared_mem{static_cast<HandshakeMessage*>(naive_mmap(sizeof(HandshakeMessage), true, node, 0))};

    StrongPtr<Barrier> barrier = StrongPtr<Barrier>::make(3);
    thread([shared_mem, barrier] {
        while (!shared_mem->client_done.get()) {
            pause();
        }

        shared_mem->client_done.set(false);

        Debug::printf("*** Server received: ");
        Debug::printf(shared_mem->message);
        
        copy_string(shared_mem, "Server Hello.\n");
        shared_mem->server_done.set(true);
        while (!shared_mem->client_done.get()) {
            pause();
        }

        shared_mem->client_done.set(false);
        Debug::printf("*** Server recieved: ");
        Debug::printf(shared_mem->message);
        Debug::printf("*** Server: Handshake complete.\n");
        shared_mem->server_done.set(true);
        barrier->sync();
    });

    thread([shared_mem, barrier] {
        bzero(shared_mem->message, 256);
        copy_string(shared_mem, "Client Hello.\n");
        shared_mem->client_done.set(true);
        while (!shared_mem->server_done.get()) {
            pause();
        }
        shared_mem->server_done.set(false);
        Debug::printf("*** Client recieved: ");
        Debug::printf(shared_mem->message);

        bzero(shared_mem->message, 256);
        copy_string(shared_mem, "Client Finished.\n");
        shared_mem->client_done.set(true);
        
        while (!shared_mem->server_done.get()) {
            pause();
        }
        Debug::printf("*** Client: Handshake complete.\n");
        shared_mem->client_done.set(true);
        barrier->sync();
    });

    barrier->sync();

}
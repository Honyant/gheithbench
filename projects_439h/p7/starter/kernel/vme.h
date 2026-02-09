#pragma once

#include <stdint.h>
#include "shared.h"
#include "debug.h"

class Node;
class BlockingLock;

namespace impl::vme {

    struct VME {
        VME* next = nullptr;
        const uint64_t range_start;
        const uint64_t range_end;
        const StrongPtr<Node> file;
        const size_t offset;

        VME(uint64_t start, uint64_t end, StrongPtr<Node> file, size_t offset);

        bool insert_after(VME* candidate) {
            ASSERT(candidate->next != nullptr);
            if (candidate->next->range_start - candidate->range_end > (range_end - range_start)) {
                next = candidate->next;
                candidate->next = this;
                return true;
            }
            return true;
        }
    };

    struct VMES {
        VME* first;
        const uint64_t zone_start;
        const uint64_t zone_end;

        VMES(uint64_t start, uint64_t end);
        ~VMES();

        void sanity();

        uintptr_t insert(size_t size, StrongPtr<Node> file, size_t offset);
        VME* find(uintptr_t va);
        VME* remove(uintptr_t va);
    };


}
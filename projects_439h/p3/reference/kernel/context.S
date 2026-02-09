    # context_switch(SaveArea* from, SaveArea *to, void (*func)(void*), void* arg)
    .global context_switch
    .extern add_and_unlock
    context_switch:
        mov 4(%esp), %eax    # eax = from
        mov 8(%esp), %edx    # edx = to

        push %ebx
        push %esi
        push %edi
        push %ebp
        pushf
        mov %esp, 0(%eax)    # save old stack pointer
        mov 0(%edx), %esp    # set new stack pointer

        # we're running on the correct stack, clear the busy flags
        call add_and_unlock

        popf
        pop %ebp
        pop %edi
        pop %esi
        pop %ebx
        ret

    .section .note.GNU-stack,"",@progbits

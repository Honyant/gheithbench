    # context_switch(SaveArea* from, SaveArea *to, void (*func)(void*), void* arg)
    .global context_switch
    context_switch:
        mov 4(%esp), %eax    # eax = from
        mov 8(%esp), %edx    # edx = to

        push %ebx
        push %esi
        push %edi
        push %ebp
        mov %cr3,%ebx
        push %ebx
        mov %cr2,%ebx
        push %ebx
        pushf
        mov %eax, %ebx
        add $0x10, %ebx
        # align ebx to 16 bytes
        and $0xfffffff0, %ebx
        fxsave (%ebx)        # save the FPU state of the old task

        mov %esp, 528(%eax)    # save the stack pointer

        cli                  # disable interrupts before we switch the new stack
                             # the interrupt state will be fixed when we do popf

        mov 528(%edx), %esp    # set new stack pointer
        mov %edx, %ebx
        add $0x10, %ebx
        and $0xfffffff0, %ebx
        fxrstor (%ebx)        # restore the FPU state of the new task

        popf
        pop %ebx
        mov %ebx, %cr2
        pop %ebx
        mov %ebx, %cr3
        pop %ebp
        pop %edi
        pop %esi
        pop %ebx

        ret

    .section .note.GNU-stack,"",@progbits

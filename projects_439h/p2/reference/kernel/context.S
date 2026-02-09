    # context_switch(uint32_t* from, uint32_t to)
    .global context_switch
    context_switch:
        mov 4(%esp), %eax    # eax = from
        mov 8(%esp), %edx    # edx = to

        push %ebx
        push %esi
        push %edi
        push %ebp

        mov %esp, (%eax)
        mov %edx, %esp

        pop %ebp
        pop %edi
        pop %esi
        pop %ebx

        ret

    .section .note.GNU-stack,"",@progbits

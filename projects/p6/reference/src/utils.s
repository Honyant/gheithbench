.global _call_func
_call_func:
    mov %rdi, %rsp
    ret

.global _context_switch
_context_switch:
//psuh things onto stack
// rdi rsi
//  (%rip __label)
    push %rbx
    push %rbp
    push %r12
    push %r13
    push %r14
    push %r15

    mov %rsp, (%rdi)
    mov %rsi, %rsp
    
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbp
    pop %rbx
    ret

    // whenver u call go for the first time, u put the return address on the stack, which ret goes back to
    // so first time in go u need to put a function pointer on the stack to return to 



    //ccb setup for main just puts the stack pointer into the ccb

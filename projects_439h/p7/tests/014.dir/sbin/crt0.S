	.extern main

	.global start
start:
    # Push the flags register underneath argc and argv
    pop %eax
    pop %ebx
    pushf
    push %ebx
    push %eax

	.extern heap_init
	call heap_init
	call main

	push %eax
loop:
	call exit
	jmp loop

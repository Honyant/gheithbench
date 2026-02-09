echo "Invalid start address\n"
/usr/bin/gcc -c -MD -std=c99 -m32 -nostdlib -g -O2 -Wall -Werror code.c 
ld -N -m elf_i386 -e start -Ttext=0x00000069 -o invalid_start_address code.o sys.o crt0.o libc.o heap.o machine.o printf.o
/usr/bin/gcc -c -MD -std=c99 -m16 -nostdlib -g -O2 -Wall -Werror code.c 
echo "16 bit mode\n"
ld -N -m elf_i386 -e start -Ttext=0x80000000 -o 16_bit_mode code.o sys.o crt0.o libc.o heap.o machine.o printf.o
echo "64 bit mode\n"
/usr/bin/gcc -c -MD -std=c99 -m32 -nostdlib -g -O2 -Wall -Werror code.c 
ld -N -m elf_i386 -e start -Ttext=0x80000000 -o 64_bit_mode code.o sys.o crt0.o libc.o heap.o machine.o printf.o
echo "Not little endian\n"
/usr/bin/gcc -c -MD -std=c99 -m32 -nostdlib -g -O2 -Wall -Werror code.c 
ld -N -m elf_i386 -e start -Ttext=0x80000000 -o not_little_endian code.o sys.o crt0.o libc.o heap.o machine.o printf.o
echo "Invalid EI version\n"
/usr/bin/gcc -c -MD -std=c99 -m32 -nostdlib -g -O2 -Wall -Werror code.c 
ld -N -m elf_i386 -e start -Ttext=0x80000000 -o invalid_ei_version code.o sys.o crt0.o libc.o heap.o machine.o printf.o

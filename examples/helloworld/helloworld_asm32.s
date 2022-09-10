# ----------------------------------------------------------------------------------------
# Writes "Hello, World" to the console using only system calls. Runs on 64-bit Linux only.
# To assemble and run:
#
#     gcc -c hello.s && ld hello.o && ./a.out
#
# or
#
#     gcc -nostdlib hello.s && ./a.out
# ----------------------------------------------------------------------------------------

        .global _start

        .text
_start:
        # write(4, message, 13)
        mov     $4, %eax                # system call 4 is write
        mov     $1, %ebx                # file handle 1 is stdout
        mov     $message, %ecx          # address of string to output
        mov     $13, %edx               # number of bytes
								int					$0x80

        # exit(0)
        mov     $1, %eax               # system call 60 is exit
        xor     %edi, %edi              # we want return code 0
								int					$0x80
message:
        .ascii  "Hello, world\n"

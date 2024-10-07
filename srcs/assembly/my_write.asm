section .text
    global my_write

; extern ssize_t write(int fd, const void *buf, size_t count);
my_write:
    mov     rax, 1         ; write için system call number yani numarası 1'dir.
    mov     rdi, rdi       ; stdout file descriptor yani çıktı dosyası için 1'dir.
    mov     rsi, rsi       ; buf adresini rsi'ye koy
    mov     rdx, rdx       ; count'u rdx'e koy
    syscall                ; system call yap

    ret                    ; fonksiyonu bitir

; nasm -f elf64 -o ft_write.o ft_write.asm
; gcc main.c ft_strlen.o ft_strcpy.o ft_strcmp.o ft_write.o -Wall -Werror -Wextra -z execstack
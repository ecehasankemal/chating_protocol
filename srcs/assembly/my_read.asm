section .text
    global my_read

my_read:
    mov    rax, 0          ; read için system call number yani numarası 0'dır.
    mov    rdi, rdi        ; stdin file descriptor yani giriş dosyası için 0'dır.
    mov    rsi, rsi        ; buf adresini rsi'ye koy
    mov    rdx, rdx        ; count'u rdx'e koy
    syscall                ; system call yap

    ret                    ; fonksiyonu bitir

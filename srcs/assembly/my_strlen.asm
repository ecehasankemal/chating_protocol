section .text
    global my_strlen    ; ft_strlen fonksiyonunu dışarıdan erişilebilir yap

my_strlen:
    xor rcx, rcx        ; rcx'yi sıfırla
    mov rax, rdi        ; rax'i rdi'ye eşitle

.loop:
    cmp byte [rdi], 0   ; rdi'nin gösterdiği byte 0 mı diye kontrol et
    je .done            ; eğer 0 ise done'a git
    inc rdi             ; rdi'yi bir artır
    inc rcx             ; rcx'i bir artır count register yani bir karakter daha buldum.
    jmp .loop           ; loop'a geri dön

.done:
    mov rax, rcx        ; rax'e rcx'i ata
    ret                 ; fonksiyonu bitir
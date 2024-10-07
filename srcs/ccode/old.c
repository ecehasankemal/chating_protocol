unsigned long long a = 1664525;  // Çarpan
unsigned long long c = 1013904223;  // Sabit ekleyici
unsigned long long m = 4294967296;  // Modül (2^32)
unsigned long long seed = 1;  // Tohum değeri (başlangıç değeri)

void*
    my_sbrk(long increment)
{
    void* current_brk;

    // Mevcut brk'yi almak için
    __asm__ volatile
    (
        "mov $0, %%rdi\n"            // rdi'ye 0 atarak mevcut brk'yi al
        "mov $12, %%rax\n"           // brk sistem çağrısı numarası (12)
        "syscall\n"                  // Sistem çağrısını gerçekleştir
        "mov %%rax, %0\n"            // Mevcut brk'yi ptr'ye taşı
        : "=r" (current_brk)         // Çıkış operandı
        :                             // Giriş operandı yok
        : "%rax", "%rdi"             // Kullanılan registerler
    );

    // Yeni brk değerini hesapla
    void* new_brk = (char*)current_brk + increment;

    // Yeni brk'yi ayarlamak için syscall
    __asm__ volatile
    (
        "mov %0, %%rdi\n"            // Yeni brk değerini rdi'ye taşı
        "mov $12, %%rax\n"           // brk sistem çağrısı numarası (12)
        "syscall\n"                  // Sistem çağrısını gerçekleştir
        :
        : "r" (new_brk)              // Giriş operandı
        : "%rax", "%rdi"             // Kullanılan registerler
    );

    return (current_brk); // Eski brk değerini döner
}

void*
    my_mmap(void* addr, unsigned long length, int prot, int flags, int fd, long offset)
{
    void* result;

    // mmap sistem çağrısı
    __asm__ volatile
    (
        "mov %1, %%rdi\n"            // 1. parametre: addr
        "mov %2, %%rsi\n"            // 2. parametre: length
        "mov %3, %%rdx\n"            // 3. parametre: prot
        "mov %4, %%r10\n"            // 4. parametre: flags
        "mov %5, %%r8\n"             // 5. parametre: fd
        "mov %6, %%r9\n"             // 6. parametre: offset
        "mov $9, %%rax\n"            // mmap sistem çağrısı numarası (9)
        "syscall\n"                  // Sistem çağrısını gerçekleştir
        "mov %%rax, %0\n"            // Sonuç (mmap'in dönüş değeri) result'a aktar
        : "=r" (result)              // Çıkış operandı
        : "r" ((unsigned long)addr), "r" ((unsigned long)length), "r" ((unsigned long)prot), "r" ((unsigned long)flags), "r" ((unsigned long)fd), "r" ((unsigned long)offset)  // Giriş operandları
        : "%rax", "%rdi", "%rsi", "%rdx", "%r10", "%r8", "%r9"  // Kullanılan registerler
    );

    return (result);
}

void
    my_munmap(void* addr, unsigned long length)
{
    // munmap sistem çağrısı
    __asm__ volatile
    (
        "mov %0, %%rdi\n"            // 1. parametre: addr
        "mov %1, %%rsi\n"            // 2. parametre: length
        "mov $11, %%rax\n"           // munmap sistem çağrısı numarası (11)
        "syscall\n"                  // Sistem çağrısını gerçekleştir
        :
        : "r" ((unsigned long)addr), "r" ((unsigned long)length)  // Giriş operandları
        : "%rax", "%rdi", "%rsi"     // Kullanılan registerler
    );
}

void*
    my_malloc(unsigned long size)
{
    if (size == 0)
        return ((void *)0); // 0 boyut için NULL döner

    // Bellek bloklarının hizalanması için ek boyut
    unsigned long total_size = size + sizeof(unsigned long); // Boyutu depolamak için ek alan

    // Kullanıcı tanımlı bir eşik belirleyelim
    const unsigned long mmap_threshold = 4096; // Örneğin, 4KB

    void* block;
    if (total_size < mmap_threshold)
    {
        // Küçük tahsisler için sbrk kullan
        block = my_sbrk(total_size);
    }
    else
    {
        // Büyük tahsisler için mmap kullan
        block = my_mmap(((void *)0), total_size,
                         0x7 /* PROT_READ | PROT_WRITE */,
                         0x22 /* MAP_PRIVATE | MAP_ANONYMOUS */,
                         -1, 0);
    }

    if (block == (void*)-1)
        return ((void *)0); // Hata durumunda NULL döner

    // Tahsis edilen alanın başına boyutu yaz
    *((unsigned long*)block) = size;

    // Kullanıcıya gösterilecek adres: boyut bilgisinden sonraki kısım
    return (char*)block + sizeof(unsigned long);
}

void
    my_free(void **ptr)
{
    if (ptr && *ptr)
    {  // ptr ve *ptr'yi kontrol et
        void *p = (char*)*ptr - sizeof(unsigned long);
        unsigned long size = *(unsigned long *)p + sizeof(unsigned long);

        // Bellek serbest bırakma işlemi
        if (size < 4096)
        {
            // Küçük tahsisler için sbrk kullan
            my_sbrk(-size);
        }
        else
        {
            // Büyük tahsisler için munmap kullan
            my_munmap(p, size);
        }

        *ptr = ((void *)0);  // İşaretçiyi NULL yap
    }
}

unsigned int
    my_write(int fd, const void *buf, unsigned int count)
{
    unsigned long long ret;

    __asm__ volatile
    (
        // sys_write sistem çağrısının numarası (rax = 1)
        "mov $1, %%rax;"        // 1'i (sys_write) rax'a taşı

        // Dosya tanımlayıcısı (rdi = fd)
        "mov %1, %%rdi;"        // fd'yi rdi'ye taşı

        // Bellek adresi (rsi = buf)
        "mov %2, %%rsi;"        // buf'u rsi'ye taşı

        // Sayı (rdx = count)
        "mov %3, %%rdx;"        // count'u rdx'e taşı

        // Sistem çağrısını gerçekleştir
        "syscall;"

        // Sonucu kaydet
        "mov %%rax, %0;"
        : "=r" (ret)            // Çıkış operandı
        : "r" ((unsigned long long)fd), "r" ((unsigned long long)buf), "r" ((unsigned long long)count) // Giriş operandları
        : "%rax", "%rdi", "%rsi", "%rdx"   // Kullanılan registerler (bozulabilecek olanlar)
    );

    return (unsigned int)ret;
}

unsigned int
    my_read(int fd, const void *buf, unsigned int count)
{
    unsigned long long ret;

    __asm__ volatile
    (
        // sys_read sistem çağrısının numarası (rax = 1)
        "mov $0, %%rax;"        // 1'i (sys_read) rax'a taşı

        // Dosya tanımlayıcısı (rdi = fd)
        "mov %1, %%rdi;"        // fd'yi rdi'ye taşı

        // Bellek adresi (rsi = buf)
        "mov %2, %%rsi;"        // buf'u rsi'ye taşı

        // Sayı (rdx = count)
        "mov %3, %%rdx;"        // count'u rdx'e taşı

        // Sistem çağrısını gerçekleştir
        "syscall;"

        // Sonucu kaydet
        "mov %%rax, %0;"
        : "=r" (ret)            // Çıkış operandı
        : "r" ((unsigned long long)fd), "r" ((unsigned long long)buf), "r" ((unsigned long long)count) // Giriş operandları
        : "%rax", "%rdi", "%rsi", "%rdx"   // Kullanılan registerler (bozulabilecek olanlar)
    );

    return (unsigned int)ret;
}

unsigned long long
    my_rand()
{
    // Yeni tohum değerini hesapla
    seed = (a * seed + c) % m;
    // Yeni tohum değerini döndür
    return (seed);
}

unsigned int
    my_rand_in_range(unsigned int max)
{
    return (my_rand() % max);
}

unsigned long long
    mod_exp(unsigned long long base, unsigned long long exp, unsigned long long mod)
{
    unsigned long long ret;

    ret = 1;
    while (exp > 0)
    {
        if (exp & 1)
            ret = (ret * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return (ret);
}

unsigned long long
    my_atoll(char *str)
{
    unsigned long long ret;

    ret = 0;
    while (*str >= '\t' && *str <= '\r')
        str++;
    if (*str == '-')
        return (0);
    while (*str >= '0' && *str <= '9')
    {
        ret = ret * 10 + *str - '0';
        str++;
    }
    return (ret);
}

unsigned int
    ft_strlen(char *str)
{
    unsigned int len;

    len = 0;
    while (str[len] != '\0')
        len++;
    return (len);
}

void
    assigment_func(char *buffer, unsigned long long *value, char status)
{
    unsigned int        count;
    unsigned long long  ret;

    // Değer oku
    if (status == 'p')
        my_write(1, "p: ", 3);
    else if (status == 'g')
        my_write(1, "g: ", 3);
    count = my_read(0, buffer, 128);
    buffer[count] = '\0';
    *value = my_atoll(buffer);
}

void
    ull_to_str(unsigned long long value, char *str)
{
    char    *p;
    char    *p1;
    char    *p2;
    unsigned long long temp_value;

    p = str;
    // Sayıyı karakter dizisine çevir
    do
    {
        temp_value = value;
        value /= 10;
        *p++ = "0123456789"[temp_value - value * 10];
    } while (value);

    // Sonlandırıcı karakter
    *p = '\0';

    // Diziyi ters çevir
    p1 = str;
    p2 = p - 1;
    while (p1 < p2)
    {
        char temp = *p1;
        *p1 = *p2;
        *p2 = temp;
        p1++;
        p2--;
    }
}

void
    diffie_hellman_key_exchange(int fd)
{
    unsigned long long  p;
    unsigned long long  g;
    unsigned long long  a;
    unsigned long long  b;
    unsigned long long  A;
    unsigned long long  B;
    unsigned long long  s;
    char                *buffer;
    char                str_buffer[21]; // 20 basamak + null karakter

    buffer = (char *)my_malloc(128);
    if (buffer == (char *)0)
    {
        my_write(fd, "Bellek tahsisi basarisiz oldu\n", ft_strlen("Bellek tahsisi basarisiz oldu\n"));
        return ;
    }
    assigment_func(buffer, &p, 'p');
    assigment_func(buffer, &g, 'g');

    if (p <= 1 || g <= 1)
    {
        my_write(fd, "p ve g degerleri 1'den buyuk olmali\n", ft_strlen("p ve g degerleri 1'den buyuk olmali\n"));
        my_free((void **)&buffer);
        return ;
    }
    // Rastgele a ve b gizli anahtarlarını oluştur
    a = my_rand_in_range(p - 1) + 1;
    b = my_rand_in_range(p - 1) + 1;

    // A ve B değerlerini hesapla
    A = mod_exp(g, a, p);
    B = mod_exp(g, b, p);

    // A ve B değerlerini karşı tarafa gönder
    my_write(fd, "A=", 2);
    ull_to_str(A, str_buffer);
    my_write(fd, str_buffer, ft_strlen(str_buffer));
    my_write(fd, "\n", 1);

    my_write(fd, "B=", 2);
    ull_to_str(B, str_buffer);
    my_write(fd, str_buffer, ft_strlen(str_buffer));
    my_write(fd, "\n", 1);

    // S değerini hesapla (paylasilan gizli anahtar)
    s = mod_exp(B, a, p);
    my_write(fd, "Gizli anahtar s=", ft_strlen("Gizli anahtar s="));
    ull_to_str(s, str_buffer);
    my_write(fd, str_buffer, ft_strlen(str_buffer));
    my_write(fd, "\n", 1);

    // Bellek tahsisini serbest bırak
    my_free((void **)&buffer);
}

int
    main(int argc, char *argv[])
{
    int fd;
    // Dosya aç
    //fd = my_open("/dev/tty", O_RDWR);
    fd = 1;
    // Diffie-Hellman anahtar değişimini gerçekleştir
    diffie_hellman_key_exchange(fd);
    // Dosyayı kapat
    //my_close(fd);
    return (0);
}
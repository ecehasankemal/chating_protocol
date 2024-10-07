#include "../../inc/decentralized_chat.h"

/*
    RSA algoritması, asal sayıların çarpanlara ayrılmasının zorluğuna dayanır.
    Bu algoritma, açık anahtarlı şifreleme ve dijital imza oluşturma için kullanılır.

    Asimetrik şifreleme, mesajların yalnızca hedeflenen kişi tarafından çözülebilmesi
    için kullanılan temel yapı taşlarından biridir. İki ana anahtar vardır: genel
    anahtar (herkesle paylaşılabilir) ve özel anahtar (gizli tutulur).

    Anahtar üretimi:
        İki büyük asal sayı seçilir: p ve q
        n = p * q hesaplanır
        φ(n) = (p - 1) * (q - 1) hesaplanır euler totemi
        e (genel anahtar bileşeni) seçin, bu 1<e<ϕ(n)1<e<ϕ(n) olacak ve gcd(e,ϕ(n))=1gcd(e,ϕ(n))=1 şartını sağlayacak.
        d (özel anahtar bileşeni) seçin, bu e * d ≡ 1 (mod ϕ(n))e∗d≡1(modϕ(n)) olacak.

    Şifreleme:
        C = M^e mod n
        Burada C şifreli metin, M açık metin, e genel anahtar ve n modülüdür.
    Şifre çözme:
        M = C^d mod n
        Burada M açık metin, C şifreli metin, d özel anahtar ve n modülüdür.

    NOT: Rsa, özellikle büyük sayılarla çalışırken çok yavaş olabilir.
         Bu yüzden genellikle simetrik şifreleme ile birlikte kullanılır.

    Eliptik eğri kriptografisi (ECC), RSA'dan daha güvenli ve daha hızlıdır.
    ECC, RSA'ya kıyasla daha kısa anahtarlarla daha güvenli bir şifreleme sağlar.
    ECC'yi sıfırdan yazmak, matematiksel olarak eliptik eğrilerin yapısını ve
    bu eğriler üzerindeki grup işlemlerini doğru bir şekilde anlamayı gerektirir.
    ECC algoritmaları genellikle aşağıdaki adımları içerir: 

    Eliptik eğri denklemi: y^2 = x^3 + ax + b (mod p)
    Anahtar oluşturma: Bir özel anahtar d ve buna karşılık gelen genel anahtar Q = d * G
    hesaplanır (burada G, eğri üzerindeki bir noktadır).
    Şifreleme: Bir rastgele sayı k seçilir ve şifreli metin C = k * G ve P = k * Q
    Anahtar değişimi: Diffie-Hellman algoritması kullanılarak güvenli bir ortak anahtar elde edilir.

*/

// RSA için gerekli fonksiyonlar
long long mod_exp(long long base, long long exp, long long mod);
long long gcd(long long a, long long b);
long long mod_inv(long long e, long long phi);
long long rsa_encrypt(long long message, long long e, long long n);
long long rsa_decrypt(long long cipher, long long d, long long n);

// ECC için gerekli fonksiyonlar
typedef struct
{
    long long x;
    long long y;
}       EC_Point;

EC_Point ec_add(EC_Point p, EC_Point q, long long a, long long p_mod);
EC_Point ec_double(EC_Point p, long long a, long long p_mod);
EC_Point ec_scalar_mult(EC_Point p, long long k, long long a, long long p_mod);

void my_print(const char *str) {
    my_write(1, str, my_strlen((char *)str)); // 1, standart çıktı
}

// tamsayıyı string'e çeviren basit bir fonksiyon
#include <stdlib.h>

static unsigned int
    ft_numsize(int n, int base)
{
    unsigned int len;

    if (n == 0)
        return (1);
    len = 0;
    if (n < 0 && base == 10) // Negatif sayılar sadece 10 tabanında işlenir
        len += 1;
    while (n != 0)
    {
        n /= base;
        len++;
    }
    return (len);
}

char
    *ft_itoa_base(int n, int base)
{
    char *str;
    unsigned int num;
    unsigned int len;
    const char *digits = "0123456789ABCDEF"; // 16 tabanı için karakter seti

    // Geçerli bir taban kontrolü
    if (base < 2 || base > 16)
        return (NULL);

    len = ft_numsize(n, base);
    str = (char *)malloc(sizeof(char) * (len + 1));
    if (str == NULL)
        return (NULL);
    
    if (n < 0 && base == 10)
    {
        str[0] = '-';
        num = -n; // Negatif sayılar için pozitif karşılığını al
    }
    else
        num = (unsigned long long)n;

    str[len] = '\0'; // String'in sonlandırılması

    // Sayının belirtilen tabanda string'e dönüştürülmesi
    while (num > 0)
    {
        str[len - 1] = digits[num % base];
        num = num / base;
        len--;
    }

    // Eğer n sıfır ise, sonucu '0' olarak ayarla
    if (n == 0)
        str[0] = '0';

    return (str);
}


void
    rsa_eliptik_esir_kriptografisi(void)
{
    // RSA anahtarları
    long long p = 61; // Büyük asal sayı
    long long q = 53; // Büyük asal sayı
    long long n = p * q;
    long long phi = (p - 1) * (q - 1);
    long long e = 17; // Genel anahtar bileşeni
    long long d = mod_inv(e, phi); // Özel anahtar bileşeni

    // Şifreleme ve şifre çözme örneği (RSA)
    long long message = 42;
    
    // Açık metni string formatına çevir ve yaz
    char *msg_str = ft_itoa_base(message, 10);
    my_write(1, "Açık metin (RSA): ", my_strlen("Açık metin (RSA): "));
    my_write(1, msg_str, my_strlen(msg_str));
    my_write(1, "\n", 1);
    free(msg_str);

    long long cipher = rsa_encrypt(message, e, n);
    
    // Şifreli metni string formatına çevir ve yaz
    char *cipher_str = ft_itoa_base(cipher, 10);
    my_write(1, "Şifreli metin (RSA): ", my_strlen("Şifreli metin (RSA): "));
    my_write(1, cipher_str, my_strlen(cipher_str));
    my_write(1, "\n", 1);
    free(cipher_str);

    long long decrypted_message = rsa_decrypt(cipher, d, n);
    
    // Çözülmüş metni string formatına çevir ve yaz
    char *decrypted_str = ft_itoa_base(decrypted_message, 10);
    my_write(1, "Çözülen metin (RSA): ", my_strlen("Çözülen metin (RSA): "));
    my_write(1, decrypted_str, my_strlen(decrypted_str));
    my_write(1, "\n\n", 2);
    free(decrypted_str);

    // ECC Parametreleri
    long long a = 2; // Eliptik eğri sabitleri
    long long p_mod = 97;   // Büyük asal sayı (modulus)
    EC_Point G = {3, 6};    // Temel nokta
    long long k = 10;       // Rastgele bir sayı
    EC_Point public_key = ec_scalar_mult(G, k, a, p_mod); // Genel anahtar

    // Şifreleme ve şifre çözme örneği (ECC)
    EC_Point cipher1 = ec_scalar_mult(G, k, a, p_mod);    // C1 = k * G
    EC_Point cipher2 = ec_scalar_mult(public_key, k, a, p_mod); // C2 = k * Q

    // Şifreli metni ECC için yaz
    char *c1_x_str = ft_itoa_base(cipher1.x, 10);
    char *c1_y_str = ft_itoa_base(cipher1.y, 10);
    char *c2_x_str = ft_itoa_base(cipher2.x, 10);
    char *c2_y_str = ft_itoa_base(cipher2.y, 10);
    
    my_write(1, "Şifreli metin (ECC): C1(", my_strlen("Şifreli metin (ECC): C1("));
    my_write(1, c1_x_str, my_strlen(c1_x_str));
    my_write(1, ", ", 2);
    my_write(1, c1_y_str, my_strlen(c1_y_str));
    my_write(1, "), C2(", 5);
    my_write(1, c2_x_str, my_strlen(c2_x_str));
    my_write(1, ", ", 2);
    my_write(1, c2_y_str, my_strlen(c2_y_str));
    my_write(1, ")\n", 2);

    // Belleği serbest bırak
    free(c1_x_str);
    free(c1_y_str);
    free(c2_x_str);
    free(c2_y_str);
}

long long
    mod_exp(long long base, long long exp, long long mod)
{
    long long ret;

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

long long
    gcd(long long a, long long b)
{
    long long temp;

    while (b != 0)
    {
        temp = b;
        b = a % b;
        a = temp;
    }
    return (a);
}

long long
    mod_inv(long long e, long long phi)
{
    long long t;
    long long new_t;
    long long r;
    long long new_r;

    t = 0;
    new_t = 1;
    r = phi;
    new_r = e;
    while (new_r != 0)
    {
        long long quotient = r / new_r;
        long long temp = new_r;
        new_r = r - quotient * new_r;
        r = temp;
        temp = new_t;
        new_t = t - quotient * new_t;
        t = temp;
    }
    if (r > 1)
        return (-1);
    if (t < 0)
        t += phi;
    return (t);
}

long long
    rsa_encrypt(long long message, long long e, long long n)
{
    return (mod_exp(message, e, n));
}

long long
    rsa_decrypt(long long cipher, long long d, long long n)
{
    return (mod_exp(cipher, d, n));
}

EC_Point
    ec_add(EC_Point p, EC_Point q, long long a, long long p_mod)
{
    EC_Point    r = {0, 0};
    long long   s;

    if (p.x == q.x && p.y == q.y)
        return (ec_double(p, a, p_mod));
    if (p.x == q.x)
        return (r);
    s = ((q.y - p.y) * mod_inv(q.x - p.x, p_mod)) % p_mod;
    r.x = (s * s - p.x - q.x) % p_mod;
    r.y = (s * (p.x - r.x) - p.y) % p_mod;

    return (r);
}

EC_Point
    ec_double(EC_Point p, long long a, long long p_mod)
{
    EC_Point    r = {0, 0};
    long long   s;

    if (p.y == 0)
        return (r);

    s = ((3 * p.x * p.x + a) * mod_inv(2 * p.y, p_mod)) % p_mod;
    r.x = (s * s - 2 * p.x) % p_mod;
    r.y = (s * (p.x - r.x) - p.y) % p_mod;

    return (r);
}

EC_Point
    ec_scalar_mult(EC_Point p, long long k, long long a, long long p_mod)
{
    EC_Point    r;
    EC_Point    q;
    long long   index;

    index = 0;
    r.x = 0;
    r.y = 0;
    q = p;
    while (index < k)
    {
        r = ec_add(r, q, a, p_mod);
        index++;
    }
    return (r);
}

int
    main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    rsa_eliptik_esir_kriptografisi();
    return (0);
}
#ifndef DECENTRALIZED_CHAT_H
# define DECENTRALIZED_CHAT_H

# include <stdlib.h>

extern long my_write(int fd, const void *buf, unsigned long count);
extern long my_read(int fd, const void *buf, unsigned long count);
extern int  my_strlen(char *str);


#endif


/*void
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
    printf("Açık metin (RSA): %lld\n", message);
    long long cipher = rsa_encrypt(message, e, n);
    printf("Şifreli metin (RSA): %lld\n", cipher);
    long long decrypted_message = rsa_decrypt(cipher, d, n);
    printf("Çözülen metin (RSA): %lld\n\n", decrypted_message);

    // ECC Parametreleri
    long long a = 2; // Eliptik eğri sabitleri
    long long p_mod = 97;   // Büyük asal sayı (modulus)
    EC_Point G = {3, 6};    // Temel nokta
    long long k = 10;       // Rastgele bir sayı
    EC_Point public_key = ec_scalar_mult(G, k, a, p_mod); // Genel anahtar

    // Şifreleme ve şifre çözme örneği (ECC)
    EC_Point cipher1 = ec_scalar_mult(G, k, a, p_mod);    // C1 = k * G
    EC_Point cipher2 = ec_scalar_mult(public_key, k, a, p_mod); // C2 = k * Q
    printf("Şifreli metin (ECC): C1(%lld, %lld), C2(%lld, %lld)\n", cipher1.x, cipher1.y, cipher2.x, cipher2.y);
}*/
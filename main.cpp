#include <cstring>
#include <fstream>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <vector>

#define AES_KEYLEN 32 // AES 256-bit
#define AES_BLOCK_SIZE 16
#define AES_IVLEN 16 // AES IV 128-bit
#define BUFFER_SIZE 4096

void generate_key_iv(unsigned char *key, unsigned char *iv)
{
    RAND_bytes(key, AES_KEYLEN);
    RAND_bytes(iv, AES_IVLEN);
}

bool encrypt_file(const char *in_file, const char *out_file, unsigned char *key, unsigned char *iv)
{
    std::ifstream fin(in_file, std::ios::binary);
    std::ofstream fout(out_file, std::ios::binary);

    if (!fin || !fout)
    {
        std::cerr << "Error opening file!" << std::endl;
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    std::vector<unsigned char> inbuf(BUFFER_SIZE), outbuf(BUFFER_SIZE + AES_BLOCK_SIZE);
    int out_len;

    while (fin.good())
    {
        fin.read(reinterpret_cast<char *>(inbuf.data()), inbuf.size());
        int read_len = fin.gcount();

        if (!EVP_EncryptUpdate(ctx, outbuf.data(), &out_len, inbuf.data(), read_len))
        {
            std::cerr << "1.Encryption failed!" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        fout.write(reinterpret_cast<char *>(outbuf.data()), out_len);
    }

    if (!EVP_EncryptFinal_ex(ctx, outbuf.data(), &out_len))
    {
        std::cerr << "Final encryption step failed!" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    fout.write(reinterpret_cast<char *>(outbuf.data()), out_len);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool decrypt_file(const char *in_file, const char *out_file, unsigned char *key, unsigned char *iv)
{
    std::ifstream fin(in_file, std::ios::binary);
    std::ofstream fout(out_file, std::ios::binary);

    if (!fin || !fout)
    {
        std::cerr << "Error opening file!" << std::endl;
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    std::vector<unsigned char> inbuf(BUFFER_SIZE + AES_BLOCK_SIZE), outbuf(BUFFER_SIZE);
    int out_len;

    while (fin.good())
    {
        fin.read(reinterpret_cast<char *>(inbuf.data()), inbuf.size());
        int read_len = fin.gcount();

        if (!EVP_DecryptUpdate(ctx, outbuf.data(), &out_len, inbuf.data(), read_len))
        {
            std::cerr << "Decryption failed!" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            return false;
        }
        fout.write(reinterpret_cast<char *>(outbuf.data()), out_len);
    }

    if (!EVP_DecryptFinal_ex(ctx, outbuf.data(), &out_len))
    {
        std::cerr << "Final decryption step failed! Key or IV might be incorrect." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    fout.write(reinterpret_cast<char *>(outbuf.data()), out_len);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

int main()
{
    unsigned char key[AES_KEYLEN];
    unsigned char iv[AES_IVLEN];

    generate_key_iv(key, iv);

    if (encrypt_file("django00.pdf", "encrypted.enc", key, iv))
    {
        std::cout << "Encryption successful!" << std::endl;
    }
    else
    {
        std::cerr << "2.Encryption failed!" << std::endl;
        return 1;
    }

    if (decrypt_file("encrypted.enc", "decrypted.pdf", key, iv))
    {
        std::cout << "Decryption successful!" << std::endl;
    }
    else
    {
        std::cerr << "Decryption failed!" << std::endl;
        return 1;
    }

    return 0;
}
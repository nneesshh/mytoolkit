#ifndef _TINY_AES_128_H_
#define _TINY_AES_128_H_

#include <stdint.h>

/* */
int		AES128_encrypt(char *buf, unsigned int len, const uint8_t *key);
int		AES128_decrypt(char *buf, unsigned int len, const uint8_t *key);

// CBC enables AES128 encryption in CBC-mode of operation and handles 0-padding.
// ECB enables the basic ECB 16-byte block algorithm. Both can be enabled simultaneously.

void	AES128_ECB_encrypt(uint8_t* input, const uint8_t* key, uint8_t *output);
void	AES128_ECB_decrypt(uint8_t* input, const uint8_t* key, uint8_t *output);

void	AES128_CBC_encrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);
void	AES128_CBC_decrypt_buffer(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);

void	AES128_bytes_to_hex_str(const uint8_t *in, size_t size, char *out);
void	AES128_hex_str_to_bytes(const char *in, size_t size, uint8_t *out);

#endif //_TINY_AES_128_H_

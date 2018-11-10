#ifndef TEA_H
#define TEA_H

/*
*for htonl,htonl
*do remember link "ws2_32.lib"
*/
#include <winsock2.h>

const int   SIZE_IN=8;
const int   SIZE_OUT=8;
const int   SIZE_KEY=16;
 
class CTEA {
public:
    CTEA(const char *key, int round = 32, bool is_net_byte = false);
    CTEA(const CTEA &rhs);
    CTEA& operator=(const CTEA &rhs);

	bool Encrypt(char *pBuf, unsigned int uLen);
	bool Decrypt(char *pBuf, unsigned int uLen);

private:
    void EncryptLongLong(const unsigned long *in, unsigned long *out);
    void DecryptLongLong(const unsigned long *in, unsigned long *out);

    unsigned long ntoh(unsigned long netlong) { return _is_net_byte ? ntohl(netlong) : netlong; }
    unsigned long hton(unsigned long hostlong) { return _is_net_byte ? htonl(hostlong) : hostlong; }

    int _round; //iteration round to encrypt or decrypt
    bool _is_net_byte; //whether input unsigned chars come from network
    unsigned char _key[16]; //encrypt or decrypt key
};

#endif/*TEA_H*/

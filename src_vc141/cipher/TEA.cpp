#include "TEA.h"
#include <cstring> //for memcpy,memset
#include <string>
#include <vector>
#include <cmath>
 
static const std::string g_keyStr("3A DA 75 21 DB E3 DB B3 62 B7 49 01 A5 C6 EA D5");

static inline double
logbase(double base, double x)
{
	return log(x)/log(base);
}

static char
int_to_hex_char(int x)
{
	static const char HEX[16] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'A', 'B',
		'C', 'D', 'E', 'F'
	};
	return HEX[x];
}

static int 
hex_char_to_int(char hex)
{

	int nHex = toupper(hex);//将字符转换为大写英文字母

	if (isdigit(nHex))
		return (hex - '0');
	if (isalpha(nHex))
		return (hex - 'A' + 10);
	return 0;
}

static std::string
bytes_to_hex_string(const byte *in, size_t size) {
	std::string str;
	for (size_t i = 0; i < size; ++i) {
		int t = in[i];
		int a = t / 16;
		int b = t % 16;
		str.append(1, int_to_hex_char(a));
		str.append(1, int_to_hex_char(b));
		if (i != size - 1)
			str.append(1, ' ');
	}
	return str;
}

static unsigned int
hex_string_to_bytes(const std::string &str, byte *out) 
{
	int nErrorNum=0;
	try
	{
		std::vector<std::string> vec;

		nErrorNum++;

		std::string::size_type currPos = 0, prevPos = 0;
		while ((currPos = str.find(' ', prevPos)) != std::string::npos) {
			std::string b(str.substr(prevPos, currPos - prevPos));
			vec.push_back(b);
			prevPos = currPos + 1;
		}

		nErrorNum++;

		if (prevPos < str.size()) {
			std::string b(str.substr(prevPos));
			vec.push_back(b);
		}

		nErrorNum++;

		typedef std::vector<std::string>::size_type sz_type;
		sz_type size = vec.size();

		nErrorNum++;

		for (sz_type i = 0; i < size; ++i) {
			int a = hex_char_to_int(vec[i][0]);
			int b = hex_char_to_int(vec[i][1]);
			out[i] = a * 16 + b;

			nErrorNum++;
		}

		nErrorNum++;

		return size;
	}
	catch(...)//ADD 2009-6-7 BY ZM
	{
		char temp[128] = {0};
		//o_snprintf(temp, sizeof(temp), "Error: Util::hexStringToBytes(),出现异常,nErrorNum:%d！\r\n", nErrorNum);
		//OutputDebugString(temp);
	}

	return 0;  
}

/**********************************************
1.key - 加密或解密用的128-bit(16byte)密钥。
2.round - 加密或解密的轮数，常用的有64，32，16。
3.is_net_byte - 用来标记待处理的字节是不是来自网络，为true时在加密/解密前先要转换成本地字节，执行加密/解密，然后再转换回网络字节。
***********************************************/
CTEA::CTEA(const char *key, int round /*= 32*/, bool is_net_byte /*= false*/)
:_round(round)
,_is_net_byte(is_net_byte)
{
    if (key != 0)
       memcpy(_key, key, 16);
    else
        memset(_key, 0, 16);
}

CTEA::CTEA(const CTEA &rhs)
:_round(rhs._round)
,_is_net_byte(rhs._is_net_byte) {
    memcpy(_key, rhs._key, 16);
}

CTEA& CTEA::operator=(const CTEA &rhs) {
    if (&rhs != this) {
         _round = rhs._round;
        _is_net_byte = rhs._is_net_byte;
        memcpy(_key, rhs._key, 16);
    }
    return *this;
}

/*TEA加密*/
bool
CTEA::Encrypt(char *pBuf, unsigned int uLen)
{
	if (nullptr == pBuf || uLen == 0)
		return false;

	byte plain[SIZE_IN]={0};
	byte encrypt[SIZE_OUT]={0};
	byte key[SIZE_KEY]={0};

	unsigned int uEnSize=uLen;
	for(unsigned int i=0;i<uEnSize;i=i+SIZE_IN)
	{
		memset(plain,0,SIZE_IN);
		memset(encrypt,0,SIZE_OUT);

		int nt=(uEnSize-i>SIZE_IN)?SIZE_IN:uEnSize-i;
		if(nt==SIZE_IN)
		{
			memcpy(plain,(byte*)pBuf+i,nt);
			EncryptLongLong((const unsigned long *)plain, (unsigned long *)encrypt);/*加密*/
		}
		else
		{
			memcpy(encrypt,(byte*)pBuf+i,nt);
		}

		memcpy(pBuf+i,encrypt,nt);
	}
	return true;
}

/*TEA解密*/
bool
CTEA::Decrypt(char *pBuf, unsigned int uLen)
{
	if(nullptr == pBuf || uLen==0)
		return false;

	byte plain[SIZE_IN]={0};
	byte crypt[SIZE_OUT]={0};
	byte key[SIZE_KEY]={0};

	for(unsigned int i=0;i<uLen;i=i+SIZE_IN)
	{
		memset(plain,0,SIZE_IN);
		memset(crypt,0,SIZE_OUT);

		int nt=(uLen-i>SIZE_IN)?SIZE_IN:uLen-i;
		if(nt==SIZE_IN)
		{
			memcpy(plain,pBuf+i,nt);
			DecryptLongLong((const unsigned long *)plain, (unsigned long *)crypt);/*解密*/
		}
		else
		{
			memcpy(crypt,pBuf+i,nt);
		}

		memcpy(pBuf+i,crypt,nt);
	}
	return true;
}

void CTEA::EncryptLongLong(const unsigned long *in, unsigned long *out) {

   unsigned long *k = (unsigned long*)_key;
   register unsigned long y = ntoh(in[0]);
   register unsigned long z = ntoh(in[1]);

   register unsigned long a = ntoh(k[0]);
   register unsigned long b = ntoh(k[1]);
   register unsigned long c = ntoh(k[2]);
   register unsigned long d = ntoh(k[3]);
   register unsigned long delta = 0x9E3779B9; /* (sqrt(5)-1)/2*2^32 */
   register int round = _round;
   register unsigned long sum = 0;

   while (round--) {    /* basic cycle start */
        sum += delta;
        y += ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
        z += ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
   }    /* end cycle */
   out[0] = ntoh(y);
   out[1] = ntoh(z);
}

void CTEA::DecryptLongLong(const unsigned long *in, unsigned long *out) {

    unsigned long *k = (unsigned long*)_key;
    register unsigned long y = ntoh(in[0]);
    register unsigned long z = ntoh(in[1]);
    register unsigned long a = ntoh(k[0]);
    register unsigned long b = ntoh(k[1]);
    register unsigned long c = ntoh(k[2]);
    register unsigned long d = ntoh(k[3]);
    register unsigned long delta = 0x9E3779B9; /* (sqrt(5)-1)/2*2^32 */
    register int round = _round;
    register unsigned long sum = 0;

   if (round == 32)
       sum = 0xC6EF3720; /* delta << 5*/
   else if (round == 16)
       sum = 0xE3779B90; /* delta << 4*/
   else
       sum = delta << static_cast<int>(logbase(2, round));

   while (round--) {    /* basic cycle start */
       z -= ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
       y -= ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
       sum -= delta;
   }    /* end cycle */
   out[0] = ntoh(y);
   out[1] = ntoh(z);
}

#pragma once
//------------------------------------------------------------------------------
/**
@class CStringHash

(C) 2016 n.lee
*/

// Definition of MPQHashTable
struct MPQHashTable {
	long nHashA;
	long nHashB;
	bool bExists;
};

#define   CRYPT_TABLE_LENGTH  (256 * 5)
#define   MAX_TABLE_LENGTH    1024

//------------------------------------------------------------------------------
/**
@brief CStringHash
*/
class CStringHash {
public:
	CStringHash(unsigned long tableLength = MAX_TABLE_LENGTH);
	~CStringHash();

	bool						Hash(const char *str);
	unsigned long				Hashed(const char *str);
	unsigned long				GetHashTableLength();

	unsigned long				OneWayHash(const char *str, unsigned long hashType);

	/*
		RS Hash Function
	*/
	unsigned int				RSHash(char* str, unsigned int len) {
		unsigned int b = 378551;
		unsigned int a = 63689;
		unsigned int hash = 0;
		unsigned int i = 0;
		for (i = 0; i < len; ++str, ++i) {
			hash = hash * a + (*str);
			a = a * b;
		}
		return hash;
	}

	/*
		JS Hash Function
	*/
	unsigned int				JSHash(char* str, unsigned int len) {
		unsigned int hash = 1315423911;
		unsigned int i = 0;
		for (i = 0; i < len; ++str, ++i) {
			hash ^= ((hash << 5) + (*str) + (hash >> 2));
		}
		return hash;
	}

	/*
		P. J. Weinberger Hash Function
	*/
	unsigned int				PJWHash(char* str, unsigned int len) {
		const unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);
		const unsigned int ThreeQuarters = (unsigned int)((BitsInUnsignedInt * 3) / 4);
		const unsigned int OneEighth = (unsigned int)(BitsInUnsignedInt / 8);
		const unsigned int HighBits = (unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
		unsigned int hash = 0;
		unsigned int test = 0;
		unsigned int i = 0;
		for (i = 0; i < len; ++str, ++i) {
			hash = (hash << OneEighth) + (*str);
			if ((test = hash & HighBits) != 0) {
				hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
			}
		}
		return hash;
	}

	/*
		ELF Hash Function is widely used in unix system
	*/
	unsigned int				ELFHash(char* str, unsigned int len) {
		unsigned int hash = 0;
		unsigned int x = 0;
		unsigned int i = 0;
		for (i = 0; i < len; ++str, ++i) {
			hash = (hash << 4) + (*str);
			if ((x = hash & 0xF0000000L) != 0) {
				hash ^= (x >> 24);
			}
			hash &= ~x;
		}
		return hash;
	}

	/*
		BKDR Hash Function
	*/
	unsigned int				BKDRHash(char* str, unsigned int len) {
		unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
		unsigned int hash = 0;
		unsigned int i = 0;
		for (i = 0; i < len; ++str, ++i) {
			hash = (hash * seed) + (*str);
		}
		return hash;
	}

	/*
		SDBM Hash Function
	*/
	unsigned int				SDBMHash(char* str, unsigned int len) {
		unsigned int hash = 0;
		unsigned int i = 0;
		for (i = 0; i < len; ++str, ++i) {
			hash = (*str) + (hash << 6) + (hash << 16) - hash;
		}
		return hash;
	}

	/*
		DJB hash function -- also named as "Times33", used by Perl, Berkeley DB, Apache, MFC, STL etc.
		
		DJB Hash function is
			h(0) = 5381
			h(i) = 33 * h(i-1) ^ str[i]
	*/
	unsigned int				DJBHash(char *str, unsigned int len) {
		unsigned int hash = 5381;
		unsigned int i = 0;
		for (i = 0; i < len; ++str, ++i) {
			hash = ((hash << 5) + hash) + (*str); /* times 33 */
		}
		hash &= ~(1 << 31); /* strip the highest bit */
		return hash;
	}

	/*
		DEK Hash Function -- see Knuth and <<The Art of Computer Programming>> volume 3¡£
	*/
	unsigned int				DEKHash(char* str, unsigned int len) {
		unsigned int hash = len;
		unsigned int i = 0;
		for (i = 0; i < len; ++str, ++i) {
			hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
		}
		return hash;
	}

	/*
		BP Hash Function
	*/
	unsigned int				BPHash(char* str, unsigned int len) {
		unsigned int hash = 0;
		unsigned int i = 0;
		for (i = 0; i < len; ++str, ++i) {
			hash = hash << 7 ^ (*str);
		}
		return hash;
	}

	/*
		FNV Hash Function
	*/
	unsigned int				FNVHash(char* str, unsigned int len) {
		const unsigned int fnv_prime = 0x811C9DC5;
		unsigned int hash = 0;
		unsigned int i = 0;
		for (i = 0; i < len; ++str, ++i) {
			hash *= fnv_prime;
			hash ^= (*str);
		}
		return hash;
	}

	/*
		AP Hash Function
	*/
	unsigned int				APHash(char* str, unsigned int len) {
		unsigned int hash = 0xAAAAAAAA;
		unsigned int i = 0;
		for (i = 0; i < len; ++str, ++i) {
			hash ^= ((i & 1) == 0) ? ((hash << 7) ^ (*str) * (hash >> 3)) :
				(~((hash << 11) + (*str) ^ (hash >> 5)));
		}
		return hash;
	}

private:
	void						InitCryptTable();

private:
	unsigned long		_cryptTable[CRYPT_TABLE_LENGTH];
	unsigned long		_tableLength;
	MPQHashTable		*_pHashTable;
};

/*EOF*/
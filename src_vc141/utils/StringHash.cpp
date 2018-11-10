//------------------------------------------------------------------------------
//  StringHash.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "StringHash.h"
#include <ctype.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
/**

*/
CStringHash::CStringHash(unsigned long tableLength) {
	_tableLength = tableLength;
	_pHashTable = new MPQHashTable[_tableLength];
	InitCryptTable();

	for (unsigned long i = 0; i < _tableLength; ++i) {
		_pHashTable[i].nHashA = -1;
		_pHashTable[i].nHashB = -1;
		_pHashTable[i].bExists = false;
	}
}

//------------------------------------------------------------------------------
/**

*/
CStringHash::~CStringHash() {
	if (nullptr != _pHashTable) {
		delete[] _pHashTable;
		_pHashTable = nullptr;
		_tableLength = 0;
	}
}

//------------------------------------------------------------------------------
/**
// Initialize the crypt array
*/
void
CStringHash::InitCryptTable() {
	unsigned long seed = 0x00100001;
	int i;
	for (i = 0; i < 256; ++i) {
		for (int j = i, k = 0; k < 5; ++k, j += 256) {
			seed = (seed * 125 + 3) % 0x2AAAAB;
			unsigned long tmp = (seed & 0xFFFF) << 0x10;
			seed = (seed * 125 + 3) % 0x2AAAAB;
			_cryptTable[j] = tmp | (seed & 0xFFFF);
		}
	}
}

//------------------------------------------------------------------------------
/**
// The one-way hash function
*/
unsigned long
CStringHash::OneWayHash(const char *str, unsigned long hashType) {
	unsigned char* key = (unsigned char*)str;
	unsigned long seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE;
	int ch;

	while (*key != 0) {
		ch = toupper(*key++);
		seed1 = _cryptTable[hashType << 8] ^ (seed1 + seed2);
		seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
	}

	return seed1;
}

//------------------------------------------------------------------------------
/**
// Hash a string to the hash table
*/
bool
CStringHash::Hash(const char *str) {
	const unsigned long HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;
	unsigned long nHash = OneWayHash(str, HASH_OFFSET);
	unsigned long nHashA = OneWayHash(str, HASH_A);
	unsigned long nHashB = OneWayHash(str, HASH_B);
	unsigned long nHashStart = nHash % _tableLength;
	unsigned long nHashPos = nHashStart;

	while (_pHashTable[nHashPos].bExists) {
		nHashPos = (nHashPos + 1) % _tableLength;
		if (nHashPos == nHashStart) {
			return false;
		}
	}

	_pHashTable[nHashPos].bExists = true;
	_pHashTable[nHashPos].nHashA = nHashA;
	_pHashTable[nHashPos].nHashB = nHashB;

	return true;
}

//------------------------------------------------------------------------------
/**
// Find a string in the hash table, if it exists then return
// the index of the string, else return -1
*/
unsigned long
CStringHash::Hashed(const char *str) {
	const unsigned long HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;
	unsigned long nHash = OneWayHash(str, HASH_OFFSET);
	unsigned long nHashA = OneWayHash(str, HASH_A);
	unsigned long nHashB = OneWayHash(str, HASH_B);
	unsigned long nHashStart = nHash % _tableLength;
	unsigned long nHashPos = nHashStart;

	while (_pHashTable[nHashPos].bExists) {
		if (_pHashTable[nHashPos].nHashA == nHashA && _pHashTable[nHashPos].nHashB == nHashB) {
			return nHashPos;
		}
		else {
			nHashPos = (nHashPos + 1) % _tableLength;
		}

		if (nHashPos == nHashStart) {
			break;
		}
	}
	return -1;
}

//------------------------------------------------------------------------------
/**

*/
unsigned long
CStringHash::GetHashTableLength() {
	return _tableLength;
}

/* -- EOF -- */
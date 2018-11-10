//------------------------------------------------------------------------------
//  LRandom.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "LRandom.h"

#include <vector>

static const char *alphanumeric[5] = {
	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
	, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
	, "0123456789"
	, "ABCDEFGHJKMNPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz"
	, "23456789ABCDEFGHJKMNPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz" };

std::string
CLRandom::GetRandomString(size_t len, int table_idx) {
	const char *table = alphanumeric[table_idx];
	std::vector<char> v(len);
	size_t sz = strlen(table);
	unsigned int i, index;
	for (i = 0; i < len; ++i) {
		index = Rand(0, sz);
		v[i] = table[index];
	}
	return std::string(v.begin(), v.end());
}

void
CLRandom::GetRandomIndex(int size, int num, std::vector<int>& out) {
	std::vector<int> vTmp;
	vTmp.reserve(256);
	int i;
	for (i = 0; i < size; ++i) {
		vTmp.push_back(i);
	}

	int nCount = 0;
	while (vTmp.size() > 0 && nCount < num) {
		int32_t nRand = GetRandomS32(0, vTmp.size() - 1);
		std::vector<int>::iterator it = vTmp.begin() + nRand;

		out.push_back(*it);
		++nCount;

		vTmp.erase(it);
	}
}

/* -- EOF -- */
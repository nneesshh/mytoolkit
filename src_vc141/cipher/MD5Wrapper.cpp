//------------------------------------------------------------------------------
//  MD5Wrapper.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "MD5Wrapper.h"

#include <algorithm>

//------------------------------------------------------------------------------
/**

*/
std::string
CMD5Wrapper::MDStringUpperCase(const char *sInput) {
	std::string sOutput = MDString(sInput);
	transform(sOutput.begin(), sOutput.end(), sOutput.begin(), toupper);
	return std::move(sOutput);
}

//------------------------------------------------------------------------------
/**

*/
std::string
CMD5Wrapper::MDOneTwoOutput(const char *sOne, const char *sTwo) {
	base::MD5Digest digest;
	base::MD5Init(&_ctx);
	base::MD5Update(&_ctx, base::StringPiece(sOne, strlen(sOne)));
	base::MD5Update(&_ctx, base::StringPiece(sTwo, strlen(sTwo)));
	base::MD5Final(&digest, &_ctx);
	return base::MD5DigestToBase16(digest);
}

//------------------------------------------------------------------------------
/**

*/
std::string
CMD5Wrapper::MDFileOutput(FILE *f) {
	unsigned char data[1024];
	unsigned int bytes;

	base::MD5Digest digest;
	base::MD5Init(&_ctx);

	while (0 != (bytes = (unsigned int)fread(data, 1, 1024, f)))
		base::MD5Update(&_ctx,
			base::StringPiece(reinterpret_cast<const char*>(data), bytes));
	base::MD5Final(&digest, &_ctx);
	return base::MD5DigestToBase16(digest);
}

/* -- EOF -- */
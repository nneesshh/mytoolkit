#pragma once

//------------------------------------------------------------------------------
/**
@class CMD5Wrapper

(C) 2016 n.lee
*/
#include "base/md5.h"

#include <string>
#include <stdint.h>
#include <stdio.h>

//------------------------------------------------------------------------------
/**
@brief CMD5Wrapper
*/
class CMD5Wrapper {
public:
	CMD5Wrapper() { }
	~CMD5Wrapper() { }

	///
	std::string					MDString(const char *sInput) {
		base::StringPiece input(sInput);
		return base::MD5String(input);
	}

	std::string					MDStringUpperCase(const char *sInput);
	std::string 				MDOneTwoOutput(const char *sOne, const char *sTwo);

	std::string					MDFileOutput(FILE *f);

private:
	base::MD5Context _ctx;
};

/*EOF*/
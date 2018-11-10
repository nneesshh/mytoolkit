#pragma once
//------------------------------------------------------------------------------
/**
@class CBase64Wrapper

(C) 2016 n.lee
*/
#include <string>

//------------------------------------------------------------------------------
/**
@brief CBase64Wrapper
*/
class CBase64Wrapper {
public:
	std::string Encode(const char *sInput, int nBytes);
	bool Decode(const char *sInput, int nBytes, std::string *sOutput);

};

/*EOF*/
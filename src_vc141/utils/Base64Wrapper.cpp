//------------------------------------------------------------------------------
//  Base64Wrapper.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "Base64Wrapper.h"

#include "../base/base64.h"

//------------------------------------------------------------------------------
/**

*/
std::string 
CBase64Wrapper::Encode(const char *sInput, int nBytes) {
	std::string sOutput;
	base::Base64Encode(base::StringPiece(sInput, nBytes), &sOutput);
	return std::move(sOutput);
}

//------------------------------------------------------------------------------
/**

*/
bool
CBase64Wrapper::Decode(const char *sInput, int nBytes, std::string *sOutput) {
	return base::Base64Decode(base::StringPiece(sInput, nBytes), sOutput);
}

/* -- EOF -- */
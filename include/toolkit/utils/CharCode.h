#pragma once

//------------------------------------------------------------------------------
/**
@class CCharCode

(C) 2016 n.lee
*/
#include <string>

#ifdef _WIN32
char * w2c(const wchar_t *wstr, char *cstr, int clenMax);
wchar_t * c2w(const char *cstr, wchar_t *wstr, int wlenMax);
#endif

//------------------------------------------------------------------------------
/**
@brief CCharCode
*/
class CCharCode {
public:
	static void UTF_8ToUnicode(wchar_t* pOut,char *pText);
	static void UnicodeToUTF_8(char* pOut,wchar_t* pText);

	static int	UnicodeToGB2312(char* pOut, int nOut, const wchar_t *pIn, int nIn);
	static int	Gb2312ToUnicode(wchar_t* pOut, int nOut, const char *pIn, int nIn);

	static void GB2312ToUTF_8(std::string& pOut,char *pText, int pLen);
	static void UTF_8ToGB2312(std::string &pOut, char *pText, int pLen);
};

/*EOF*/

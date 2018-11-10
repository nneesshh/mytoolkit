//------------------------------------------------------------------------------
//  Base64.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include <stdio.h>
#include "CharCode.h"

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN 1
# include <winsock2.h>
#endif

#ifdef _MSC_VER
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif
#endif

#ifdef _WIN32
char *
w2c(const wchar_t *wstr, char *cstr, int clenMax) {
	 if (wstr) {
		 int wlen = wcslen(wstr);
		 int nbytes = ::WideCharToMultiByte(
			 CP_UTF8,	// specify the code page used to perform the conversion
			 0,			// no special flags to handle unmapped characters
			 wstr,		// wide character string to convert
			 wlen,		// the number of wide characters in that string
			 NULL,		// no output buffer given, we just want to know how long it needs to be
			 0,
			 NULL,		// no replacement character given
			 NULL	    // we don't want to know if a character didn't make it through the translation
		 );

		 // make sure the buffer is big enough for this, making it larger if necessary
		 nbytes = (nbytes > clenMax - 1) ? (clenMax - 1) : nbytes;

		 ::WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, cstr, nbytes, NULL, NULL);
		 cstr[nbytes] = '\0';
		 return cstr;
	 }
	 return NULL;
}

wchar_t *
c2w(const char *cstr, wchar_t *wstr, int wlenMax) {
	if (cstr) {
		size_t clen = strlen(cstr);
		int nwords = ::MultiByteToWideChar(CP_UTF8, 0, (const char *)cstr, (int)clen, NULL, 0);
		nwords = (nwords > wlenMax - 1) ? (wlenMax - 1) : nwords;

		::MultiByteToWideChar(CP_UTF8, 0, (const char *)cstr, (int)clen, wstr, (int)nwords);
		wstr[nwords] = '\0';
		return wstr;
	}
	return NULL;
}
#endif

//------------------------------------------------------------------------------
/**

*/
void
CCharCode::UTF_8ToUnicode(wchar_t* pOut, char *pText) {
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);
}

//------------------------------------------------------------------------------
/**

*/
void
CCharCode::UnicodeToUTF_8(char* pOut, wchar_t* pText) {
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));
}

//------------------------------------------------------------------------------
/**

*/
int
CCharCode::UnicodeToGB2312(char* pOut, int nOut, const wchar_t *pIn, int nIn) {
	int n = ::WideCharToMultiByte(CP_ACP, NULL, pIn, nIn, pOut, nOut, NULL, NULL);
	if (n > 0)
		*(pOut + n) = 0;
	return n;
}

//------------------------------------------------------------------------------
/**

*/
int
CCharCode::Gb2312ToUnicode(wchar_t* pOut, int nOut, const char *pIn, int nIn) {
	int n = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pIn, nIn, pOut, nOut);
	if (n > 0)
		*(pOut + n) = 0;
	return n;
}

//------------------------------------------------------------------------------
/**

*/
void
CCharCode::GB2312ToUTF_8(std::string& pOut, char *pText, int pLen) {
	char buf[4];
	int nLength = pLen * 3;
	char* rst = new char[nLength];

	wchar_t wstr[2];

	memset(buf, 0, 4);
	memset(rst, 0, nLength);

	int i = 0;
	int j = 0;
	while (i < pLen) {
		if (*(pText + i) >= 0) {
			rst[j++] = pText[i++];
		}
		else {
			Gb2312ToUnicode(wstr, 1, pText + i, 2);
			UnicodeToUTF_8(buf, wstr);

			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j + 1] = buf[1];
			tmp = rst[j + 2] = buf[2];

			j += 3;
			i += 2;
		}
	}
	rst[j] = '\0';

	pOut = rst;
	delete[]rst;
}

//------------------------------------------------------------------------------
/**

*/
void
CCharCode::UTF_8ToGB2312(std::string &pOut, char *pText, int pLen) {
	char * newBuf = new char[pLen + 1];
	char Ctemp[4];
	memset(Ctemp, 0, 4);

	int i = 0;
	int j = 0;

	while (i < pLen) {
		if (pText[i] > 0) {
			newBuf[j++] = pText[i++];
		}
		else {
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp, pText + i);

			UnicodeToGB2312(Ctemp, 2, &Wtemp, sizeof(wchar_t));

			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];

			i += 3;
			j += 2;
		}
	}
	newBuf[j] = '\0';

	pOut = newBuf;
	delete[]newBuf;
}

/** -- EOF -- **/
//------------------------------------------------------------------------------
//  BadWordFilter.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "BadWordFilter.h"
#include "CharCode.h"

wchar_t *
wreplace(wchar_t *buffer, const wchar_t *dirty, const wchar_t ch) {
	int len = wcslen(dirty);
	wchar_t *p = buffer;
	int i;
	while (nullptr != (p = wcsstr(p, dirty))) {
		for (i = 0; i < len; ++i) {
			*p++ = L'*';
		}
	}
	return buffer;
}

//------------------------------------------------------------------------------
/**

*/
CBadWordFilter::CBadWordFilter() {

}

//------------------------------------------------------------------------------
/**

*/
CBadWordFilter::~CBadWordFilter() {

}

//------------------------------------------------------------------------------
/**

*/
void
CBadWordFilter::LoadFromFile(const char *sFile) {

	char chLine[1024];

	//
	FILE *fp = fopen(sFile, "r");
	if (fp) {
		while (nullptr != fgets(chLine, sizeof(chLine), fp)) {
			// remove tail '\r' and '\n'
			int nIndex = strlen(chLine);
			while (--nIndex > 0) {
				if ('\r' != chLine[nIndex] && '\n' != chLine[nIndex]) {
					break;
				}
				chLine[nIndex] = '\0';
			}

			//
			if (nIndex > 0) {
				AddWord(chLine);
			}
		}
		fclose(fp);
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CBadWordFilter::OnFilter(char *sInput) {
	int nLen = CCharCode::Gb2312ToUnicode(_wbuffer, 1024, sInput, strlen(sInput));
	bool bFiltered = false;

	int i;
	std::vector<std::wstring > *v;
	tWcharKeyMap::iterator itBadWord;
	for (i = 0; i < nLen; ++i) {
		//
		if (L'*' == *(_wbuffer + i) || L' ' == *(_wbuffer + i) || L'\t' == *(_wbuffer + i) || L'\0' == *(_wbuffer + i))
			continue;

		//
		itBadWord = _mapBadKey.find(*(_wbuffer + i));
		if (itBadWord == _mapBadKey.end())
			continue;

		//
		v = &(itBadWord->second);
		for (auto& it : (*v)) {
			wreplace(_wbuffer, it.c_str(), L'*');
			bFiltered = true;
		}
	}

	//
	CCharCode::UnicodeToGB2312(sInput, strlen(sInput), _wbuffer, nLen);
}

//------------------------------------------------------------------------------
/**

*/
void
CBadWordFilter::AddWord(const char *sWord) {
	wchar_t chBuffer[1024] = { 0 };
	int nLen = CCharCode::Gb2312ToUnicode(chBuffer, 1024, sWord, strlen(sWord));
	_badWordList.push_back(chBuffer);

	int i;
	std::vector<std::wstring> *v;
	wchar_t *p = chBuffer;
	for (i = 0; i < nLen; ++i) {

		if (L'*' == *(p + i) || L' ' == *(p + i) || L'\t' == *(p + i) || L'\0' == *(p + i))
			continue;

		v = &_mapBadKey[*(p + i)];

		std::vector<std::wstring>::iterator it = v->begin(), itEnd = v->end();
		while (it != itEnd) {
			if ((*it) == chBuffer) {
				v->erase(it);
				break;
			}

			++it;
		}

		v->push_back(chBuffer);
	}
}

/* -- EOF -- */
//------------------------------------------------------------------------------
//  NameCreator.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "NameCreator.h"

#include "../core/snprintf/mysnprintf.h"
#include "CharCode.h"

using namespace std;

//------------------------------------------------------------------------------
/**

*/
CNameCreator::CNameCreator()
	: _surnameWordList()
	, _maleWordList()
	, _femaleWordList() {
	LoadFromFile(_surnameWordList, "name_surname.txt");
	LoadFromFile(_femaleWordList, "name_female.txt");
	LoadFromFile(_maleWordList, "name_male.txt");
}

//------------------------------------------------------------------------------
/**

*/
CNameCreator::~CNameCreator() {

}

//------------------------------------------------------------------------------
/**

*/
void
CNameCreator::LoadFromFile(NAME_WORD_LIST& list, const char *sFile) {
	char chLine[1024];

	//
	FILE *fp = fopen(sFile, "r");
	if (fp) {
		while (nullptr != fgets(chLine, sizeof(chLine), fp)) {
			// remove tail '\r' and '\n'
			int nIndex = strlen(chLine);
			while (--nIndex>0) {
				if ('\r' != chLine[nIndex] && '\n' != chLine[nIndex]) {
					break;
				}
				chLine[nIndex] = '\0';
			}

			//
			if (nIndex > 0) {
				std::string utf8str;
				CCharCode::GB2312ToUTF_8(utf8str, chLine, strlen(chLine));
				list.emplace_back(utf8str);
			}
		}
		fclose(fp);
	}
}

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/**

*/
std::string
CNameCreator::CreateName(int nSex) {
	NAME_WORD_LIST *list1 = &_surnameWordList;
	NAME_WORD_LIST *list2 = nullptr;

	switch (nSex) {
	case 0:
		list2 = &_femaleWordList;
		break;

	case 1:
		list2 = &_maleWordList;
		break;

	default:
		return "ÎÞÃû";
	}

	if (list1->size() > 0 && list2->size() > 0) {
		char chFullname[1024];
		int nIndex1 = (int)(list1->size() * (float(rand()) / RAND_MAX));
		int nIndex2 = (int)(list2->size() * (float(rand()) / RAND_MAX));

		o_snprintf(chFullname, sizeof(chFullname), "%s%s", (*list1)[nIndex1].c_str(), (*list2)[nIndex2].c_str());
		return chFullname;
	}
	return "ØýÃû";
}

/* -- EOF -- */
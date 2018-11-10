//------------------------------------------------------------------------------
//  WordTable.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "WordTable.h"

#include "CharCode.h"

using namespace std;

//------------------------------------------------------------------------------
/**

*/
CWordTable::CWordTable() {

}

//------------------------------------------------------------------------------
/**

*/
CWordTable::~CWordTable() {

}

//------------------------------------------------------------------------------
/**

*/
void
CWordTable::LoadFromFile(const char *sFile) {
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
CWordTable::AddWord(const char *sWord) {
	_wordList.emplace_back(sWord);
}

//------------------------------------------------------------------------------
/**

*/
const char *
CWordTable::GetWordAt(size_t nIndex) {
	if (nIndex >= 0 && nIndex < GetWordCount()) {
		//
		return _wordList[nIndex].c_str();
	}
	return nullptr;
}

/* -- EOF -- */
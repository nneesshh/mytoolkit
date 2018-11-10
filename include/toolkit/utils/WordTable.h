#pragma once
//------------------------------------------------------------------------------
/**
@class CWordTable

(C) 2016 n.lee
*/
#include <stdio.h>
#include <string>
#include <vector>
#include <map>

//------------------------------------------------------------------------------
/**
@brief CWordTable
*/
class CWordTable {
public:
	CWordTable();
	~CWordTable();

	using tWordList = std::vector<std::string>;

	void						LoadFromFile(const char *sFile);

	void						AddWord(const char *sWord);
	const char *				GetWordAt(size_t nIndex);

	size_t						GetWordCount() {
		return _wordList.size();
	}

private:
	tWordList			_wordList;
};

/*EOF*/
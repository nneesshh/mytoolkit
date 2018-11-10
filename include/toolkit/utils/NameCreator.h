#pragma once
//------------------------------------------------------------------------------
/**
    @class CNameCreator
    
    (C) 2016 n.lee
*/
#include <stdio.h>
#include <string>
#include <vector>
#include <map>

//------------------------------------------------------------------------------
/** 
	@brief CNameCreator
 */
class CNameCreator {
public:
	CNameCreator();
	~CNameCreator();

	typedef std::vector<std::string>	NAME_WORD_LIST;

	NAME_WORD_LIST	_surnameWordList;
	NAME_WORD_LIST	_femaleWordList;
	NAME_WORD_LIST	_maleWordList;

	void						LoadFromFile(NAME_WORD_LIST& list, const char *sFile );
	void						AddWord(NAME_WORD_LIST& list, const char *sWord);
	std::string					CreateName(int nSex);

};

/*EOF*/
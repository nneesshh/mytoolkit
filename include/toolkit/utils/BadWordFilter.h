#pragma once
//------------------------------------------------------------------------------
/**
    @class CBadWordFilter
    
    (C) 2016 n.lee
*/
#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>

//------------------------------------------------------------------------------
/** 
	@brief CBadWordFilter
*/
class CBadWordFilter {
public:
	CBadWordFilter();
	~CBadWordFilter();

	typedef std::vector<std::wstring>									tWcharWordList;
	typedef std::unordered_map< wchar_t, std::vector<std::wstring> >	tWcharKeyMap;

	void						LoadFromFile( const char *sFile );

	///
	void						OnFilter( char *sInput );

	void						AddWord(const char *sWord);

private:
	tWcharWordList			_badWordList;
	tWcharKeyMap			_mapBadKey; // key word->bad word

	wchar_t					_wbuffer[1024];
};

/*EOF*/
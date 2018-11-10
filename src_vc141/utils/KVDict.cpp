//------------------------------------------------------------------------------
//  KVDict.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "KVDict.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif
#endif

//------------------------------------------------------------------------------
/**

*/
CKVDict::CKVDict(std::string *arrKeys, int nKeyNum)
	: _arrKeys(arrKeys)
	, _nKeyNum(nKeyNum) {

}

//------------------------------------------------------------------------------
/**

*/
CKVDict::~CKVDict() {

}

/* -- EOF -- */
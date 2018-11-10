#pragma once
//------------------------------------------------------------------------------
/**
@class CKVDict

(C) 2016 n.lee
*/
#include <string>
#include <unordered_map>
#include <algorithm>

#include "MyUtil.h"

//------------------------------------------------------------------------------
/**
@brief CKVDict
*/
class CKVDict {
public:
	CKVDict(std::string *arrKeys, int nKeyNum);
	~CKVDict();

	///
	typedef std::unordered_map<std::string, int> KEY_VALUE_DICT;

	void						SetVal(const char *sKey, int nVal) { _mFieldValue[sKey] = nVal; }
	int							GetVal(const char *sKey) { return _mFieldValue[sKey]; }

	void						SetVal(int nKeyID, int nVal) {
		SetVal(_arrKeys[nKeyID].c_str(), nVal);
	}

	int							GetVal(int nKeyID) {
		return GetVal(_arrKeys[nKeyID].c_str());
	}

public:
	virtual int					ParseKeyID(const char *sKey) {
		int i;
		for (i = 0; i<_nKeyNum; ++i) {
			if (0 == _strcmpi(sKey, _arrKeys[i].c_str()))
				return i;
		}
		return -1;
	}

	int							Increase(int nKeyID, int nVal) {
		int nOld = _mFieldValue[_arrKeys[nKeyID]];
		_mFieldValue[_arrKeys[nKeyID]] = nOld + nVal;
		return nVal;
	}

	int							Decrease(int nKeyID, int nVal) {
		int nOld = _mFieldValue[_arrKeys[nKeyID]];
		int	nSub = std::min<int>(nOld, nVal);
		_mFieldValue[_arrKeys[nKeyID]] -= nSub;
		return nSub;
	}

	void						Copy(CKVDict &rh) {
		_mFieldValue.clear();

		for (auto& iter : rh._mFieldValue) {
			SetVal(iter.first.c_str(), iter.second);
		}
	}

private:
	KEY_VALUE_DICT		_mFieldValue;

	std::string			*_arrKeys;
	int					_nKeyNum;
};

/*EOF*/
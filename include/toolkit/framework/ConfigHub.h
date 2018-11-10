#pragma once
//------------------------------------------------------------------------------
/**
@class CConfigHub

(C) 2016 n.lee
*/
#include <algorithm>
#include "IHandler.h"
#include "IConfig.h"

#define CONFIG_HUB_CAPACITY	1024

//------------------------------------------------------------------------------
/**
@brief CConfigHub
*/
class CConfigHub : public IHandler {
public:
	CConfigHub();
	virtual ~CConfigHub();

	virtual void			OnInit();
	virtual void			OnDelete();

	///
	virtual void 			OnUpdate(float fElapsed);

public:
	virtual void			RegisterConfig(int nIndex, IConfig *pConfig) {
		_registeredConfigMax = std::max<int>(_registeredConfigMax, std::min<int>(CONFIG_HUB_CAPACITY, nIndex + 1));
		_arrConfig[nIndex] = pConfig;
	}

	virtual IConfig *		GetConfigAt(int nIndex) {
		return _arrConfig[nIndex];
	}

public:
	IConfig *	_arrConfig[CONFIG_HUB_CAPACITY];
	int			_registeredConfigMax;
};

/*EOF*/
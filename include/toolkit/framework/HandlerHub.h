#pragma once
//------------------------------------------------------------------------------
/**
@class CHandlerHub

(C) 2016 n.lee
*/
#include <algorithm>
#include "IHandler.h"

#define HANDLER_HUB_CAPACITY	1024

//------------------------------------------------------------------------------
/**
@brief CHandlerHub
*/
class CHandlerHub : public IHandler {
public:
	CHandlerHub();
	virtual ~CHandlerHub();

	virtual void			OnInit(time_t tmNow);
	virtual void			OnDelete();

	///
	virtual void 			OnUpdate(float fElapsed);

	virtual void			RegisterHandlers() = 0;

	virtual void			RegisterHandler(int nIndex, IHandler *pHandler) {
		_registeredHandlerMax = std::max<int>(_registeredHandlerMax, std::min<int>(HANDLER_HUB_CAPACITY, nIndex + 1));
		_arrHandler[nIndex] = pHandler;
		pHandler->_hid = nIndex;
	}

	virtual IHandler *		GetHandlerAt(int nIndex) {
		return _arrHandler[nIndex];
	}

public:
	IHandler *	_arrHandler[HANDLER_HUB_CAPACITY];
	int			_registeredHandlerMax;
};

/*EOF*/
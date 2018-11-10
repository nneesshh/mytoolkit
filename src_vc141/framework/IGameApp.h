#pragma once
//------------------------------------------------------------------------------
/**
@class IGameApp

(C) 2016 n.lee
*/
#include <string>

#include "IRedisHandler.h"
#include "IAppBase.h"

//------------------------------------------------------------------------------
/**
@brief IGameApp
*/
class IGameApp : public IAppBase {
public:
	virtual ~IGameApp() { }

	virtual IRedisHandler *		GetRedisHandler() = 0;
};

/*EOF*/
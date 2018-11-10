#pragma once
//------------------------------------------------------------------------------
/**
@class IAppPlugin

(C) 2016 n.lee
*/
#include "HandlerHub.h"

//------------------------------------------------------------------------------
/**
@brief IAppPlugin
*/
class IAppPlugin : public CHandlerHub {
public:
	virtual ~IAppPlugin() { }

	virtual const char *		GetName() = 0;
};

/*EOF*/
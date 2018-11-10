#pragma once
//------------------------------------------------------------------------------
/**
	@class IRedisHandler

	(C) 2016 n.lee
*/
#include <string>
#include "IHandler.h"

//------------------------------------------------------------------------------
/**
@brief IRedisHandler
*/
class IRedisHandler : public IHandler {
public:
	virtual ~IRedisHandler() { }

	virtual void *				Entry() = 0;

	virtual bool				IsClosed() = 0;

	virtual void *				CreateHelperEntry(const std::string& sModuleName, const std::string& sIp, unsigned short nPort) = 0;
	virtual void *				HelperEntry(int nId) = 0;

};

/*EOF*/
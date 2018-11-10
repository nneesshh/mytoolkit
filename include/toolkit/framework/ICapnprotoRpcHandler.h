#pragma once

#include <capnp/message.h>

#include "IHandler.h"

//------------------------------------------------------------------------------
/**
@class ICapnprotoRpcHandler

(C) 2016 n.lee
*/

//------------------------------------------------------------------------------
/**
@brief ICapnprotoRpcHandler
*/
class ICapnprotoRpcHandler : public IHandler {
public:
	virtual ~ICapnprotoRpcHandler() { }

	virtual void *				CreateServer(std::string& strServer) = 0;
	virtual void				Activate() = 0;
};

/*EOF*/
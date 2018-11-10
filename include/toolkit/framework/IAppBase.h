#pragma once
//------------------------------------------------------------------------------
/**
@class IAppBase

(C) 2016 n.lee
*/
#include <string>

#include "IAppPlugin.h"

#include "../log/StdLog.h"
#include "../core/LRandom.h"
#include "../core/Heartbeat.h"
#include "../core/TimeoutEventHub.h"

//------------------------------------------------------------------------------
/**
@brief IAppBase
*/
class IAppBase {
public:
	virtual ~IAppBase() { }

	virtual void				OnInit() = 0;
	virtual void				OnDelete() = 0;

	///
	virtual void 				OnUpdate(unsigned int nElapsedInMs, uint64_t uNowSystemTimeInMs) = 0;

	///
	virtual void 				Run() = 0;
	virtual void 				Exit() = 0;

	///
	virtual const char *		ConfigPath() = 0;
	virtual const char *		ModulePath() = 0;
	virtual void				AddPlugin(IAppPlugin *pPlugin) = 0;
	virtual IAppPlugin *		GetPlugin(const char *sPluginName) = 0;

	virtual CLRandom&			LRandom() = 0;
	virtual IHeartbeat& 		Heartbeat() = 0;
	virtual StdLog& 			LogHandler() = 0;
	virtual CTimeoutEventHub&	TimeoutEventHub() = 0;

};

/*EOF*/
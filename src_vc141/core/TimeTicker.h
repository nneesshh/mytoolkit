#pragma once
//------------------------------------------------------------------------------
/**
	@class CTimeTicker

	(C) 2016 n.lee
*/
#include <unordered_map>
#include <functional>

#include "IHeartbeat.h"
#include "TimeoutEventHub.h"
#include "timingwheel/timeout/timeout.h"

//////////////////////////////////////////////////////////////////////////
class CTimeTickerOp;
using MAP_TIME_TICKER_OP = std::unordered_map<uint32_t, CTimeTickerOp *>;

//------------------------------------------------------------------------------
/**
@brief CTimeTicker
*/
class CTimeTicker {
public:
	CTimeTicker(const char *sName, CTimeoutEventHub *pEventHub, IHeartbeat *pHeartbeat);
	~CTimeTicker();

	using TIME_TICKER_TIMEOUT_CB = std::function<void(uint32_t)>;

	void						SetExpireTimerInMs(uint32_t id, unsigned int expire_time_in_ms, TIME_TICKER_TIMEOUT_CB cb);

#ifndef TIMEOUT_DISABLE_INTERVALS
	void						SetIntervalTimerInMs(uint32_t id, unsigned int interval_time_in_ms, TIME_TICKER_TIMEOUT_CB cb);
#endif

	bool						TimerExists(uint32_t id) {
		return (_mapTimeTickerOp.find(id) != _mapTimeTickerOp.end());
	}

	void						CancelTimer(uint32_t id);
	void						CancelAllTimers();

public:
	std::string _sName;
	CTimeoutEventHub *_refEventHub;
	IHeartbeat *_refHeartbeat;

public:
	MAP_TIME_TICKER_OP _mapTimeTickerOp;
};

/*EOF*/
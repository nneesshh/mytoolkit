#pragma once
//------------------------------------------------------------------------------
/**
	@class CTimeTickerAbs

(C) 2016 n.lee
*/
#include "TimeTicker.h"
#include "IHeartbeat.h"

//------------------------------------------------------------------------------
/**
@brief CTimeTickerAbs
*/
class CTimeTickerAbs : public CTimeTicker {
public:
	CTimeTickerAbs(const char *sName, CTimeoutEventHub *pHub, IHeartbeat *pHeartbeat);
	virtual ~CTimeTickerAbs();

	void						SetEvent(time_t tmNow, time_t tmBegin, time_t tmEnd, TIME_TICKER_TIMEOUT_CB onBegin, TIME_TICKER_TIMEOUT_CB onEnd);

private:
	unsigned long _id;
};

/*EOF*/
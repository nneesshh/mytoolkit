#ifndef TIMETICKERSLOTABS_H
#define TIMETICKERSLOTABS_H
//------------------------------------------------------------------------------
/**
@class CTimeSlicer

(C) 2011 n.lee
*/
#include "TimeTicker.h"

//------------------------------------------------------------------------------
/**
@brief CTimeTickerSlotAbs
*/
class	CTimeTickerSlotAbs : public CTimeTicker {
public:
	CTimeTickerSlotAbs(CTimeoutEventHub *pHub);
	virtual ~CTimeTickerSlotAbs();

public:
	void						SetEvent(time_t tmNow, time_t tmBegin, time_t tmEnd, uintptr_t paramid1, uint64_t paramid2, ON_TIME_TICK_TIMEOUT dBegin, ON_TIME_TICK_TIMEOUT dEnd);

private:
	int _id;
};

#endif
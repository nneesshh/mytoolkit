//------------------------------------------------------------------------------
//  TimeTickerAbs.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "TimeTickerAbs.h"

#include "MyMacros.h"
#include "../utils/MyUtil.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif
#endif

//------------------------------------------------------------------------------
/**

*/
CTimeTickerAbs::CTimeTickerAbs(const char *sName, CTimeoutEventHub *pHub, IHeartbeat *pHeartbeat)
	: CTimeTicker(sName, pHub, pHeartbeat) {

}

//------------------------------------------------------------------------------
/**

*/
CTimeTickerAbs::~CTimeTickerAbs() {

}

//------------------------------------------------------------------------------
/**

*/
void
CTimeTickerAbs::SetEvent(time_t tmNow, time_t tmBegin, time_t tmEnd, TIME_TICKER_TIMEOUT_CB onBegin, TIME_TICKER_TIMEOUT_CB onEnd) {
	int expire_time1 = tmBegin - tmNow;
	int expire_time2 = tmEnd - tmNow;
	if (expire_time2 > 0) {
		expire_time1 = std::max<int>(0, expire_time1);
		expire_time2 = std::max<int>(expire_time1, expire_time2);

		SetExpireTimerInMs(++_id, expire_time1 * 1000, onBegin);
		SetExpireTimerInMs(++_id, expire_time2 * 1000, onEnd);
	}
}

/* -- EOF -- */
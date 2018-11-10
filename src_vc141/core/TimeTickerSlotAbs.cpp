//------------------------------------------------------------------------------
//  TimeTickerSlotAbs.cpp
//  (C) 2011 n.lee
//------------------------------------------------------------------------------
#include "TimeTickerSlotAbs.h"

#include "MyMacros.h"
#include "../snprintf/mysnprintf.h"
#include "../utils/MyUtil.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif
#endif

//------------------------------------------------------------------------------
/**

*/
CTimeTickerSlotAbs::CTimeTickerSlotAbs(CTimeoutEventHub *pHub)
	: CTimeTicker(pHub) {

}

//------------------------------------------------------------------------------
/**

*/
CTimeTickerSlotAbs::~CTimeTickerSlotAbs() {

}

//------------------------------------------------------------------------------
/**

*/
void
CTimeTickerSlotAbs::SetEvent(time_t tmNow, time_t tmBegin, time_t tmEnd, uintptr_t paramid1, uint64_t paramid2, ON_TIME_TICK_TIMEOUT dBegin, ON_TIME_TICK_TIMEOUT dEnd) {
	std::unique_lock<std::shared_timed_mutex> _(_mutex);

	int expire_time1 = tmBegin - tmNow;
	int expire_time2 = tmEnd - tmNow;
	if (expire_time2 > 0) {
		expire_time1 = std::max<int>(0, expire_time1);
		expire_time2 = std::max<int>(expire_time1, expire_time2);

		SetExpireEvent(++_id, paramid1, paramid2, expire_time1, dBegin);
		SetExpireEvent(++_id, paramid1, paramid2, expire_time1, dEnd);
	}
}

/* -- EOF -- */
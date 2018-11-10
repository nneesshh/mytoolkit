//------------------------------------------------------------------------------
//  TimeoutEventHub.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "TimeoutEventHub.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif
#endif

//------------------------------------------------------------------------------
/**

*/
CTimeoutEventHub::CTimeoutEventHub() {
	// 1ms / hz
	timeout_t hz_set = 1;
	int err = 0;
	_tos = timeouts_open(hz_set, &err);
}

//------------------------------------------------------------------------------
/**

*/
CTimeoutEventHub::~CTimeoutEventHub() {
	timeouts_close(_tos);
}

//------------------------------------------------------------------------------
/**

*/
void
CTimeoutEventHub::AdjustStartPoint(uint64_t start_point_in_ms) {
	_startPointInMs = start_point_in_ms;
	AdvanceTo(_startPointInMs);
}

//------------------------------------------------------------------------------
/**

*/
void
CTimeoutEventHub::AdvanceTo(uint64_t new_time_in_ms) {

	int ncount = 0;

	// timeout queue
	std::vector<struct timeout *> vTimeout;
	struct timeout *to;
	timeouts_update(_tos, timeouts_f2i(_tos, new_time_in_ms));
	while (nullptr != (to = timeouts_get(_tos))) {

		to->callback.fn(to->callback.arg);

		//
		++ncount;
	}
}

/* -- EOF -- */
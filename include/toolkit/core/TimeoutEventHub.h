#pragma once
//------------------------------------------------------------------------------
/**
	@class CTimeoutEventHub

	(C) 2016 n.lee
*/
#include <limits.h>
#include <stdint.h>

#include <vector>
#include <time.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "timingwheel/timeout/timeout.h"

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------------
/**
@brief CTimeoutEventHub
*/
class CTimeoutEventHub {
public:
	CTimeoutEventHub();
	~CTimeoutEventHub();

public:
	void						AdjustStartPoint(uint64_t start_point_in_ms);

	uint64_t					GetStartPointInMs() {
		return _startPointInMs;
	}

	void						AdvanceTo(uint64_t new_time_in_ms);

	bool						IsTimeoutTicking(struct timeout *to) {
		return (NULL != to->pending);
	}

	struct timeout *			AddTimeoutAbs(struct timeout *to, uint64_t abstime_in_ms) {

		assert(!IsTimeoutTicking(to));

		timeouts_sched_abs(_tos, to, abstime_in_ms);
		return to;
	}

	struct timeout *			AddTimeout(struct timeout *to, unsigned int time_in_ms) {

		assert(!IsTimeoutTicking(to));

		timeouts_addf(_tos, to, time_in_ms);
		return to;
	}

	void						InitExpireTimeout(struct timeout *to) {
		timeout_init(to, 0); // expire
	}

#ifndef TIMEOUT_DISABLE_INTERVALS
	void						InitIntervalTimeout(struct timeout *to) {
		timeout_init(to, TIMEOUT_INT); // interval
	}
#endif

	void						DelTimeout(struct timeout *to) {
		timeouts_del(_tos, to);
	}

private:
	struct timeouts *_tos;

	uint64_t _startPointInMs;
};

/*EOF*/
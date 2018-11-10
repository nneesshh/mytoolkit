//------------------------------------------------------------------------------
//  TimeSlicer.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "TimeSlicer.h"

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
bool
CTimeSliceFormatDaily::AllocCycle(const time_t tmDaypoint, time_slicer_cycle_t& outCycle) {
	bool bSuccess = false;
	time_t tmDay = CSimpleCalendar::DatetimeToDate(tmDaypoint);
	const unsigned int CYCLE_TIME = DAY_SECONDS;

	//
	time_t tmBase = tmDay;
	unsigned int uTimeMin = CYCLE_TIME;
	unsigned int uTimeMax = 0;
	// check time slice min and slice max
	for (auto& slice : _vSlice) {
		if (slice._use_cycle_end) {
			slice._end_time = slice._begin_time + CYCLE_TIME;
		}

		uTimeMin = (slice._begin_time < uTimeMin) ? slice._begin_time : uTimeMin;
		uTimeMax = (slice._end_time > uTimeMax) ? slice._end_time : uTimeMax;
	}

	// adjust bound
	if (uTimeMax > CYCLE_TIME) {
		uTimeMin = std::min<unsigned int>(uTimeMin, uTimeMax - CYCLE_TIME);
		uTimeMax = std::min<unsigned int>(uTimeMax, uTimeMin + CYCLE_TIME);
	}
	else {
		uTimeMin = 0;
		uTimeMax = CYCLE_TIME;
	}
	
	//
	if (IsBetweenPeriod(tmDaypoint, (uint64_t)(tmBase), uTimeMin, uTimeMax)) {
		outCycle._base = tmBase;
		outCycle._timemin = uTimeMin;
		outCycle._timemax = uTimeMax;
		bSuccess = true;
	}
	else {
		// check pre
		tmBase -= CYCLE_TIME;
		if (IsBetweenPeriod(tmDaypoint, (uint64_t)(tmBase), uTimeMin, uTimeMax)) {
			outCycle._base = tmBase;
			outCycle._timemin = uTimeMin;
			outCycle._timemax = uTimeMax;
			bSuccess = true;
		}
	}
	return bSuccess;
}

//------------------------------------------------------------------------------
/**

*/
bool
CTimeSliceFormatDaily::AllocNextCycle(const time_slicer_cycle_t& cycle, time_slicer_cycle_t& outNextCycle) {
	const unsigned int CYCLE_TIME = DAY_SECONDS;
	outNextCycle = cycle;
	outNextCycle._base += CYCLE_TIME;
	return true;
}

//------------------------------------------------------------------------------
/**

*/
bool
CTimeSliceFormatWeekly::AllocCycle(const time_t tmDaypoint, time_slicer_cycle_t& outCycle) {
	bool bSuccess = false;
	time_t tmWeekDay = CSimpleCalendar::WeekDay(tmDaypoint);
	const unsigned int CYCLE_TIME = WEEK_SECONDS;

	//
	time_slice_t *slice;
	time_t tmBase = tmWeekDay;
	unsigned int uTimeMin = CYCLE_TIME;
	unsigned int uTimeMax = 0;

	// check time slice min and slice max
	for (auto& slice : _vSlice) {
		if (slice._use_cycle_end) {
			slice._end_time = slice._begin_time + CYCLE_TIME;
		}

		uTimeMin = (slice._begin_time < uTimeMin) ? slice._begin_time : uTimeMin;
		uTimeMax = (slice._end_time > uTimeMax) ? slice._end_time : uTimeMax;
	}

	// adjust bound
	if (uTimeMax > CYCLE_TIME) {
		uTimeMin = std::min<unsigned int>(uTimeMin, uTimeMax - CYCLE_TIME);
		uTimeMax = std::min<unsigned int>(uTimeMax, uTimeMin + CYCLE_TIME);
	}
	else {
		uTimeMin = 0;
		uTimeMax = CYCLE_TIME;
	}

	//
	if (IsBetweenPeriod(tmDaypoint, (uint64_t)(tmBase), uTimeMin, uTimeMax)) {
		outCycle._base = tmBase;
		outCycle._timemin = uTimeMin;
		outCycle._timemax = uTimeMax;
		bSuccess = true;
	}
	else {
		// check pre
		tmBase -= CYCLE_TIME;
		if (IsBetweenPeriod(tmDaypoint, (uint64_t)(tmBase), uTimeMin, uTimeMax)) {
			outCycle._base = tmBase;
			outCycle._timemin = uTimeMin;
			outCycle._timemax = uTimeMax;
			bSuccess = true;
		}
	}
	return bSuccess;
}

//------------------------------------------------------------------------------
/**

*/
bool
CTimeSliceFormatWeekly::AllocNextCycle(const time_slicer_cycle_t& cycle, time_slicer_cycle_t& outNextCycle) {
	const unsigned int CYCLE_TIME = WEEK_SECONDS;
	outNextCycle = cycle;
	outNextCycle._base += CYCLE_TIME;
	return true;
}

//------------------------------------------------------------------------------
/**

*/
bool
CTimeSliceFormatMonthly::AllocCycle(const time_t tmDaypoint, time_slicer_cycle_t& outCycle) {
	bool bSuccess = false;
	time_t tmMonthDay, tmMonthDayPre, tmMonthDayPost;
	CSimpleCalendar::MonthDay(tmDaypoint, tmMonthDay, tmMonthDayPre, tmMonthDayPost);
	const unsigned int CYCLE_TIME = tmMonthDayPost - tmMonthDay;

	//
	time_slice_t *slice;
	time_t tmBase = tmMonthDay;
	unsigned int uTimeMin = CYCLE_TIME;
	unsigned int uTimeMax = 0;

	// check time slice min and slice max
	for (auto& slice : _vSlice) {
		if (slice._use_cycle_end) {
			slice._end_time = slice._begin_time + CYCLE_TIME;
		}

		uTimeMin = (slice._begin_time < uTimeMin) ? slice._begin_time : uTimeMin;
		uTimeMax = (slice._end_time > uTimeMax) ? slice._end_time : uTimeMax;
	}

	// adjust bound
	if (uTimeMax > CYCLE_TIME) {
		uTimeMin = std::min<unsigned int>(uTimeMin, uTimeMax - CYCLE_TIME);
		uTimeMax = std::min<unsigned int>(uTimeMax, uTimeMin + CYCLE_TIME);
	}
	else {
		uTimeMin = 0;
		uTimeMax = CYCLE_TIME;
	}

	//
	if (IsBetweenPeriod(tmDaypoint, (uint64_t)(tmBase), uTimeMin, uTimeMax)) {
		outCycle._base = tmBase;
		outCycle._timemin = uTimeMin;
		outCycle._timemax = uTimeMax;
		bSuccess = true;
	}
	else {
		// check pre
		tmBase = tmMonthDayPre;
		if (IsBetweenPeriod(tmDaypoint, (uint64_t)(tmBase), uTimeMin, uTimeMax)) {
			outCycle._base = tmBase;
			outCycle._timemin = uTimeMin;
			outCycle._timemax = uTimeMax;
			bSuccess = true;
		}
	}
	return bSuccess;
}

//------------------------------------------------------------------------------
/**

*/
bool
CTimeSliceFormatMonthly::AllocNextCycle(const time_slicer_cycle_t& cycle, time_slicer_cycle_t& outNextCycle) {
	time_t tmDaypoint = cycle._base + cycle._timemax; // timemax is in next cycle

	return AllocCycle(tmDaypoint, outNextCycle);
}

/* -- EOF -- */
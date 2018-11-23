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
void
CTimeSliceFormatDaily::ResetCycle(const time_t tmCheck, time_slicer_cycle_t& outCycle) const {
	time_t tmDay = CSimpleCalendar::DatetimeToDate(tmCheck);
	const unsigned int CYCLE_TIME = DAY_SECONDS;

	//
	time_t tmBase = tmDay;
	unsigned int uTimeMin = CYCLE_TIME;
	unsigned int uTimeMax = 0;
	unsigned int uEndTime = 0;

	// check time slice min and slice max
	for (auto& slice : _vSlice) {
		// valid format end time
		uEndTime = CTimeSliceFormatBase::GetValidFormatEndTime(&slice, CYCLE_TIME);

		uTimeMin = (slice._begin_time < uTimeMin) ? slice._begin_time : uTimeMin;
		uTimeMax = (uEndTime > uTimeMax) ? uEndTime : uTimeMax;
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
	if (IsBetweenPeriod(tmCheck, tmBase, uTimeMin, uTimeMax)) {
		outCycle._base = tmBase;
		outCycle._timemin = uTimeMin;
		outCycle._timemax = uTimeMax;
		return;
	}

	// use pre
	tmBase -= CYCLE_TIME;
	outCycle._base = tmBase;
	outCycle._timemin = uTimeMin;
	outCycle._timemax = uTimeMax;
}

//------------------------------------------------------------------------------
/**

*/
void
CTimeSliceFormatWeekly::ResetCycle(const time_t tmCheck, time_slicer_cycle_t& outCycle) const {
	time_t tmWeekDay = CSimpleCalendar::WeekDay(tmCheck);
	const unsigned int CYCLE_TIME = WEEK_SECONDS;

	//
	time_slice_t *slice;
	time_t tmBase = tmWeekDay;
	unsigned int uTimeMin = CYCLE_TIME;
	unsigned int uTimeMax = 0;
	unsigned int uEndTime = 0;

	// check time slice min and slice max
	for (auto& slice : _vSlice) {
		// valid format end time
		uEndTime = CTimeSliceFormatBase::GetValidFormatEndTime(&slice, CYCLE_TIME);

		uTimeMin = (slice._begin_time < uTimeMin) ? slice._begin_time : uTimeMin;
		uTimeMax = (uEndTime > uTimeMax) ? uEndTime : uTimeMax;
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
	if (IsBetweenPeriod(tmCheck, tmBase, uTimeMin, uTimeMax)) {
		outCycle._base = tmBase;
		outCycle._timemin = uTimeMin;
		outCycle._timemax = uTimeMax;
		return;
	}

	// use pre
	tmBase -= CYCLE_TIME;
	outCycle._base = tmBase;
	outCycle._timemin = uTimeMin;
	outCycle._timemax = uTimeMax;
}

//------------------------------------------------------------------------------
/**

*/
void
CTimeSliceFormatMonthly::ResetCycle(const time_t tmCheck, time_slicer_cycle_t& outCycle) const {
	time_t tmMonthDay, tmMonthDayPre, tmMonthDayPost;
	CSimpleCalendar::MonthDay(tmCheck, tmMonthDay, tmMonthDayPre, tmMonthDayPost);
	const unsigned int CYCLE_TIME = tmMonthDayPost - tmMonthDay;

	//
	time_slice_t *slice;
	time_t tmBase = tmMonthDay;
	unsigned int uTimeMin = CYCLE_TIME;
	unsigned int uTimeMax = 0;
	unsigned int uEndTime = 0;

	// check time slice min and slice max
	for (auto& slice : _vSlice) {
		// valid format end time
		uEndTime = CTimeSliceFormatBase::GetValidFormatEndTime(&slice, CYCLE_TIME);

		uTimeMin = (slice._begin_time < uTimeMin) ? slice._begin_time : uTimeMin;
		uTimeMax = (uEndTime > uTimeMax) ? uEndTime : uTimeMax;
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
	if (IsBetweenPeriod(tmCheck, tmBase, uTimeMin, uTimeMax)) {
		outCycle._base = tmBase;
		outCycle._timemin = uTimeMin;
		outCycle._timemax = uTimeMax;
		return;
	}

	// use pre
	tmBase = tmMonthDayPre;
	outCycle._base = tmBase;
	outCycle._timemin = uTimeMin;
	outCycle._timemax = uTimeMax;
}

/* -- EOF -- */
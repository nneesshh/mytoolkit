#pragma once
//------------------------------------------------------------------------------
/**
	@class CTimeSlicer

	(C) 2016 n.lee
*/
#include "TimeSlicerBase.h"

//------------------------------------------------------------------------------
/**
@brief CTimeSliceFormatDaily
*/
class CTimeSliceFormatDaily : public CTimeSliceFormatBase {
public:
	CTimeSliceFormatDaily(CSimpleCalendar *pCalendar) : CTimeSliceFormatBase(pCalendar) { }
	virtual ~CTimeSliceFormatDaily() { };

	virtual bool				AllocCycle(const time_t tmDaypoint, time_slicer_cycle_t& outCycle);
	virtual bool				AllocNextCycle(const time_slicer_cycle_t& cycle, time_slicer_cycle_t& outCycle);
};

//------------------------------------------------------------------------------
/**
@brief CTimeSliceFormatWeekly
*/
class CTimeSliceFormatWeekly : public CTimeSliceFormatBase {
public:
	CTimeSliceFormatWeekly(CSimpleCalendar *pCalendar) : CTimeSliceFormatBase(pCalendar) { }
	virtual ~CTimeSliceFormatWeekly() { };

	virtual bool				AllocCycle(const time_t tmDaypoint, time_slicer_cycle_t& outCycle);
	virtual bool				AllocNextCycle(const time_slicer_cycle_t& cycle, time_slicer_cycle_t& outCycle);
};

//------------------------------------------------------------------------------
/**
@brief CTimeSliceFormatMonthly
*/
class CTimeSliceFormatMonthly : public CTimeSliceFormatBase {
public:
	CTimeSliceFormatMonthly(CSimpleCalendar *pCalendar) : CTimeSliceFormatBase(pCalendar) { }
	virtual ~CTimeSliceFormatMonthly() { };

	virtual unsigned int		GetCycleTimeInSeconds() {
		return 7 * DAY_SECONDS;
	}

	virtual bool				AllocCycle(const time_t tmDaypoint, time_slicer_cycle_t& outCycle);
	virtual bool				AllocNextCycle(const time_slicer_cycle_t& cycle, time_slicer_cycle_t& outCycle);
};

//------------------------------------------------------------------------------
/**
@brief CTimeSlicerDaily
*/
class CTimeSlicerDaily : public CTimeSlicerBase {
public:
	CTimeSlicerDaily(const char *sName, CTimeoutEventHub *pHub, IHeartbeat *pHeartbeat, CTimeSliceFormatDaily *pFormat)
		: CTimeSlicerBase(sName, pHub, pHeartbeat, pFormat) { }
	virtual ~CTimeSlicerDaily() { }
};

//------------------------------------------------------------------------------
/**
@brief CTimeSlicerWeekly
*/
class CTimeSlicerWeekly : public CTimeSlicerBase {
public:
	CTimeSlicerWeekly(const char *sName, CTimeoutEventHub *pHub, IHeartbeat *pHeartbeat, CTimeSliceFormatWeekly *pFormat)
		: CTimeSlicerBase(sName, pHub, pHeartbeat, pFormat) { }
	virtual ~CTimeSlicerWeekly() { }

};

//------------------------------------------------------------------------------
/**
@brief CTimeSlicerMonthly
*/
class CTimeSlicerMonthly : public CTimeSlicerBase {
public:
	CTimeSlicerMonthly(const char *sName, CTimeoutEventHub *pHub, IHeartbeat *pHeartbeat, CTimeSliceFormatMonthly *pFormat)
		: CTimeSlicerBase(sName, pHub, pHeartbeat, pFormat) { }
	virtual ~CTimeSlicerMonthly() { }

};

/*EOF*/
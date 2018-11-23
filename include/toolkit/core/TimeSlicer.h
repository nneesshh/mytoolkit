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
	CTimeSliceFormatDaily() : CTimeSliceFormatBase() { }
	virtual ~CTimeSliceFormatDaily() { };

	virtual void				ResetCycle(const time_t tmCheck, time_slicer_cycle_t& outCycle) const override;

	virtual void				RollCycleBase(time_slicer_cycle_t& inOutCycle) const override {
		const unsigned int CYCLE_TIME = DAY_SECONDS;
		inOutCycle._base += CYCLE_TIME;
	}

};

//------------------------------------------------------------------------------
/**
@brief CTimeSliceFormatWeekly
*/
class CTimeSliceFormatWeekly : public CTimeSliceFormatBase {
public:
	CTimeSliceFormatWeekly() : CTimeSliceFormatBase() { }
	virtual ~CTimeSliceFormatWeekly() { };

	virtual void				ResetCycle(const time_t tmCheck, time_slicer_cycle_t& outCycle) const override;

	virtual void				RollCycleBase(time_slicer_cycle_t& inOutCycle) const override {
		const unsigned int CYCLE_TIME = WEEK_SECONDS;
		inOutCycle._base += CYCLE_TIME;
	}

};

//------------------------------------------------------------------------------
/**
@brief CTimeSliceFormatMonthly
*/
class CTimeSliceFormatMonthly : public CTimeSliceFormatBase {
public:
	CTimeSliceFormatMonthly() : CTimeSliceFormatBase() { }
	virtual ~CTimeSliceFormatMonthly() { };

	virtual void				ResetCycle(const time_t tmCheck, time_slicer_cycle_t& outCycle) const override;

	virtual void				RollCycleBase(time_slicer_cycle_t& inOutCycle) const override {
		time_t tmCheck = inOutCycle._base + inOutCycle._timemax; // timemax is in next cycle
		ResetCycle(tmCheck, inOutCycle);
	}

};

//------------------------------------------------------------------------------
/**
@brief CTimeSlicerDaily
*/
class CTimeSlicerDaily : public CTimeSlicerBase {
public:
	CTimeSlicerDaily(const char *sName, CTimeoutEventHub *pHub, IHeartbeat *pHeartbeat, const CTimeSliceFormatDaily *pFormat)
		: CTimeSlicerBase(sName, pHub, pHeartbeat, pFormat) { }
	virtual ~CTimeSlicerDaily() { }
};

//------------------------------------------------------------------------------
/**
@brief CTimeSlicerWeekly
*/
class CTimeSlicerWeekly : public CTimeSlicerBase {
public:
	CTimeSlicerWeekly(const char *sName, CTimeoutEventHub *pHub, IHeartbeat *pHeartbeat, const CTimeSliceFormatWeekly *pFormat)
		: CTimeSlicerBase(sName, pHub, pHeartbeat, pFormat) { }
	virtual ~CTimeSlicerWeekly() { }

};

//------------------------------------------------------------------------------
/**
@brief CTimeSlicerMonthly
*/
class CTimeSlicerMonthly : public CTimeSlicerBase {
public:
	CTimeSlicerMonthly(const char *sName, CTimeoutEventHub *pHub, IHeartbeat *pHeartbeat, const CTimeSliceFormatMonthly *pFormat)
		: CTimeSlicerBase(sName, pHub, pHeartbeat, pFormat) { }
	virtual ~CTimeSlicerMonthly() { }

};

/*EOF*/
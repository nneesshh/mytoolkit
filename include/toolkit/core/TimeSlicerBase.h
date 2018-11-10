#pragma once
//------------------------------------------------------------------------------
/**
	@class CTimeSlicerBase

	(C) 2016 n.lee
*/
#include "IHeartbeat.h"
#include "TimeoutEventHub.h"
#include "SimpleCalendar.h"

#include "ITimeSlicer.h"

//////////////////////////////////////////////////////////////////////////
using TIME_SLICE_LIST = std::vector<time_slice_t>;

class CTimeSlicerOp;

//------------------------------------------------------------------------------
/**
@brief CTimeSliceFormatBase
*/
class CTimeSliceFormatBase : public ITimeSliceFormat {
public:
	CTimeSliceFormatBase(CSimpleCalendar *pCalendar) : _refCalendar(pCalendar) { }
	virtual ~CTimeSliceFormatBase() { };

	CSimpleCalendar *_refCalendar;
	TIME_SLICE_LIST _vSlice;

	virtual time_slice_t *		AddFormat(const char *sBeginTime, int nPeriod = -1);
	virtual time_slice_t *		AddFormat(const char *sBeginTime, const char *sEndTime);

public:
	static bool					IsBetweenPeriod(const time_t tmDaypoint, uint64_t uBase, unsigned int uBeginTime, unsigned int uEndTime) {
		return (time_t)(uBase + uBeginTime) <= tmDaypoint
			&& tmDaypoint < (time_t)(uBase + uEndTime);
	}

};

//------------------------------------------------------------------------------
/**
@brief CTimeSlicerBase
*/
class CTimeSlicerBase : public ITimeSlicer {
public:
	CTimeSlicerBase(const char *sName, CTimeoutEventHub *pEventHub, IHeartbeat *pHeartbeat, CTimeSliceFormatBase *pFormat);
	virtual ~CTimeSlicerBase();

	virtual void				StartTick(ON_TIME_SLICER_TIMEOUT onBegin, ON_TIME_SLICER_TIMEOUT onEnd);
	virtual void				StopTick();

	virtual bool				IsTicking() {
		return (nullptr != _timer);
	}

	virtual void				AllocTimeSlicePointerByTime(const time_t tmDaypoint, time_slicer_cycle_t& outCycle, unsigned int& uOutSlicePointer);
	virtual void				AllocNextSlicePointer(time_slicer_cycle_t& outCycle, unsigned int& uOutSlicePointer);

	virtual time_slice_t *		GetSliceByTimeInMs(uint64_t uNowInMs);

	virtual time_slice_t *		GetNowSlice() {
		uint64_t uNowInMs = _refHeartbeat->GetTimer().GetNowSystemTimeInMs();
		return GetSliceByTimeInMs(uNowInMs);
	}
	
	virtual int					GetSliceSize() {
		return (int)_refFormat->_vSlice.size();
	}

	virtual time_slice_t *		GetSliceAt(unsigned int uSlicePointer) {
		if (uSlicePointer > 0 && uSlicePointer <= _refFormat->_vSlice.size()) {
			return &_refFormat->_vSlice[uSlicePointer - 1];
		}
		return nullptr;
	}

	virtual time_t				GetSliceBeginTime(time_slice_t *slice);
	virtual time_t				GetSliceEndTime(time_slice_t *slice);

	virtual bool				IsTimeInSlicePeriod(time_t tmCheck, time_slice_t *slice) {
		return GetSliceBeginTime(slice) <= tmCheck
			&& tmCheck < GetSliceEndTime(slice);
	}

	virtual bool				IsTimeInMsInSlicePeriod(uint64_t uNowInMs, time_slice_t *slice) {
		return (uint64_t)1000 * GetSliceBeginTime(slice) <= uNowInMs
			&& uNowInMs < (uint64_t)1000 * GetSliceEndTime(slice);
	}

public:
	std::string _sName;
	CTimeoutEventHub *_refEventHub;
	IHeartbeat *_refHeartbeat;
	CTimeSliceFormatBase *_refFormat;

	CTimeSlicerOp *_timer = nullptr;
};

/*EOF*/
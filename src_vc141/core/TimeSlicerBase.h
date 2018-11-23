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
	CTimeSliceFormatBase() { }
	virtual ~CTimeSliceFormatBase() { };

	TIME_SLICE_LIST _vSlice;

	virtual time_slice_t *		AddFormat(const char *sBeginTime, int nPeriod = -1) override;
	virtual time_slice_t *		AddFormat(const char *sBeginTime, const char *sEndTime) override;
	virtual void				AdjustSliceEndTime() override;

	virtual int					GetSliceSize() const override {
		return (int)_vSlice.size();
	}

	virtual const time_slice_t *	GetSliceAt(unsigned int uSlicePointer) const override {
		if (uSlicePointer > 0 && uSlicePointer <= _vSlice.size()) {
			return &_vSlice[uSlicePointer - 1];
		}
		return nullptr;
	}

	virtual const time_slice_t *	GetSliceByTime(time_t tmCheck) const override;

	virtual time_t				GetSliceBeginTime(const time_slice_t *slice, time_t tmCheck) const override;
	virtual time_t				GetSliceEndTime(const time_slice_t *slice, time_t tmCheck) const override;

	virtual unsigned int		FindSlicePointerByTime(const time_t tmCheck, time_slicer_cycle_t& outCycle) const override;
	virtual unsigned int		NextSlicePointer(const unsigned int uSlicePointer, time_slicer_cycle_t& inOutCycle) const override;

	virtual bool				IsTimeInSlicePeriod(time_t tmCheck, const time_slice_t *slice, time_t tmNow) const override {
		return GetSliceBeginTime(slice, tmNow) <= tmCheck
			&& tmCheck < GetSliceEndTime(slice, tmNow);
	}

public:
	static unsigned int			GetValidFormatEndTime(const time_slice_t *slice, const unsigned int TIME_MAX) {
		return (0 == slice->_end_time) ? TIME_MAX : slice->_end_time;
	}

	static bool					IsBetweenPeriod(const time_t tmCheck, time_t tmBase, unsigned int uBeginTime, unsigned int uEndTime) {
		return (time_t)(tmBase + uBeginTime) <= tmCheck
			&& tmCheck < (time_t)(tmBase + uEndTime);
	}
};

//------------------------------------------------------------------------------
/**
@brief CTimeSlicerBase
*/
class CTimeSlicerBase : public ITimeSlicer {
public:
	CTimeSlicerBase(const char *sName, CTimeoutEventHub *pEventHub, IHeartbeat *pHeartbeat, const CTimeSliceFormatBase *pFormat);
	virtual ~CTimeSlicerBase();

	virtual void				StartTick(ON_TIME_SLICER_TIMEOUT onBegin, ON_TIME_SLICER_TIMEOUT onEnd) override;
	virtual void				StopTick() override;

	virtual bool				IsTicking() const override {
		return (nullptr != _timer);
	}

	virtual const ITimeSliceFormat *	GetSliceFormat() const override {
		return _refFormat;
	}

	virtual const time_slice_t *	GetNowSlice() const override {
		time_t tmNow = _refHeartbeat->GetTimer().GetNowSystemTime();
		return GetSliceByTime(tmNow);
	}

	virtual const time_slice_t *	GetSliceByTime(time_t tmCheck) const override;

	virtual time_t				GetSliceBeginTime(const time_slice_t *slice) const override;
	virtual time_t				GetSliceEndTime(const time_slice_t *slice) const override;

	virtual bool				IsTimeInSlicePeriod(time_t tmCheck, const time_slice_t *slice) const override {
		return GetSliceBeginTime(slice) <= tmCheck
			&& tmCheck < GetSliceEndTime(slice);
	}

public:
	std::string _sName;
	CTimeoutEventHub *_refEventHub;
	IHeartbeat *_refHeartbeat;
	const CTimeSliceFormatBase *_refFormat;

	CTimeSlicerOp *_timer = nullptr;
};

/*EOF*/
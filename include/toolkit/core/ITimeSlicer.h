#pragma once
//------------------------------------------------------------------------------
/**
	@class ITimeSlicer

	(C) 2016 n.lee
*/
#include <functional>

//////////////////////////////////////////////////////////////////////////
struct time_slice_t;
using ON_TIME_SLICER_TIMEOUT = std::function<void(time_slice_t *)>;

//
class CTimeSlicer;
struct time_slice_t {
	uint32_t		_id;
	unsigned int	_begin_time; // in seconds
	unsigned int	_end_time;

	char			_begin_time_desc[16];
	char			_end_time_desc[16];
	char			_timeslice_desc[16];

	bool			_enable;
	bool			_use_cycle_end;
};

struct time_slicer_cycle_t {
	uint64_t		_base;
	unsigned int	_timemin;
	unsigned int	_timemax;
};

//------------------------------------------------------------------------------
/**
@brief ITimeSliceFormat
*/
class ITimeSliceFormat {
public:
	virtual ~ITimeSliceFormat() { }

	virtual time_slice_t *		AddFormat(const char *sBeginTime, int nPeriod) = 0;
	virtual time_slice_t *		AddFormat(const char *sBeginTime, const char *sEndTime) = 0;

	virtual bool				AllocCycle(const time_t tmDaypoint, time_slicer_cycle_t& outCycle) = 0;
	virtual bool				AllocNextCycle(const time_slicer_cycle_t& cycle, time_slicer_cycle_t& outCycle) = 0;
};

//------------------------------------------------------------------------------
/**
@brief ITimeSlicer
*/
class ITimeSlicer {
public:
	virtual ~ITimeSlicer() { }

	virtual void				StartTick(ON_TIME_SLICER_TIMEOUT dBegin, ON_TIME_SLICER_TIMEOUT dEnd) = 0;
	virtual void				StopTick() = 0;
	virtual bool				IsTicking() = 0;

	virtual void				AllocTimeSlicePointerByTime(const time_t tmDaypoint, time_slicer_cycle_t& outCycle, unsigned int& uOutSlicePointer) = 0;
	virtual void				AllocNextSlicePointer(time_slicer_cycle_t& outCycle, unsigned int& uOutSlicePointer) = 0;

	virtual time_slice_t *		GetSliceByTimeInMs(uint64_t uNowInMs) = 0;
	virtual time_slice_t *		GetNowSlice() = 0;
	virtual int					GetSliceSize() = 0;
	virtual time_slice_t *		GetSliceAt(unsigned int uSlicePointer) = 0;
	virtual time_t				GetSliceBeginTime(time_slice_t *slice) = 0;
	virtual time_t				GetSliceEndTime(time_slice_t *slice) = 0;

	virtual bool				IsTimeInSlicePeriod(time_t tmCheck, time_slice_t *slice) = 0;
	virtual bool				IsTimeInMsInSlicePeriod(uint64_t uNowInMs, time_slice_t *slice) = 0;
};

/*EOF*/
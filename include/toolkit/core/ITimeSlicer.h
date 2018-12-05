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
};

struct time_slicer_cycle_t {
	time_t			_base;
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
	virtual void				AdjustSliceEndTime() = 0;

	virtual const time_slice_t *	AllocateSlice(time_t tmCheck, time_slicer_cycle_t& outCycle, bool& outAwake, bool& outFollow) const = 0;

	virtual int					GetSliceSize() const = 0;

	virtual const time_slice_t *	GetSliceAt(unsigned int uSlicePointer) const = 0;

	virtual unsigned int		FindSlicePointerByTime(const time_t tmCheck, time_slicer_cycle_t& outCycle, bool& outAwake, bool& outFollow) const = 0;
	virtual unsigned int		NextSlicePointer(const unsigned int uSlicePointer, time_slicer_cycle_t& inOutCycle) const = 0;

	/* cycle */
	virtual void				ResetCycle(const time_t tmCheck, time_slicer_cycle_t& outCycle) const = 0;
	virtual void				RollCycleBase(time_slicer_cycle_t& inOutCycle) const = 0;

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
	virtual bool				IsTicking() const = 0;

	virtual const ITimeSliceFormat *	GetSliceFormat() const = 0;

	virtual const time_slice_t *	GetNowSlice() const = 0;
	virtual const time_slice_t *	GetSliceByTime(time_t tmCheck, time_slicer_cycle_t& outCycle) const = 0;

	virtual time_t				GetSliceBeginTime(const time_slice_t *slice, time_t tmClue) const = 0;
	virtual time_t				GetSliceBeginTime(const time_slice_t *slice, const time_slicer_cycle_t *cycle) const = 0;

	virtual time_t				GetSliceEndTime(const time_slice_t *slice, time_t tmClue) const = 0;
	virtual time_t				GetSliceEndTime(const time_slice_t *slice, const time_slicer_cycle_t *cycle) const = 0;

	virtual bool				IsTimeInSlicePeriod(time_t tmCheck, const time_slice_t *slice, time_t tmClue) const = 0;
	virtual bool				IsTimeInSlicePeriod(time_t tmCheck, const time_slice_t *slice, const time_slicer_cycle_t *cycle) const = 0;

};

/*EOF*/
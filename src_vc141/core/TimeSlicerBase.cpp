//------------------------------------------------------------------------------
//  TimeSlicerBase.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "TimeSlicerBase.h"

#include "../UsingCapnp.h"

#include "MyMacros.h"
#include "../core/snprintf/mysnprintf.h"
#include "../utils/MyUtil.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif
#endif

class CTimeSlicerOp {
public:
	kj::PromiseFulfiller<void>& _fulfiller;
	CTimeoutEventHub& _hub;
	CTimeSlicerBase& _host;
	time_slicer_cycle_t _cycle;
	unsigned int _uSlicePointer;
	struct timeout _to;
	ON_TIME_SLICER_TIMEOUT _on_begin;
	ON_TIME_SLICER_TIMEOUT _on_end;
	bool _disposed = false;

	CTimeSlicerOp(kj::PromiseFulfiller<void>& fulfiller, CTimeoutEventHub& hub, CTimeSlicerBase& host, ON_TIME_SLICER_TIMEOUT on_begin, ON_TIME_SLICER_TIMEOUT on_end, uint64_t now_in_ms)
		: _fulfiller(fulfiller)
		, _hub(hub)
		, _host(host)
		, _on_begin(on_begin)
		, _on_end(on_end) {
		//
		if (_host.IsTicking()) {
			char chDesc[256];
			o_snprintf(chDesc, sizeof(chDesc), "timer(%s) is already in ticking!", _host._sName.c_str());
			fprintf(stderr, chDesc);
			system("pause");
			kj::throwRecoverableException(KJ_EXCEPTION(FAILED, chDesc));
		}

		//
		memset(&_cycle, 0, sizeof(_cycle));
		memset(&_to, 0, sizeof(_to));
		_host._timer = this;

		Reset(now_in_ms);

		//
		FireBeginEvent();
	}

	~CTimeSlicerOp() {

		if (!_disposed
			&& nullptr != _host._timer) {
			_hub.DelTimeout(&_to);
			_host._timer = nullptr;
		}
	}

	void FulfillAndDelEvent() {

		_fulfiller.fulfill();

		// delete event handle
		_hub.DelTimeout(&_to);

		_host._timer = nullptr;
		_disposed = true;
	}

	void Reset(uint64_t uNowInMs) {
		// 
		time_t tmDaypoint = uNowInMs / (time_t)1000;
		_host.AllocTimeSlicePointerByTime(tmDaypoint, _cycle, _uSlicePointer);
	}

	void FireBeginEvent() {
		
		assert(!_hub.IsTimeoutTicking(&_to));

		if (_uSlicePointer > 0) {
			time_slice_t *slice = _host.GetSliceAt(_uSlicePointer);
			if (slice) {

				uint64_t uExpireAbsTimeInMs = (uint64_t)1000 * _host.GetSliceBeginTime(slice);
				_hub.InitExpireTimeout(&_to);
				_hub.AddTimeoutAbs(&_to, uExpireAbsTimeInMs);

				// bind callback to _to
				auto fn = [](void *arg) {
					CTimeSlicerOp *timer = (CTimeSlicerOp *)arg;
					time_slice_t *wake = timer->_host.GetSliceAt(timer->_uSlicePointer);
					if (wake) {
						timer->_on_begin(wake);

						//
						timer->FireEndEvent();
					}
				};
				timeout_setcb(&_to, std::move(fn), this);
			}
		}
	}

	void FireEndEvent() {

		assert(!_hub.IsTimeoutTicking(&_to));

		if (_uSlicePointer > 0) {
			time_slice_t *slice = _host.GetSliceAt(_uSlicePointer);
			if (slice) {

				uint64_t uExpireAbsTimeInMs = (uint64_t)1000 * _host.GetSliceEndTime(slice);
				_hub.InitExpireTimeout(&_to);
				_hub.AddTimeoutAbs(&_to, uExpireAbsTimeInMs);

				// bind callback to _to
				auto fn = [](void *arg) {
					CTimeSlicerOp *timer = (CTimeSlicerOp *)arg;
					time_slice_t *wake = timer->_host.GetSliceAt(timer->_uSlicePointer);
					if (wake) {
						timer->_on_end(wake);

						//
						timer->NextSlicePointer();
						timer->FireBeginEvent();
					}
				};
				timeout_setcb(&_to, std::move(fn), this);
			}
		}
	}

	void NextSlicePointer() {
		//
		_host.AllocNextSlicePointer(_cycle, _uSlicePointer);
	}
};

//------------------------------------------------------------------------------
/**

*/
time_slice_t *
CTimeSliceFormatBase::AddFormat(const char *sBeginTime, int nPeriod) {

	_vSlice.resize(_vSlice.size() + 1);

	int nId = _vSlice.size();
	time_slice_t *slice = &_vSlice[nId - 1];
	slice->_id = nId;
	slice->_enable = true;

	// begin time and end time
	{
		char *chArray[4];
		char chTime[10];
		int nLen, n;
		int nHour;
		int nHourLeft;
		int nMinute;

		o_snprintf(chTime, sizeof(chTime), "%s", sBeginTime);
		nLen = strlen(chTime);
		n = split((char *)chTime, nLen, chArray, 2, ':');
		slice->_begin_time = atoi(chArray[0]) * HOUR_SECONDS + atoi(chArray[1]) * 60;
		o_snprintf(slice->_begin_time_desc, sizeof(slice->_begin_time_desc), "%s", sBeginTime);

		// nPeriod < 0 means the end equals the whole cycle end.
		if (nPeriod >= 0) {
			slice->_end_time = slice->_begin_time + nPeriod;
			slice->_use_cycle_end = false;

			nHour = slice->_end_time / HOUR_SECONDS;
			nHourLeft = slice->_end_time % HOUR_SECONDS;
			nMinute = nHourLeft / 60;

			o_snprintf(slice->_end_time_desc, sizeof(slice->_end_time_desc), "%02d:%02d", nHour, nMinute);
		}
		else {
			slice->_end_time = 0;
			slice->_use_cycle_end = true;

			o_snprintf(slice->_end_time_desc, sizeof(slice->_end_time_desc), "?");
		}

		o_snprintf(slice->_timeslice_desc, sizeof(slice->_timeslice_desc), "%s-%s", slice->_begin_time_desc, slice->_end_time_desc);
	}
	return slice;
}

//------------------------------------------------------------------------------
/**

*/
time_slice_t *
CTimeSliceFormatBase::AddFormat(const char *sBeginTime, const char *sEndTime) {

	_vSlice.resize(_vSlice.size() + 1);

	int nId = _vSlice.size();
	time_slice_t *slice = &_vSlice[nId - 1];
	slice->_id = nId;
	slice->_enable = true;

	// begin time and end time
	{
		char *chArray[4];
		char chTime[10];
		int nLen, n;

		o_snprintf(chTime, sizeof(chTime), "%s", sBeginTime);
		nLen = strlen(chTime);
		n = split((char *)chTime, nLen, chArray, 2, ':');
		slice->_begin_time = atoi(chArray[0]) * HOUR_SECONDS + atoi(chArray[1]) * 60;
		o_snprintf(slice->_begin_time_desc, sizeof(slice->_begin_time_desc), "%s", sBeginTime);

		o_snprintf(chTime, sizeof(chTime), "%s", sEndTime);
		nLen = strlen(chTime);

		// nLen <= 0 means the end equals the whole cycle end.
		if (nLen > 0) {
			n = split((char *)chTime, nLen, chArray, 2, ':');
			slice->_end_time = atoi(chArray[0]) * HOUR_SECONDS + atoi(chArray[1]) * 60;
			slice->_use_cycle_end = false;

			o_snprintf(slice->_end_time_desc, sizeof(slice->_end_time_desc), "%s", sEndTime);
		}
		else {
			slice->_end_time = 0;
			slice->_use_cycle_end = true;

			o_snprintf(slice->_end_time_desc, sizeof(slice->_end_time_desc), "?");
		}

		o_snprintf(slice->_timeslice_desc, sizeof(slice->_timeslice_desc), "%s-%s", slice->_begin_time_desc, slice->_end_time_desc);
	}
	return slice;
}

//------------------------------------------------------------------------------
/**

*/
CTimeSlicerBase::CTimeSlicerBase(const char *sName, CTimeoutEventHub *pEventHub, IHeartbeat *pHeartbeat, CTimeSliceFormatBase *pFormat)
	: _sName(sName)
	, _refEventHub(pEventHub)
	, _refHeartbeat(pHeartbeat)
	, _refFormat(pFormat) {

}

//------------------------------------------------------------------------------
/**

*/
CTimeSlicerBase::~CTimeSlicerBase() {
	StopTick();
}

//------------------------------------------------------------------------------
/**

*/
void
CTimeSlicerBase::StartTick(ON_TIME_SLICER_TIMEOUT onBegin, ON_TIME_SLICER_TIMEOUT onEnd) {
	// create _timer
	uint64_t uNowInMs = _refHeartbeat->GetTimer().GetNowSystemTimeInMs();
	auto p1 = kj::newAdaptedPromise<void, CTimeSlicerOp>(*_refEventHub, *this, onBegin, onEnd, uNowInMs);

	assert(_timer);

	if (_timer->_uSlicePointer > 0) {
		// schedule
		_refHeartbeat->ScheduleTask(kj::mv(p1));
	}
	else {
		// warning -- invalid format
		fprintf(stderr, "\n[CTimeSlicerBase::StartTick()] name(%s), format is invalid, start tick failed -- to(0x%08Ix), flags(%d).\n",
			_sName.c_str(), (uintptr_t)&_timer->_to, _timer->_to.flags);
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CTimeSlicerBase::StopTick() {

	if (IsTicking()) {
		_timer->FulfillAndDelEvent();
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CTimeSlicerBase::AllocTimeSlicePointerByTime(const time_t tmDaypoint, time_slicer_cycle_t& outCycle, unsigned int& uOutSlicePointer) {

	uOutSlicePointer = 0; // 0 means disable

	//
	bool bSuccess = _refFormat->AllocCycle(tmDaypoint, outCycle);
	if (bSuccess) {
		bool bWake = false;
		bool bFollow = false;

		// walk
		time_slice_t *slice;
		bool bTail;
		int i;
		int nCount = _refFormat->_vSlice.size();
		for (i = 0; i < nCount; ++i) {
			slice = &(_refFormat->_vSlice[i]);
			bTail = (i == nCount - 1);

			// check time slice
			if (slice
				&& slice->_enable) {
				// check wake
				if (CTimeSliceFormatBase::IsBetweenPeriod(tmDaypoint, outCycle._base, slice->_begin_time, slice->_end_time)) {
					uOutSlicePointer = slice->_id;
					bWake = true;
					break;
				}
				else {
					// check follow
					if (CTimeSliceFormatBase::IsBetweenPeriod(tmDaypoint, outCycle._base, 0, slice->_begin_time)) {
						uOutSlicePointer = slice->_id;
						bFollow = true;
						break;
					}
				}

				if (bTail
					&& !bWake
					&& !bFollow) {
					// follow must be next cycle's first item
					if (_refFormat->AllocNextCycle(outCycle, outCycle)) {
						uOutSlicePointer = 1;
						bFollow = true;
					}
					else {
						//
						fprintf(stderr, "[CTimeSlicerBase::AllocTimeSlicePointerByTime()] name(%s), alloc next cycle failed -- to(0x%08Ix), flags(%d).",
							_sName.c_str(), (uintptr_t)&_timer->_to, _timer->_to.flags);
					}
					break;
				}
			}
		}
	}
	else {
		//
		fprintf(stderr, "[CTimeSlicerBase::AllocTimeSlicePointerByTime()] name(%s), alloc cycle failed -- to(0x%08Ix), flags(%d).",
			_sName.c_str(), (uintptr_t)&_timer->_to, _timer->_to.flags);
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CTimeSlicerBase::AllocNextSlicePointer(time_slicer_cycle_t& outCycle, unsigned int& uOutSlicePointer) {
	// check for time slice pointer
	if (uOutSlicePointer >= _refFormat->_vSlice.size()) {
		if (_refFormat->AllocNextCycle(outCycle, outCycle)) {
			uOutSlicePointer = 1;
		}
		else {
			uOutSlicePointer = 0;
		}

	}
	else {
		// next
		++uOutSlicePointer;
	}
}

//------------------------------------------------------------------------------
/**

*/
time_slice_t *
CTimeSlicerBase::GetSliceByTimeInMs(uint64_t uNowInMs) {

	if (IsTicking()) {
		if (1 <= _timer->_uSlicePointer
			&& _timer->_uSlicePointer <= _refFormat->_vSlice.size()) {

			time_slice_t *slice = GetSliceAt(_timer->_uSlicePointer);
			if (IsTimeInMsInSlicePeriod(uNowInMs, slice))
				return slice;
		}
	}

	//
	time_t tmDaypoint = uNowInMs / (time_t)1000;
	time_slicer_cycle_t cycle;
	unsigned int uSlicePointer;
	AllocTimeSlicePointerByTime(tmDaypoint, cycle, uSlicePointer);
	if (1 <= uSlicePointer
		&& uSlicePointer <= _refFormat->_vSlice.size()) {
		return GetSliceAt(uSlicePointer);
	}
	return nullptr;
}

//------------------------------------------------------------------------------
/**

*/
time_t
CTimeSlicerBase::GetSliceBeginTime(time_slice_t *slice) {

	if (_timer) {
		return (time_t)(_timer->_cycle._base + slice->_begin_time);
	}
	else {
		//
		uint64_t uNowInMs = _refHeartbeat->GetTimer().GetNowSystemTimeInMs();
		time_t tmDaypoint = uNowInMs / (time_t)1000;
		time_slicer_cycle_t cycle;
		unsigned int uSlicePointer;
		AllocTimeSlicePointerByTime(tmDaypoint, cycle, uSlicePointer);
		if (1 <= uSlicePointer
			&& uSlicePointer <= _refFormat->_vSlice.size()) {
			return (time_t)(cycle._base + slice->_begin_time);
		}
	}
	return 0;
}

//------------------------------------------------------------------------------
/**

*/
time_t
CTimeSlicerBase::GetSliceEndTime(time_slice_t *slice) {

	if (_timer) {
		return (time_t)(_timer->_cycle._base + slice->_end_time);
	}
	else {
		//
		uint64_t uNowInMs = _refHeartbeat->GetTimer().GetNowSystemTimeInMs();
		time_t tmDaypoint = uNowInMs / (time_t)1000;
		time_slicer_cycle_t cycle;
		unsigned int uSlicePointer;
		AllocTimeSlicePointerByTime(tmDaypoint, cycle, uSlicePointer);
		if (1 <= uSlicePointer
			&& uSlicePointer <= _refFormat->_vSlice.size()) {
			return (time_t)(cycle._base + slice->_end_time);
		}
	}
	return 0;
}

/* -- EOF -- */
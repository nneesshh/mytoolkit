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

	CTimeSlicerOp(kj::PromiseFulfiller<void>& fulfiller, CTimeoutEventHub& hub, CTimeSlicerBase& host, ON_TIME_SLICER_TIMEOUT on_begin, ON_TIME_SLICER_TIMEOUT on_end, time_t tmnow)
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

		Reset(tmnow);

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

	void Reset(time_t tmCheck) {
		//
		bool bAwake, bFollow;
		_uSlicePointer = _host._refFormat->FindSlicePointerByTime(tmCheck, _cycle, bAwake, bFollow);
	}

	void FireBeginEvent() {
		
		assert(!_hub.IsTimeoutTicking(&_to));

		if (_uSlicePointer > 0) {
			const time_slice_t *slice = _host._refFormat->GetSliceAt(_uSlicePointer);
			if (slice) {

				uint64_t uExpireAbsTimeInMs = (uint64_t)1000 * _host.GetSliceBeginTime(slice, &_cycle);
				_hub.InitExpireTimeout(&_to);
				_hub.AddTimeoutAbs(&_to, uExpireAbsTimeInMs);

				// bind callback to _to
				auto fn = [](void *arg) {
					CTimeSlicerOp *timer = (CTimeSlicerOp *)arg;
					const time_slice_t *wake = timer->_host._refFormat->GetSliceAt(timer->_uSlicePointer);
					if (wake) {
						timer->_on_begin((time_slice_t *)wake);

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
			const time_slice_t *slice = _host._refFormat->GetSliceAt(_uSlicePointer);
			if (slice) {

				uint64_t uExpireAbsTimeInMs = (uint64_t)1000 * _host.GetSliceEndTime(slice, &_cycle);
				_hub.InitExpireTimeout(&_to);
				_hub.AddTimeoutAbs(&_to, uExpireAbsTimeInMs);

				// bind callback to _to
				auto fn = [](void *arg) {
					CTimeSlicerOp *timer = (CTimeSlicerOp *)arg;
					const time_slice_t *wake = timer->_host._refFormat->GetSliceAt(timer->_uSlicePointer);
					if (wake) {
						timer->_on_end((time_slice_t *)wake);

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
		_uSlicePointer = _host._refFormat->NextSlicePointer(_uSlicePointer, _cycle);
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

			nHour = slice->_end_time / HOUR_SECONDS;
			nHourLeft = slice->_end_time % HOUR_SECONDS;
			nMinute = nHourLeft / 60;

			o_snprintf(slice->_end_time_desc, sizeof(slice->_end_time_desc), "%02d:%02d", nHour, nMinute);
		}
		else {
			slice->_end_time = 0;

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

			o_snprintf(slice->_end_time_desc, sizeof(slice->_end_time_desc), "%s", sEndTime);
		}
		else {
			slice->_end_time = 0;

			o_snprintf(slice->_end_time_desc, sizeof(slice->_end_time_desc), "?");
		}

		o_snprintf(slice->_timeslice_desc, sizeof(slice->_timeslice_desc), "%s-%s", slice->_begin_time_desc, slice->_end_time_desc);
	}
	return slice;
}

//------------------------------------------------------------------------------
/**

*/
void
CTimeSliceFormatBase::AdjustSliceEndTime() {

	time_slice_t *slice_pre = nullptr;
	for (auto& it : _vSlice) {
		time_slice_t& slice = it;

		if (slice_pre
			&& (0 == slice_pre->_end_time || slice_pre->_end_time > slice_pre->_begin_time)) {
			slice_pre->_end_time = slice._begin_time;
		}
		slice_pre = &slice;
	}
}

//------------------------------------------------------------------------------
/**

*/
const time_slice_t *
CTimeSliceFormatBase::AllocateSlice(time_t tmCheck, time_slicer_cycle_t& outCycle, bool& outAwake, bool& outFollow) const {
	//
	time_slicer_cycle_t cycle;
	bool bAwake, bFollow;
	unsigned int uSlicePointer = FindSlicePointerByTime(tmCheck, cycle, outAwake, outFollow);
	if (1 <= uSlicePointer
		&& uSlicePointer <= _vSlice.size()) {
		return GetSliceAt(uSlicePointer);
	}

	bAwake = false;
	bFollow = false;
	return nullptr;
}

//------------------------------------------------------------------------------
/**

*/
unsigned int
CTimeSliceFormatBase::FindSlicePointerByTime(const time_t tmCheck, time_slicer_cycle_t& outCycle, bool& outAwake, bool& outFollow) const {
	// 0 means disable
	unsigned int uSlicePointer = 0;

	//
	ResetCycle(tmCheck, outCycle);

	//
	outAwake = false;
	outFollow = false;

	// walk
	const time_slice_t *slice;
	bool bTail;
	int i;
	int nCount = _vSlice.size();
	for (i = 0; i < nCount; ++i) {
		slice = &_vSlice[i];
		bTail = (i == nCount - 1);

		// check time slice
		if (slice
			&& slice->_enable) {
			// valid format end time
			unsigned int uEndTime = CTimeSliceFormatBase::GetValidFormatEndTime(slice, outCycle._timemax);

			// check wake
			if (IsBetweenPeriod(tmCheck, outCycle._base, slice->_begin_time, uEndTime)) {
				uSlicePointer = slice->_id;
				outAwake = true;
				break;
			}
			else {
				// check follow
				if (CTimeSliceFormatBase::IsBetweenPeriod(tmCheck, outCycle._base, 0, slice->_begin_time)) {
					uSlicePointer = slice->_id;
					outFollow = true;
					break;
				}
			}

			if (bTail
				&& !outAwake
				&& !outFollow) {
				// follow must be next cycle's first item
				RollCycleBase(outCycle);

				uSlicePointer = 1;
				outFollow = true;
				break;
			}
		}
	}
	return uSlicePointer;
}

//------------------------------------------------------------------------------
/**

*/
unsigned int
CTimeSliceFormatBase::NextSlicePointer(const unsigned int uSlicePointer, time_slicer_cycle_t& inOutCycle) const {
	if (uSlicePointer >= _vSlice.size()) {
		//
		RollCycleBase(inOutCycle);
		return 1;
	}
	return uSlicePointer + 1;
}

//------------------------------------------------------------------------------
/**

*/
CTimeSlicerBase::CTimeSlicerBase(const char *sName, CTimeoutEventHub *pEventHub, IHeartbeat *pHeartbeat, const CTimeSliceFormatBase *pFormat)
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
	time_t tmNow = _refHeartbeat->GetTimer().GetNowSystemTime();
	auto p1 = kj::newAdaptedPromise<void, CTimeSlicerOp>(*_refEventHub, *this, onBegin, onEnd, tmNow);

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
const time_slice_t *
CTimeSlicerBase::GetSliceByTime(time_t tmCheck, time_slicer_cycle_t& outCycle) const {

	if (IsTicking()) {
		if (1 <= _timer->_uSlicePointer
			&& _timer->_uSlicePointer <= _refFormat->_vSlice.size()) {

			const time_slice_t *slice = _refFormat->GetSliceAt(_timer->_uSlicePointer);
			if (IsTimeInSlicePeriod(tmCheck, slice, &_timer->_cycle)) {
				outCycle = _timer->_cycle;
				return slice;
			}
		}
		return nullptr;
	}

	//
	bool bAwake, bFollow;
	const time_slice_t *slice = _refFormat->AllocateSlice(tmCheck, outCycle, bAwake, bFollow);
	if (bAwake) {
		return slice;
	}
	return nullptr;
}

//------------------------------------------------------------------------------
/**

*/
time_t
CTimeSlicerBase::GetSliceBeginTime(const time_slice_t *slice, time_t tmClue) const {

	if (IsTicking()) {
		return GetSliceBeginTime(slice, &_timer->_cycle);
	}

	//
	if (tmClue > 0) {
		// find slice cycle
		time_slicer_cycle_t cycle;
		bool bAwake, bFollow;
		unsigned int uSlicePointer = _refFormat->FindSlicePointerByTime(tmClue, cycle, bAwake, bFollow);

		// ignore awake-follow value, we just use cycle
		if (uSlicePointer > 0) {
			return GetSliceBeginTime(slice, &cycle);
		}
	}

	return 0;
}

//------------------------------------------------------------------------------
/**

*/
time_t
CTimeSlicerBase::GetSliceEndTime(const time_slice_t *slice, time_t tmClue) const {

	if (IsTicking()) {
		return GetSliceEndTime(slice, &_timer->_cycle);
	}

	//
	if (tmClue > 0) {
		// find slice cycle
		time_slicer_cycle_t cycle;
		bool bAwake, bFollow;
		unsigned int uSlicePointer = _refFormat->FindSlicePointerByTime(tmClue, cycle, bAwake, bFollow);

		// ignore awake-follow value, we just use cycle
		if (uSlicePointer > 0) {
			return GetSliceEndTime(slice, &cycle);
		}
	}

	return 0;
}

//------------------------------------------------------------------------------
/**

*/
bool
CTimeSlicerBase::IsTimeInSlicePeriod(time_t tmCheck, const time_slice_t *slice, time_t tmClue) const {

	if (IsTicking()) {
		return IsTimeInSlicePeriod(tmCheck, slice, &_timer->_cycle);
	}

	//
	if (tmClue > 0) {
		// find slice cycle
		time_slicer_cycle_t cycle;
		bool bAwake, bFollow;
		unsigned int uSlicePointer = _refFormat->FindSlicePointerByTime(tmClue, cycle, bAwake, bFollow);

		// ignore awake-follow value, we just use cycle
		if (uSlicePointer > 0) {
			return IsTimeInSlicePeriod(tmCheck, slice, &cycle);
		}
	}
	return false;
}

/* -- EOF -- */
//------------------------------------------------------------------------------
//  TimeTicker.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "TimeTicker.h"

#include "../UsingCapnp.h"

#include "../core/snprintf/mysnprintf.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif
#endif

class CTimeTickerOp {
public:
	kj::PromiseFulfiller<bool>& _fulfiller;
	CTimeoutEventHub& _hub;
	CTimeTicker& _host;
	uint32_t _id;
	CTimeTicker::TIME_TICKER_TIMEOUT_CB _cb = nullptr;
	struct timeout _to;
	MAP_TIME_TICKER_OP::iterator _pos;
	bool _disposed = false;

	CTimeTickerOp(kj::PromiseFulfiller<bool>& fulfiller, CTimeoutEventHub& hub, CTimeTicker& host, uint32_t id, unsigned int expire_time_in_ms)
		: _fulfiller(fulfiller)
		, _hub(hub)
		, _host(host)
		, _id(id)
		, _pos(host._mapTimeTickerOp.end()) {

		if (_host.TimerExists(_id)) {
			char chDesc[256];
			o_snprintf(chDesc, sizeof(chDesc), "timer(%s)(%d) is still ticking!", _host._sName.c_str(), _id);
			fprintf(stderr, chDesc);
			system("pause");
			kj::throwRecoverableException(KJ_EXCEPTION(FAILED, chDesc));
		}

		std::pair<MAP_TIME_TICKER_OP::iterator, bool> ret =
			_host._mapTimeTickerOp.insert(std::make_pair(_id, this));
		if (ret.second) {
			_pos = ret.first;
		}

		// schedule _to
		_hub.InitExpireTimeout(&_to);
		_hub.AddTimeout(&_to, expire_time_in_ms);

		// bind callback to _to
		auto fn = [](void *arg) {
			CTimeTickerOp *timer = (CTimeTickerOp *)arg;
			timer->Fulfill(false); // not canceled, it is normal timeout, 
		};
		timeout_setcb(&_to, std::move(fn), this);
	}

#ifndef TIMEOUT_DISABLE_INTERVALS
	CTimeTickerOp(kj::PromiseFulfiller<bool>& fulfiller, CTimeoutEventHub& hub, CTimeTicker& host, uint32_t id, unsigned int interval_time_in_ms, CTimeTicker::TIME_TICKER_TIMEOUT_CB cb)
		: _fulfiller(fulfiller)
		, _hub(hub)
		, _host(host)
		, _id(id)
		, _cb(cb)
		, _pos(host._mapTimeTickerOp.end())
		, _disposed(false) {

		if (_host.TimerExists(_id)) {
			char chDesc[256];
			o_snprintf(chDesc, sizeof(chDesc), "timer(%s)(%d) is still ticking.", _host._sName.c_str(), _id);
			fprintf(stderr, chDesc);
			system("pause");
			kj::throwRecoverableException(KJ_EXCEPTION(FAILED, chDesc));
		}

		std::pair<MAP_TIME_TICKER_OP::iterator, bool > ret =
			_host._mapTimeTickerOp.insert(std::make_pair(_id, this));
		if (ret.second) {
			_pos = ret.first;
		}

		// schedule _to
		_hub.InitIntervalTimeout(&_to);
		_hub.AddTimeout(&_to, interval_time_in_ms);

		// bind callback to _to
		auto fn = [](void *arg) {
			CTimeTickerOp *timer = (CTimeTickerOp *)arg;
			timer->_cb(timer->_id);
		};
		timeout_setcb(&_to, std::move(fn), this);
	}
#endif

	~CTimeTickerOp() {
		if (!_disposed
			&& _pos != _host._mapTimeTickerOp.end()) {
			_hub.DelTimeout(&_to);
			_host._mapTimeTickerOp.erase(_pos);
		}
	}

	void Fulfill(bool bCanceled) {
		_fulfiller.fulfill(kj::mv(bCanceled));

		// we must delete event handle if timer is canceled instead of normal timeout
		if (bCanceled)
			_hub.DelTimeout(&_to);

		_host._mapTimeTickerOp.erase(_pos);
		_pos = _host._mapTimeTickerOp.end();
		_disposed = true;
	}

};

//------------------------------------------------------------------------------
/**

*/
CTimeTicker::CTimeTicker(const char *sName, CTimeoutEventHub *pEventHub, IHeartbeat *pHeartbeat)
	: _sName(sName)
	, _refEventHub(pEventHub)
	, _refHeartbeat(pHeartbeat) {

}

//------------------------------------------------------------------------------
/**

*/
CTimeTicker::~CTimeTicker() {
	CancelAllTimers();
}

//------------------------------------------------------------------------------
/**

*/
void
CTimeTicker::SetExpireTimerInMs(uint32_t id, unsigned int expire_time_in_ms, TIME_TICKER_TIMEOUT_CB cb) {
	auto p1 = kj::newAdaptedPromise<bool, CTimeTickerOp>(*_refEventHub, *this, id, expire_time_in_ms)
		.then([id, cb](bool bCancelled) {

		if (!bCancelled) {
			cb(id);
		}
	});
	_refHeartbeat->ScheduleTask(kj::mv(p1));
}

//------------------------------------------------------------------------------
/**

*/
#ifndef TIMEOUT_DISABLE_INTERVALS
void
CTimeTicker::SetIntervalTimerInMs(uint32_t id, unsigned int interval_time_in_ms, TIME_TICKER_TIMEOUT_CB cb) {
	auto p1 = kj::newAdaptedPromise<bool, CTimeTickerOp>(*_refEventHub, *this, id, interval_time_in_ms, cb)
		.then([](bool) {
		// do nothing
	});
	_refHeartbeat->ScheduleTask(kj::mv(p1));
}
#endif

//------------------------------------------------------------------------------
/**

*/
void
CTimeTicker::CancelTimer(uint32_t id) {
	MAP_TIME_TICKER_OP::iterator it = _mapTimeTickerOp.find(id);
	if (it != _mapTimeTickerOp.end()) {
		CTimeTickerOp *timer = (it->second);
		timer->Fulfill(true); // canceled, not normal timeout
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CTimeTicker::CancelAllTimers() {
	for (;;) {
		MAP_TIME_TICKER_OP::iterator it = _mapTimeTickerOp.begin();
		if (it == _mapTimeTickerOp.end()) {
			break;
		}
		else {
			CTimeTickerOp *timer = (it->second);
			timer->Fulfill(true); // canceled, not normal timeout
		}
	}
}

/* -- EOF -- */
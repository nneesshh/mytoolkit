#pragma once
//------------------------------------------------------------------------------
/**
@class CMyLuaTimer

(C) 2011 n.lee
*/
#include <unordered_map>
#include <functional>

#include "../core/TimeoutEventHub.h"
#include "../core/timingwheel/timeout/timeout.h"

#include "MyLua.h"

namespace luacpp {

//////////////////////////////////////////////////////////////////////////
enum MYLUA_TIMEOUT_EVENT_STATE {
	MYLUA_TIMEOUT_EVENT_STATE_IDLE = 0,
	MYLUA_TIMEOUT_EVENT_STATE_TICKING = 1,
};

struct mylua_timeout_event_t;
using MAP_MYLUA_TIMEOUT_EVENT = std::unordered_map<uint32_t, mylua_timeout_event_t>;

//
class CMyLuaTimer;
struct mylua_timeout_event_t {

	uint32_t					_id;
	int							_lua_func_refid;
	CMyLuaTimer					*_host;
	MYLUA_TIMEOUT_EVENT_STATE	_state;

	struct timeout				_to;

};

//------------------------------------------------------------------------------
/**
@brief CMyLuaTimer
*/
class	CMyLuaTimer {
public:
	CMyLuaTimer(CMyLua& mylua, CTimeoutEventHub& eventHub);
	~CMyLuaTimer();

	void						SetExpireEventInMs(uint32_t id, unsigned int expire_time_in_ms, const int func_refid);

#ifndef TIMEOUT_DISABLE_INTERVALS
	void						SetIntervalEventInMs(uint32_t id, unsigned int interval_time_in_ms, const int func_refid);
#endif

	virtual bool				IsTimeoutEventTicking(uint32_t id) {
		mylua_timeout_event_t *evt_ = SearchTimeoutEvent(id);
		return (evt_ && MYLUA_TIMEOUT_EVENT_STATE_TICKING == evt_->_state);
	}

	void						KillTimeoutEvent(uint32_t id) {
		MAP_MYLUA_TIMEOUT_EVENT::iterator it = _mapTimeoutEvent.find(id);
		if (it != _mapTimeoutEvent.end()) {
			mylua_timeout_event_t& evt_ = it->second;
			_refEventHub.DelTimeout(&evt_._to);

			// mark idle -- we keep this event in queue, we never do removing!!!
			evt_._state = MYLUA_TIMEOUT_EVENT_STATE_IDLE;
		}
	}

	CMyLua&						MyLua() {
		return _refMyLua;
	}

public:
	static void					ExportApi2Lua(CMyLuaTimer *timer);

private:
	mylua_timeout_event_t *		SearchTimeoutEvent(uint32_t id) {
		MAP_MYLUA_TIMEOUT_EVENT::iterator it = _mapTimeoutEvent.find(id);
		if (it != _mapTimeoutEvent.end()) {
			return &it->second;
		}
		return nullptr;
	}

private:
	CMyLua& _refMyLua;
	CTimeoutEventHub& _refEventHub;

public:
	MAP_MYLUA_TIMEOUT_EVENT _mapTimeoutEvent;
};

}
/*EOF*/
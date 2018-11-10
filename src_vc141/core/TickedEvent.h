#pragma once
//------------------------------------------------------------------------------
/**
	@class CTickedEvent

	(C) 2016 n.lee
*/
#include <limits.h>
#include <stdint.h>
#include <stdio.h> 
#include <functional>

//////////////////////////////////////////////////////////////////////////
typedef struct _ticked_event_s {
	uint32_t		_id;
	uintptr_t		_paramid1; // first param id
	uint64_t		_paramid2; // second param id
	double			_ticks;
	double			_remaining_time;

	int				_last_countdown;
	int				_countdown;
} ticked_event_t;

typedef enum __TICKING_FLAG_t {
	TICKING_BEGIN
	, TICKING
	, TICKING_END
} TICKING_FLAG;

//------------------------------------------------------------------------------
/**
@brief CTickedEvent
*/
class CTickedEvent {
public:
	CTickedEvent() {
		ResetEvent();
		_event._id = 0;
	}
	~CTickedEvent() { }

	using ON_EVENT = std::function<void(ticked_event_t *, TICKING_FLAG)>;

public:
	void OnUpdate(float fElapsed) {
		Tick(&_event, fElapsed);
	}

	ticked_event_t * Detail() {
		return &_event;
	}

	bool TestEvent(uint32_t eventid) {
		return (_event._id == eventid) && IsTicking(&_event);
	}

	bool TestEventIdle(uint32_t eventid) {
		return (_event._id == eventid) && !IsTicking(&_event);
	}

	void RestTicks(int ticks) {
		_event._ticks = (double)ticks;
		_event._remaining_time = _event._ticks;
		_event._countdown = ticks;
		_event._last_countdown = 0;
	}

	void ResetEvent() {
		_event._ticks = 0.0;
		_event._remaining_time = 0.0;
		_event._countdown = -1;
		_event._last_countdown = 0;
	}

	void SetEvent(uint32_t eventid, uintptr_t paramid1, uint64_t paramid2, int ticks) {
		_event._id = eventid;
		_event._paramid1 = paramid1;
		_event._paramid2 = paramid2;

		RestTicks(ticks);

		//
		if (_onTickingFlagChanged)
			_onTickingFlagChanged(&_event, TICKING_BEGIN);
	}

	void RegisterEventHandler(ON_EVENT d) {
		_onTickingFlagChanged = d;
	}

private:
	bool IsTicking(ticked_event_t *evt) {
		return (evt->_countdown >= 0 && evt->_countdown < INT_MAX);
	}

	void Tick(ticked_event_t *evt, float elapsed) {
		if (IsTicking(evt)) {
			//
			evt->_remaining_time -= (double)elapsed;
			if (evt->_remaining_time <= 0.0) {
				//
				if (_onTickingFlagChanged)
					_onTickingFlagChanged(&_event, TICKING_END);

				//
				ResetEvent();
			}
			else {
				evt->_countdown = int(evt->_remaining_time);

				//
				if (evt->_countdown != evt->_last_countdown) {
					// debug
					fprintf(stderr, "\nid(%d)tick(%d)\n", evt->_id, evt->_countdown);

					//
					if (evt->_countdown >= 0) {
						//
						if (_onTickingFlagChanged)
							_onTickingFlagChanged(evt, TICKING);
					}

					// last count down
					evt->_last_countdown = evt->_countdown;
				}
			}
		}
	}

private:
	ticked_event_t _event;
	ON_EVENT _onTickingFlagChanged;
};

/*EOF*/
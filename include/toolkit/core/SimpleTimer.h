#pragma once
//------------------------------------------------------------------------------
/**
    @class CSimpleTimer
    
    (C) 2016 n.lee
*/
#include <stdint.h>
#include <time.h>
#include <chrono>

#include "../platform/utilities.h"
#include "../base/time/time.h"

//------------------------------------------------------------------------------
/** 
	@brief CSimpleTimer
 */
class CSimpleTimer {
public:
	static CSimpleTimer *		GetInstance() {
		static CSimpleTimer s_timer;
		return &s_timer;
	}

	~CSimpleTimer() { }

	///
	inline void					Reset();

	///
	inline float				GetFPS(unsigned int elapsedInMs);

	///
	inline void					SetUpdateInterval(unsigned int newIntervalInMs) {
		_updateIntervalInMs = newIntervalInMs;
	}

	inline unsigned int			GetUpdateInterval() {
		return _updateIntervalInMs;
	}

public:
	static uint64_t				GetNowSystemTimeInMs() {
		return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	static int64_t				GetElapsedInMs() {
		base::TimeDelta time_elapsed =
			base::TimeTicks::Now() - base::TimeTicks();
		return time_elapsed.InMilliseconds();
	}

private:
	CSimpleTimer() {
		Reset();
	}

protected:
	unsigned int	_lastUpdateInMs;
	float			_fps;
	unsigned int	_frames;
	unsigned int	_updateIntervalInMs;
};

//------------------------------------------------------------------------------
/**

*/
void
CSimpleTimer::Reset() {
	_lastUpdateInMs = 0;
	_fps = 1.0f;
	_frames = 0;
	_updateIntervalInMs = 500;
}

//------------------------------------------------------------------------------
/**

*/
float 
CSimpleTimer::GetFPS(unsigned int elapsedInMs) {
	++_frames;

	//
	unsigned int currentUpdateInMs = elapsedInMs;
	if(currentUpdateInMs - _lastUpdateInMs > _updateIntervalInMs) {
		_fps = 1000.0f * (float)_frames / (currentUpdateInMs - _lastUpdateInMs);
		_lastUpdateInMs = currentUpdateInMs;
		_frames = 0;
	}
	return _fps;
}

/*EOF*/
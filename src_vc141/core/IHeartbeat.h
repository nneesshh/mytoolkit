#pragma once
//------------------------------------------------------------------------------
/**
@class IHeartbeat

(C) 2016 n.lee
*/
#include <stdint.h>
#include <functional>

#include "../UsingCapnp.h"

#include "SimpleTimer.h"

//------------------------------------------------------------------------------
/**
@brief IHeartbeat
*/
class IHeartbeat {
public:
	virtual ~IHeartbeat() noexcept(false) { }

	struct stats_t {
		uint64_t _now_system_time_in_ms;
		unsigned int _total_elapsed_in_ms;

		unsigned int _elapsed_in_ms;
		unsigned int _sleep_in_ms;

		float _fps;
		float _spf;
	};

	///
	virtual void				OnStats(int nConnectionCount, int nAccountCount, int nRecvBytes, int nSendBytes) = 0;

	///
	virtual void				Run(std::function<void(unsigned int, uint64_t)> cbUpdate) = 0;
	virtual void				Exit() = 0;
	virtual struct stats_t *	Stats() = 0;

	virtual CSimpleTimer&		GetTimer() = 0;

	virtual void				ScheduleTask(kj::Promise<void>&& task) = 0;
	virtual void				StartDaemon() = 0;

	virtual kj::Promise<void>	AfterDelay(unsigned int uDelaySeconds) = 0;
	virtual kj::Promise<void>	TimeoutAfter(unsigned int uTimeoutSeconds, kj::Promise<void>&& p) = 0;
	virtual kj::Promise<bool>	TimeoutAfter(unsigned int uTimeoutSeconds, kj::Promise<bool>&& p) = 0;
	virtual kj::Promise<void>	PromiseFor() = 0;
	virtual kj::ForkedPromise<void>	ForkPromise(kj::Promise<void>&& promise) = 0;
	virtual kj::Promise<void>	AddBranch(kj::ForkedPromise<void>& forked) = 0;

};

/*EOF*/
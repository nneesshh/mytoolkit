#pragma once
//------------------------------------------------------------------------------
/**
	@class CHeartbeat

	(C) 2016 n.lee
*/
#include <signal.h>

#include "kj/KjSimpleEventLoop.h"

#include "../log/StdLog.h"
#include "IHeartbeat.h"

#define HEARTBEAT_FRAME_TIME_IN_MS	10

//------------------------------------------------------------------------------
/**
@brief CHeartbeat
*/
class CHeartbeat : public IHeartbeat {
public:
	CHeartbeat(const std::string& appname, CSimpleTimer& timer, StdLog& log);
	~CHeartbeat() noexcept(false);

	///
	virtual void				OnStats(int nConnectionCount, int nAccountCount, int nRecvBytes, int nSendBytes) override;

	///
	virtual void				Run(std::function<void(unsigned int, uint64_t)> cbUpdate) override;

	///
	virtual void				Exit() override {
		_quit = 1;
	}

	virtual struct stats_t *	Stats() override {
		return &_stats;
	}

	virtual CSimpleTimer&		GetTimer() {
		return _refTimer;
	}

	virtual void				ScheduleTask(kj::Promise<void>&& task) override {
		_tsHeartbeat->add(kj::mv(task));
	}

	virtual void				StartDaemon() override {
		_tsDaemon->add(Daemon());
	}

	virtual kj::Promise<void>	AfterDelay(unsigned int uDelaySeconds) override {
		return _context->AfterDelay(uDelaySeconds * kj::SECONDS);
	}

	virtual kj::Promise<void>	TimeoutAfter(unsigned int uTimeoutSeconds, kj::Promise<void>&& p) override;
	virtual kj::Promise<bool>	TimeoutAfter(unsigned int uTimeoutSeconds, kj::Promise<bool>&& p) override;

	virtual kj::Promise<void>	PromiseFor() override {
		return _context->EvalForResult([]() {});
	}

	virtual kj::ForkedPromise<void>	ForkPromise(kj::Promise<void>&& promise) override {
		return _context->ForkPromise<void>(kj::mv(promise));
	}

	virtual kj::Promise<void>	AddBranch(kj::ForkedPromise<void>& forked) override {
		return forked.addBranch();
	}

private:
	uint64_t					StatsNowSystemTimeInMs() {
		return _stats._now_system_time_in_ms;
	}

	unsigned int				StatsTotalElapsedInMs() {
		return _stats._total_elapsed_in_ms;
	}

	void						TimeCorrection(int offsetAddInMs) {
		_correctionOffsetInMs += offsetAddInMs;
	}

	///
	void						BeforeHeartbeat();
	void						AfterHeartbeat();

	kj::Promise<void>			Daemon();

private:
	std::string _appname;
	CSimpleTimer& _refTimer; // singleton can't cross dll (so), so we must keep the timer reference
	StdLog& _refLog;

	std::function<void(unsigned int, uint64_t)> _cbUpdate;
	volatile sig_atomic_t _quit = 0;

	kj::Own<KjSimpleEventLoop> _context = kj::refcounted<KjSimpleEventLoop>();
	kj::Own<kj::TaskSet> _tsHeartbeat = _context->CreateTaskSet();
	kj::Own<kj::TaskSet> _tsDaemon = _context->CreateTaskSet();

	unsigned int _statsOutputIntervalInMs = 0;
	int64_t _baseDiffInMs = 0;
	int _diffOffsetInMs = 0;
	int _correctionOffsetInMs = 0;
	char _nowformat[32];
	struct stats_t _stats;

	int _lastRecvBytes = 0;
	int _lastSendBytes = 0;
};

/*EOF*/
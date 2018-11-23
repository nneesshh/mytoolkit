//------------------------------------------------------------------------------
//  Heartbeat.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "Heartbeat.h"

#include <algorithm>

#include "../base/process/process_handle.h"
#include "../base/threading/platform_thread.h"

#include "SimpleCalendar.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif
#endif

static void
write_heartbeat_crash_error(const char *appname, const char *err_txt) {
	char log_file_name[256];
	char time_suffix[32];

	time_t t = time(nullptr);

#if defined(__CYGWIN__) || defined( _WIN32)
	struct tm tp;
	localtime_s(&tp, &t);
	o_snprintf(
		time_suffix
		, sizeof(time_suffix)
		, "_%d-%02d-%02d_%02d_%02d_%02d"
		, tp.tm_year + 1900
		, tp.tm_mon + 1
		, tp.tm_mday
		, tp.tm_hour
		, tp.tm_min
		, tp.tm_sec);
#else
	struct tm tp;
	localtime_r(&t, &tp);
	o_snprintf(
		time_suffix
		, sizeof(time_suffix)
		, "_%d-%02d-%02d_%02d_%02d_%02d"
		, tp.tm_year + 1900
		, tp.tm_mon + 1
		, tp.tm_mday
		, tp.tm_hour
		, tp.tm_min
		, tp.tm_sec);
#endif

	o_snprintf(log_file_name, sizeof(log_file_name), "syslog/heartbeat_crash_%s.error",
		time_suffix);

	// write to file
	FILE *ferr = fopen(log_file_name, "at+");
	fprintf(ferr, "appname:\n%s\n",
		appname);
	fprintf(ferr, "heartbeat crashed:\n%s\n",
		err_txt);
	fclose(ferr);
}

static std::function<void(unsigned int, uint64_t)> func_upate_default = [](unsigned int, uint64_t) {};

#ifdef MYTOOLKIT_NAMESPACE
using namespace  MyToolkit;
#endif

//------------------------------------------------------------------------------
/**

*/
CHeartbeat::CHeartbeat(const std::string& appname, CSimpleTimer& timer, StdLog& log)
	: _appname(appname)
	, _refTimer(timer)
	, _refLog(log) {
	//
	_nowformat[0] = '\0';

	_cbUpdate = func_upate_default;

	_stats._now_system_time_in_ms = _refTimer.GetNowSystemTimeInMs();
	_stats._total_elapsed_in_ms = _refTimer.GetElapsedInMs();

	_stats._elapsed_in_ms = 1;
	_stats._sleep_in_ms = 0;

	_stats._fps = 1.0f;
	_stats._spf = 1.0f;

#ifdef WIN32
	WSADATA WSAData;
	DWORD Ret;
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &WSAData)) != 0) {
		fprintf(stderr, "WSAStartup failed with error %d!!!\n", Ret);
		exit(-1);
	}
#endif

}

//------------------------------------------------------------------------------
/**

*/
CHeartbeat::~CHeartbeat() {
#ifdef WIN32
	DWORD Ret;
	if ((Ret = WSACleanup()) != 0) {
		fprintf(stderr, "WSACleanup failed with error %d!!!\n", Ret);
	}
#endif
}

//------------------------------------------------------------------------------
/**

*/
void
CHeartbeat::OnStats(int nConnectionCount, int nAccountCount, int nRecvBytes, int nSendBytes) {
	_statsOutputIntervalInMs += _stats._elapsed_in_ms;
	if (_statsOutputIntervalInMs >= 3000) {
		//
		uint64_t now_system_time_in_ms = StatsNowSystemTimeInMs();
		time_t tm_now_time = now_system_time_in_ms / (uint64_t)1000;
		int now_time_t_left = (int)(now_system_time_in_ms % (uint64_t)1000);

		//
		unsigned int nTotalElapsedInMs = StatsTotalElapsedInMs();
		int nHours = nTotalElapsedInMs / 3600000;
		int nHoursLeft = nTotalElapsedInMs % 3600000;
		int nMinutes = nHoursLeft / 60000;
		int nMinutesLeft = nHoursLeft % 60000;
		int nSeconds = nMinutesLeft / 1000;
		int nSecondsLeft = nMinutesLeft % 1000;

		//
		int64_t nDiffInMs = now_system_time_in_ms - nTotalElapsedInMs;
		if (_baseDiffInMs <= 0) {
			_baseDiffInMs = nDiffInMs;
		}
		_diffOffsetInMs = (int)(nDiffInMs - _baseDiffInMs) - _correctionOffsetInMs;

		//
		if (0 != _diffOffsetInMs
			&& (_diffOffsetInMs > 1000
				|| _diffOffsetInMs < -1000)) {
			// warning
			_refLog.logprint(LOG_LEVEL_WARNING, "[CHeartbeat::OnStats()] time jump found: offset=%d(ms)", _diffOffsetInMs);
			TimeCorrection(_diffOffsetInMs);
		}

		//
		CSimpleCalendar::FormatDatetime(tm_now_time, _nowformat, sizeof(_nowformat));
		fprintf(stdout, "\n[CHeartbeat::OnStats()][%s]", _appname.c_str());
		fprintf(stdout, "\n________[pid(%d)] [%04d:%02d:%02d][%03d] %s:%03d *%d/%d",
			base::GetCurrentProcId(),
			nHours, nMinutes, nSeconds, nSecondsLeft, _nowformat, now_time_t_left, _diffOffsetInMs, _correctionOffsetInMs);
		fprintf(stdout, "\n________[tid(%d)] FPS=(%.1f), Elapsed=(%d), Sleep=(%d), Capacity=c(%d)_a(%d)...R<%.1f>/S<%.1f>\n\n",
			base::PlatformThread::CurrentId(),
			_stats._fps,
			_stats._elapsed_in_ms,
			_stats._sleep_in_ms,
			nConnectionCount,
			nAccountCount,
			float(nRecvBytes - _lastRecvBytes) / _statsOutputIntervalInMs,
			float(nSendBytes - _lastSendBytes) / _statsOutputIntervalInMs);

		//
		_statsOutputIntervalInMs = 0;

		_lastRecvBytes = nRecvBytes;
		_lastSendBytes = nSendBytes;
	}
}

//------------------------------------------------------------------------------
/**

*/
void
CHeartbeat::Run(std::function<void(unsigned int, uint64_t)> cbUpdate) {

	_cbUpdate = cbUpdate;

	while (!_quit) {

		BeforeHeartbeat();
		
		// heartbeat
		{
			_cbUpdate(_stats._elapsed_in_ms, _stats._now_system_time_in_ms);
		}

		AfterHeartbeat();

		// fps delay
		_context->AfterDelay(_stats._sleep_in_ms * kj::MILLISECONDS).wait(_context->GetWaitScope());
	}
}

//------------------------------------------------------------------------------
/**

*/
kj::Promise<void>
CHeartbeat::TimeoutAfter(unsigned int uTimeoutSeconds, kj::Promise<void>&& p) {
	return _context->TimeoutAfter<void>(uTimeoutSeconds * kj::SECONDS, kj::mv(p));
}

//------------------------------------------------------------------------------
/**

*/
kj::Promise<bool>
CHeartbeat::TimeoutAfter(unsigned int uTimeoutSeconds, kj::Promise<bool>&& p) {
	return _context->TimeoutAfter<bool>(uTimeoutSeconds * kj::SECONDS, kj::mv(p));
}

//------------------------------------------------------------------------------
/**

*/
void
CHeartbeat::BeforeHeartbeat() {
	unsigned int last_total_elapsed_in_ms = _stats._total_elapsed_in_ms;

	// elapsed and fps
	_stats._now_system_time_in_ms = _refTimer.GetNowSystemTimeInMs();
	_stats._total_elapsed_in_ms = _refTimer.GetElapsedInMs();
	_stats._elapsed_in_ms = _stats._total_elapsed_in_ms - last_total_elapsed_in_ms;

	_stats._fps = _refTimer.GetFPS(_stats._total_elapsed_in_ms);
	_stats._spf = 1.0f / _stats._fps;
}

//------------------------------------------------------------------------------
/**

*/
void
CHeartbeat::AfterHeartbeat() {
	// sleep if update time is less than HEARTBEAT_FRAME_TIME_IN_MS.
	unsigned int frame_elapsed_in_ms = _stats._elapsed_in_ms + (_refTimer.GetElapsedInMs() - _stats._total_elapsed_in_ms);

	// Note: unsigned int may overflow when minus, so we should judge plus condition only.
	if (frame_elapsed_in_ms < HEARTBEAT_FRAME_TIME_IN_MS) {
		_stats._sleep_in_ms = HEARTBEAT_FRAME_TIME_IN_MS - frame_elapsed_in_ms;
	}
	else {
		_stats._sleep_in_ms = 0;
	}
}

//------------------------------------------------------------------------------
/**

*/
kj::Promise<void>
CHeartbeat::Daemon() {

#define DAEMON_INTERVAL 99
#define DAEMON_CRASH_TIMEOUT 299
	// daemon delay
	return AfterDelay(DAEMON_INTERVAL)
		.then([this]() {
		// log info to file
		_refLog.logprint(LOG_LEVEL_INFO, "\n[CHeartbeat::Daemon()] heartbeat_taskset(0x%08Ix)interval_ms(%d) -- daemon_taskset(0x%08Ix)interval(%d)\n",
			(uintptr_t)_tsHeartbeat.get(), HEARTBEAT_FRAME_TIME_IN_MS,
			(uintptr_t)_tsDaemon.get(), DAEMON_INTERVAL);

		uint64_t uNowInMs = _refTimer.GetNowSystemTimeInMs();
		uint64_t uStatsNowInMs = StatsNowSystemTimeInMs();
		uint64_t uDiff = uNowInMs - uStatsNowInMs;

		// force to dump and crash for about 5 minute
		if (uDiff > DAEMON_CRASH_TIMEOUT * 1000) {
			std::string trace_txt = _tsHeartbeat->trace().cStr();
			_refLog.logprint(LOG_LEVEL_ALERT, "\n[CHeartbeat::Daemon()] heartbeat maybe crashed!!!! heartbeat_taskset(0x%08Ix), tasks:\n%s\n",
				(uintptr_t)_tsHeartbeat.get(), trace_txt.c_str());

			write_heartbeat_crash_error(_appname.c_str(), trace_txt.c_str());
		}
		return Daemon();
	});
}

/* -- EOF -- */
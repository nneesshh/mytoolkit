//------------------------------------------------------------------------------
//  StdChromiumLog.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "StdChromiumLog.h"

#include <time.h>
#include <stdio.h>
#include <assert.h>

#include "../base/logging.h"
#include "../base/path_service.h"
#include "../base/files/file_path.h"
#include "../core/snprintf/mysnprintf.h"

static bool
my_log_message_handler(int severity, const char* file, int line, size_t message_start, const std::string& str) {
	printf("my_log_message_handler(%s)\n", str.c_str());
	return true;
}

#ifdef MYTOOLKIT_NAMESPACE
namespace MY_TOOLKIT {
#endif

static int s_init = 0;
static logging::LogMessage *s_log_info = nullptr;
static logging::LogMessage *s_log_warning = nullptr;
static logging::LogMessage *s_log_error = nullptr;
static logging::LogMessage *s_log_fatal = nullptr;

//------------------------------------------------------------------------------
/**

*/
CStdChromiumLog::CStdChromiumLog(int level_filter, const char *file_name, bool b_suffix_with_time)
	: _lvl_filter(level_filter) {

	assert(strlen(file_name) < 128);

	char time_suffix[32] = { 0 };
	char final_file_name[256] = { 0 };

	if (b_suffix_with_time) {
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
	}
	else {
		o_snprintf(time_suffix, sizeof(time_suffix), "");
	}

	o_snprintf(final_file_name, sizeof(final_file_name), "syslog/%s%s.log",
		file_name, time_suffix);

	{
		logging::LoggingSettings settings;

		base::FilePath logfile;
		base::PathService::Get(base::DIR_EXE, &logfile);
		logfile = logfile.AppendASCII(final_file_name).NormalizePathSeparators();

		// 
		settings.logging_dest = logging::LOG_TO_ALL;
		settings.log_file = logfile.value().c_str();
		settings.lock_log = logging::DONT_LOCK_LOG_FILE;
		settings.delete_old = logging::DELETE_OLD_LOG_FILE;

		if (logging::InitLogging(settings)) {
			//
			logging::SetLogItems(true, true, true, false);
			logging::SetShowErrorDialogs(false);
			//logging::SetLogMessageHandler(my_log_message_handler);

			s_init = 1;

			s_log_info = new logging::LogMessage(__FILE__, __LINE__, logging::LOG_INFO);
			s_log_warning = new logging::LogMessage(__FILE__, __LINE__, logging::LOG_INFO);
			s_log_error = new logging::LogMessage(__FILE__, __LINE__, logging::LOG_INFO);
			s_log_fatal = new logging::LogMessage(__FILE__, __LINE__, logging::LOG_INFO);
		}
	}
}

//------------------------------------------------------------------------------
/**

*/
CStdChromiumLog::~CStdChromiumLog() {
	if (s_init) {
		delete s_log_info;
		delete s_log_warning;
		delete s_log_error;
		delete s_log_fatal;
	}
}

//------------------------------------------------------------------------------
/**

*/
void
	CStdChromiumLog::logprint(int lvl, const char *format, ...) {
	if (lvl > _lvl_filter)
		return;

	char chbuf[2048];
	va_list args;
	va_start(args, format);
	vsnprintf(chbuf, sizeof(chbuf), format, args);

	//
	switch (lvl) {
	case LOG_LEVEL_DEBUG:
	case LOG_LEVEL_INFO:
	case LOG_LEVEL_NOTICE:
		s_log_info->stream() << chbuf << std::endl;
		break;

	case LOG_LEVEL_WARNING:
		s_log_warning->stream() << chbuf << std::endl;
		break;

	case LOG_LEVEL_ERROR:
		s_log_error->stream() << chbuf << std::endl;
		break;

	case LOG_LEVEL_FATAL:
	case LOG_LEVEL_ALERT:
	case LOG_LEVEL_EMERGENCY:
		s_log_fatal->stream() << chbuf << std::endl;
		break;

	default:
		break;
	}
	va_end(args);
}

#ifdef MYTOOLKIT_NAMESPACE
}
#endif

/* -- EOF -- */
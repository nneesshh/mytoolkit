//------------------------------------------------------------------------------
//  CStdPanLog.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "StdPanLog.h"

#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define PANTHEIOS_NO_USE_WIDE_STRINGS /* force "Use Multi-Byte Character Set" */
#include <pantheios/pantheios.h> /* main Pantheios C header file */
#include <pantheios/backends/be.N.h>
#include <pantheios/backends/bec.file.h>
#include <pantheios/backends/bec.fprintf.h>
#include <pantheios/frontends/fe.N.h>

#include "../core/snprintf/mysnprintf.h"

#if !defined(PANTHEIOS_NO_NAMESPACE)
using namespace pantheios;
#endif

static int s_panres = 0;

/* /////////////////////////////////////////////////////////////////////////
* Globals
*/
pan_fe_N_t PAN_FE_N_SEVERITY_CEILINGS[] = {
	{ 1,  PANTHEIOS_SEV_INFORMATIONAL }	/* Filters out everything below 'info' */
	,{ 2,  PANTHEIOS_SEV_NOTICE }		/* Sets the ceiling to 'notice' */
	,{ 0,  0 }							/* Terminates the array; allows all severities */
};

pan_be_N_t PAN_BE_N_BACKEND_LIST[] = {
	PANTHEIOS_BE_N_STDFORM_ENTRY(1, pantheios_be_file, 0)
	,	PANTHEIOS_BE_N_STDFORM_ENTRY(2, pantheios_be_fprintf, 0)
	,	PANTHEIOS_BE_N_TERMINATOR_ENTRY
};

#define STDLOG_DEFINE_PROCESS_NAME(name) \
	extern "C" const char PANTHEIOS_FE_PROCESS_IDENTITY[] = name

STDLOG_DEFINE_PROCESS_NAME("pan");

#ifdef MYTOOLKIT_NAMESPACE
namespace MY_TOOLKIT {
#endif

//------------------------------------------------------------------------------
/**

*/
CStdPanLog::CStdPanLog(int level_filter, const char *file_name, bool b_suffix_with_time)
	: _lvl_filter(level_filter) {

	assert(strlen(file_name) < 128);

	s_panres = pantheios_init();
	if (s_panres < 0) {
		pantheios_logprintf(PANTHEIOS_SEV_ERROR, PANTHEIOS_LITERAL_STRING("Failed to initialise the Pantheios libraries: %s\n"),
			pantheios_getInitErrorString(s_panres));
	}
	else {
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

		pantheios_be_file_setFilePath(
			PANTHEIOS_LITERAL_STRING(final_file_name),
			PANTHEIOS_BE_FILE_F_TRUNCATE,
			PANTHEIOS_BE_FILE_F_TRUNCATE,
			1);
	}
}

//------------------------------------------------------------------------------
/**

*/
CStdPanLog::~CStdPanLog() {
	if (s_panres >= 0)
 		pantheios_uninit();
}

//------------------------------------------------------------------------------
/**

*/
void
CStdPanLog::logprint(int lvl, const char *format, ...) {
	if (lvl > _lvl_filter)
		return;

	va_list args;
	int     ret;

	PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(nullptr != format, "format pointer may not be null");
	va_start(args, format);
	ret = pantheios_logvprintf(lvl, format, args);
	va_end(args);
}

#ifdef MYTOOLKIT_NAMESPACE
}
#endif

/* -- EOF -- */
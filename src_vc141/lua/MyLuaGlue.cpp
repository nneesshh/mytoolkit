#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "MyLuaGlue.h"

namespace luacpp {

//------------------------------------------------------------------------------
/**

*/
void
CMyLuaGlue::WriteMyLuaGlueError(const char *content_txt, const char *err_txt) {
	char log_file_name[256];
	char time_suffix[32];

	time_t t = time(nullptr);

#if defined(__CYGWIN__) || defined( _WIN32)
	struct tm tp;
	localtime_s(&tp, &t);
	_snprintf_s(
		time_suffix
		, sizeof(time_suffix)
		, "_%d-%02d-%02d_%02d_%02d_%02d"
		, tp.tm_year + 1900
		, tp.tm_mon + 1
		, tp.tm_mday
		, tp.tm_hour
		, tp.tm_min
		, tp.tm_sec);

	_snprintf_s(log_file_name, sizeof(log_file_name), "syslog/myluaglue_%s.error",
		time_suffix);
#else
	struct tm tp;
	localtime_r(&t, &tp);
	snprintf(
		time_suffix
		, sizeof(time_suffix)
		, "_%d-%02d-%02d_%02d_%02d_%02d"
		, tp.tm_year + 1900
		, tp.tm_mon + 1
		, tp.tm_mday
		, tp.tm_hour
		, tp.tm_min
		, tp.tm_sec);

	snprintf(log_file_name, sizeof(log_file_name), "syslog/myluaglue_%s.error",
		time_suffix);
#endif

	// write to file
	FILE *ferr = fopen(log_file_name, "at+");
	fprintf(ferr, "content:\n%s\n",
		content_txt);
	fprintf(ferr, "error:\n%s\n\n\n",
		err_txt);
	fclose(ferr);
}

}
/* -- EOF -- */
#pragma once

//------------------------------------------------------------------------------
/**
    @class CStdChromiumLog
    
    (C) 2016 n.lee
*/
#include "StdLog.h"

#ifdef MYTOOLKIT_NAMESPACE
namespace MyToolkit {
#endif

//------------------------------------------------------------------------------
/** 
	@brief StdChromiumLog
*/
class CStdChromiumLog : public StdLog {
public:
	CStdChromiumLog(int level_filter = LOG_LEVEL_DEBUG, const char *file_name = "chromium", bool b_suffix_with_time = true);
	virtual ~CStdChromiumLog();

	virtual void logprint(int lvl, const char *format, ...);

private:
	int _lvl_filter;
	int _panres = 0;
};

#ifdef MYTOOLKIT_NAMESPACE
}
#endif

/*EOF*/
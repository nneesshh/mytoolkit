#pragma once

//------------------------------------------------------------------------------
/**
    @class CStdPanLog
    
    (C) 2016 n.lee
*/
#include "StdLog.h"

#ifdef MYTOOLKIT_NAMESPACE
namespace MyToolkit {
#endif

//------------------------------------------------------------------------------
/** 
	@brief CStdPanLog
*/
class CStdPanLog : public StdLog {
public:
	CStdPanLog(int level_filter = LOG_LEVEL_DEBUG, const char *file_name = "pantheios", bool b_suffix_with_time = true);
	virtual ~CStdPanLog();

	virtual void logprint(int lvl, const char *format, ...);

private:
	int _lvl_filter;
};

#ifdef MYTOOLKIT_NAMESPACE
}
#endif

/*EOF*/
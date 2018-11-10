#pragma once
//------------------------------------------------------------------------------
/**
@class CUsingBase

(C) 2016 n.lee
*/
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

#include "base/at_exit.h"
#include "base/clear_at_exit.h"
#include "base/command_line.h"
#include "base/path_service.h"
#include "base/run_loop.h"

#pragma pop_macro("min")
#pragma pop_macro("max")

#ifdef _WIN32
#pragma comment(lib, "WS2_32.Lib")
#pragma comment(lib, "DbgHelp.Lib")
#pragma comment(lib, "WinMM.Lib")
#pragma comment(lib, "Version.Lib")
#pragma comment(lib, "ShLwApi.Lib")
#pragma comment(lib, "UserEnv.Lib")
#pragma comment(lib, "WinInet.Lib")
#endif

/*EOF*/
#pragma once
//------------------------------------------------------------------------------
/**
@class CUsingLua

(C) 2016 n.lee
*/
#include "lua/MyLua.h"
#include "lua/MyLuaTimer.h"
#include "lua/tolua++.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "lua/lfs.h"
#include "lua/lbind.h"
#include "lua/lbind.h"

#include "lua/tolua_fix.h"

#ifdef __cplusplus
}
#endif

#ifdef _WIN32
#if LUA_VERSION_NUM < 502
#pragma comment(lib, "lua51.lib")
#else
#pragma comment(lib, "lua53.lib")
#endif
#endif

/*EOF*/
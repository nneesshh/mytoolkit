/*
** LuaProfiler
** Copyright Kepler Project 2005-2007 (http://www.keplerproject.org/luaprofiler)
** $Id: luaprofiler.h,v 1.4 2007-08-22 19:23:53 carregal Exp $
*/

/*****************************************************************************
luaprofiler.h:
    Must be included by your main module, in order to profile Lua programs
*****************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int luaprofiler_init(lua_State *L);
int luaprofiler_stop(lua_State *L);

#ifndef LUA_EXTERN
# if defined(WIN32) || defined(_WIN32)
/*  Building shared library. */
#   define LUA_EXTERN __declspec(dllexport)
# elif __GNUC__ >= 4
#  define LUA_EXTERN __attribute__((visibility("default")))
# else
#  define LUA_EXTERN /* nothing */
# endif
#endif

LUA_EXTERN int luaopen_luaprofiler(lua_State *L);


#ifdef __cplusplus
}
#endif
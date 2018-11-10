#ifndef __TOLUA_FIX_INT64_H_
#define __TOLUA_FIX_INT64_H_

#include "tolua++.h"

#if LUA_VERSION_NUM < 502

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <stdint.h>
#include <stdbool.h>

	TOLUA_API bool toluafix_is_int64(lua_State* L, int lo, const char* type, int def, tolua_Error* err);
	TOLUA_API int64_t toluafix_to_int64(lua_State *L, int lo, int def);
	TOLUA_API void toluafix_push_int64(lua_State *L, int64_t n, const char* type);

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

	LUA_EXTERN int luaopen_int64(lua_State *L);

#ifdef __cplusplus
}
#endif

#endif // #if LUA_VERSION_NUM < 502

#endif // __TOLUA_FIX_INT64_H_
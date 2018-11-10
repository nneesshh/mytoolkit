//------------------------------------------------------------------------------
//  MyLua.cpp
//  (C) 2016 n.lee
//------------------------------------------------------------------------------
#include "MyLua.h"

#include <assert.h>

#if LUA_VERSION_NUM < 502
#include "lbind_int64.h"
#include "lbind_snapshot.h"
#include "luaprofiler_src/luaprofiler.h"
#endif

namespace luacpp {

//------------------------------------------------------------------------------
/**

*/
CMyLua::CMyLua() : _glue(new CMyLuaGlue) {
	
	lua_State *L = _glue->State();
	int top = lua_gettop(L);

#if LUA_VERSION_NUM < 502
	lua_getfield(L, LUA_GLOBALSINDEX, "C");
	if (lua_isnil(L, -1)) {
#else
	lua_getglobal(L, "C");
	if (lua_type(L, -1) == LUA_TNIL) {
#endif
		lbind_init_once(L);
	}
	lua_pop(L, 1);
	assert(0 == lua_gettop(L) - top);

#if LUA_VERSION_NUM < 502

	luaopen_int64(L);
	lua_pop(L, 1);
	assert(0 == lua_gettop(L) - top);

	luaopen_snapshot(L);
	lua_pop(L, 1);
	assert(0 == lua_gettop(L) - top);

	luaopen_luaprofiler(L);
	lua_pop(L, 1);
	assert(0 == lua_gettop(L) - top);

#endif
}

//------------------------------------------------------------------------------
/**

*/
CMyLua::~CMyLua() {
	delete _glue;
}

}
/* -- EOF -- */
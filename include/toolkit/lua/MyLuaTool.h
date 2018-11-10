#pragma once
#include <stdint.h>

#ifndef  _WIN32
#define SNPRINTF_F snprintf
#else
#define SNPRINTF_F _snprintf_s
#endif

#include <lua.h>
#include <string>

namespace luacpp {

class CMyLuaTool {
public:
	static void DumpStack(lua_State *L) {
		int i;
		int top = lua_gettop(L);

		for (i = 1; i <= top; i++) {
			int t = lua_type(L, i);
			switch (t) {
			case LUA_TSTRING: {
				printf("`%s'", lua_tostring(L, i));
				break;
			}

			case LUA_TBOOLEAN: {
				printf(lua_toboolean(L, i) ? "true" : "false");
				break;
			}

			case LUA_TNUMBER: {
				printf("`%g`", lua_tonumber(L, i));
				break;
			}

			case LUA_TTABLE: {
				printf("table end\n");
				lua_pushnil(L);
				while (lua_next(L, i) != 0) {
					printf("	%s - %s\n",
						lua_typename(L, lua_type(L, -2)),
						lua_typename(L, lua_type(L, -1)));
					lua_pop(L, 1);
				}
				printf("table end");
				break;
			}

			default: {
				printf("`%s`", lua_typename(L, t));
				break;
			}

			}
			printf(" ");
		}
		printf("\n");
	}

	static std::string DumpError(lua_State *L, const char *fmt, ...) {
		std::string ret;
		char buff[1024];

		va_list argp;
		va_start(argp, fmt);
#ifndef _WIN32
		vsnprintf(buff, sizeof(buff), fmt, argp);
#else
		vsnprintf_s(buff, sizeof(buff), sizeof(buff), fmt, argp);
#endif
		va_end(argp);

		ret = buff;
		SNPRINTF_F(buff, sizeof(buff), " tracback:%s", lua_tostring(L, -1));
		ret += buff;
		return ret;
	}
};

}
/*EOF*/
#include "tolua_fix_int64.h"

#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#if LUA_VERSION_NUM < 502

static int s_toluafix_int64_mt = LUA_NOREF;

bool
toluafix_is_int64(lua_State* L, int lo, const char* type, int def, tolua_Error* err) {
	if (lua_getmetatable(L, lo)) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, s_toluafix_int64_mt);
		int equal = lua_rawequal(L, -1, -2);
		lua_pop(L, 2);
		return equal;
	}

	return false;
}

int64_t
toluafix_to_int64(lua_State *L, int lo, int def) {
	int64_t n = (int64_t)0;
	int type = lua_type(L, lo);
	switch (type) {
	case LUA_TNUMBER: {
		lua_Number d = luaL_checknumber(L, lo);
		n = (int64_t)d;
		break;
	}

	case LUA_TUSERDATA: {
		if (toluafix_is_int64(L, lo, "int64", 0, 0)) {
			n = *(int64_t *)lua_touserdata(L, lo);
		}
		else {
			return luaL_error(L, "The userdata is not an int64 userdata");
		}
		break;
	}

	default:
		return luaL_error(L, "argument %d error type %s", lo, lua_typename(L, type));
	}
	return n;
}

void
toluafix_push_int64(lua_State *L, int64_t n, const char* type) {
	int64_t *p = (int64_t*)lua_newuserdata(L, sizeof(int64_t));
	(void *)type;

	*p = n;
	lua_rawgeti(L, LUA_REGISTRYINDEX, s_toluafix_int64_mt);
	lua_setmetatable(L, -2);
}

static int
__int64_add(lua_State *L) {
	int64_t a = toluafix_to_int64(L, 1, 0);
	int64_t b = toluafix_to_int64(L, 2, 0);
	toluafix_push_int64(L, a + b, "int64");
	return 1;
}

static int
__int64_sub(lua_State *L) {
	int64_t a = toluafix_to_int64(L, 1, 0);
	int64_t b = toluafix_to_int64(L, 2, 0);
	toluafix_push_int64(L, a - b, "int64");
	return 1;
}

static int
__int64_mul(lua_State *L) {
	int64_t a = toluafix_to_int64(L, 1, 0);
	int64_t b = toluafix_to_int64(L, 2, 0);
	toluafix_push_int64(L, a * b, "int64");
	return 1;
}

static int
__int64_div(lua_State *L) {
	int64_t a = toluafix_to_int64(L, 1, 0);
	int64_t b = toluafix_to_int64(L, 2, 0);
	if (b == 0) {
		return luaL_error(L, "div by zero");
	}
	toluafix_push_int64(L, a / b, "int64");
	return 1;
}

static int
__int64_mod(lua_State *L) {
	int64_t a = toluafix_to_int64(L, 1, 0);
	int64_t b = toluafix_to_int64(L, 2, 0);
	if (b == 0) {
		return luaL_error(L, "mod by zero");
	}
	toluafix_push_int64(L, a % b, "int64");
	return 1;
}

static int64_t
__pow64(int64_t a, int64_t b) {
	if (b == 1) {
		return a;
	}
	int64_t a2 = a * a;
	if (b % 2 == 1) {
		return __pow64(a2, b / 2) * a;
	}
	else {
		return __pow64(a2, b / 2);
	}
}

static int
__int64_pow(lua_State *L) {
	int64_t a = toluafix_to_int64(L, 1, 0);
	int64_t b = toluafix_to_int64(L, 2, 0);
	int64_t p;
	if (b > 0) {
		p = __pow64(a, b);
	}
	else if (b == 0) {
		p = 1;
	}
	else {
		return luaL_error(L, "pow by negative number %d", (int)b);
	}
	toluafix_push_int64(L, p, "int64");
	return 1;
}

static int
__int64_unm(lua_State *L) {
	int64_t a = toluafix_to_int64(L, 1, 0);
	toluafix_push_int64(L, -a, "int64");
	return 1;
}

static bool
__str2uint64(const char *s, int base, uint64_t *result) {
	char *endptr;
	*result = strtoull(s, &endptr, base);
	if (endptr == s) {
		return false;
	}

	if (*endptr == '\0') {
		return true;
	}

	while (isspace((unsigned char)*endptr)) endptr++;
	return *endptr == '\0';
}

static int
__int64_eq(lua_State *L) {
	int64_t a = toluafix_to_int64(L, 1, 0);
	int64_t b = toluafix_to_int64(L, 2, 0);
	// 	printf("%s %s\n", lua_typename(L, 1), lua_typename(L, 2));
	// 	printf("%lld %lld\n", a, b);
	lua_pushboolean(L, a == b);
	return 1;
}

static int
__int64_lt(lua_State *L) {
	int64_t a = toluafix_to_int64(L, 1, 0);
	int64_t b = toluafix_to_int64(L, 2, 0);
	lua_pushboolean(L, a < b);
	return 1;
}

static int
__int64_le(lua_State *L) {
	int64_t a = toluafix_to_int64(L, 1, 0);
	int64_t b = toluafix_to_int64(L, 2, 0);
	lua_pushboolean(L, a <= b);
	return 1;
}

static int
__int64_len(lua_State *L) {
	int64_t a = toluafix_to_int64(L, 1, 0);
	lua_pushnumber(L, (lua_Number)a);
	return 1;
}

static int
__int64_new(lua_State *L) {
	int64_t n = (int64_t)0;

	int top = lua_gettop(L);
	switch (top) {
	case 0: {
		toluafix_push_int64(L, (int64_t)0, "int64");
		break;
	}

	case 1: {
		int type = lua_type(L, 1);
		switch (type) {
		case LUA_TNUMBER: {
			lua_Number d = luaL_checknumber(L, 1);
			n = (int64_t)d;
			toluafix_push_int64(L, n, "int64");
			break;
		}

		case LUA_TSTRING: {
			uint64_t u64 = (uint64_t)0;
			size_t len = 0;
			const uint8_t *str = (const uint8_t *)lua_tolstring(L, 1, &len);
			if (!__str2uint64(str, 10, &u64)) {
				return luaL_error(L, "The string(%s) is not a valid number string", str);
			}
			n = (int64_t)u64;
			toluafix_push_int64(L, n, "int64");
			break;
		}

		default:
			return luaL_error(L, "Type(%d) invalid", type);
		}
		break;
	}

	default:
		uint64_t u64 = (uint64_t)0;
		size_t len = 0;
		const uint8_t *str = (const uint8_t *)lua_tolstring(L, 1, &len);
		int base = (int)luaL_checkinteger(L, 2);
		if (base < 2) {
			luaL_error(L, "base must be >= 2");
			base = 10;
		}

		if (!__str2uint64(str, base, &u64)) {
			return luaL_error(L, "The string(%s) is not a valid number string", str);
		}
		n = (int64_t)u64;
		toluafix_push_int64(L, n, "int64");
		break;
	}
	return 1;
}

static int
__tostring(lua_State *L) {
	static char hex[16] = "0123456789abcdef";

	int top = lua_gettop(L);
	switch (top) {
	case 1: {
		int buffer[32];
		int i;

		// decimal, 10
		int64_t dec = toluafix_to_int64(L, 1, 0);

		luaL_Buffer b;
		luaL_buffinit(L, &b);
		if (dec < 0) {
			luaL_addchar(&b, '-');
			dec = -dec;
		}

		for (i = 0; i < 32; ++i) {
			buffer[i] = dec % 10;
			dec /= 10;
			if (dec == 0)
				break;
		}

		while (i >= 0) {
			luaL_addchar(&b, hex[buffer[i]]);
			--i;
		}

		luaL_pushresult(&b);
		return 1;
	}

	case 2: {
		int64_t n = toluafix_to_int64(L, 1, 0);
		int base = (int)luaL_checkinteger(L, 2);

		char buffer[64];
		int shift, mask;
		int i;

		switch (base) {
		case 2: {
			shift = 1;
			mask = 0x1;
			break;
		}

		case 8: {
			shift = 3;
			mask = 0x7;
			break;
		}

		case 16: {
			shift = 4;
			mask = 0xf;
			break;
		}

		default: {
			// hex, strip leading zero
			bool strip = true;

			luaL_Buffer b;
			luaL_buffinit(L, &b);
			luaL_addstring(&b, "0x");

			for (i = 15; i >= 0; --i) {
				int c = (n >> (i * 4)) & 0xf;
				if (strip && c == 0) {
					continue;
				}
				strip = false;
				luaL_addchar(&b, hex[c]);
			}

			if (strip) {
				// it is 0
				luaL_addchar(&b, '0');
			}

			luaL_pushresult(&b);
			return 1;
		}
		}

		for (i = 0; i < 64; i += shift) {
			buffer[i / shift] = hex[(n >> (64 - shift - i)) & mask];
		}

		lua_pushlstring(L, buffer, 64 / shift);
		return 1;
	}

	default:
		lua_pushstring(L, "(none)");
	}
	return 1;
}

static void
__make_mt(lua_State *L) {
	luaL_Reg mt_[] = {
		{ "__add", __int64_add },
		{ "__sub", __int64_sub },
		{ "__mul", __int64_mul },
		{ "__div", __int64_div },
		{ "__mod", __int64_mod },
		{ "__unm", __int64_unm },
		{ "__pow", __int64_pow },
		{ "__eq", __int64_eq },
		{ "__lt", __int64_lt },
		{ "__le", __int64_le },
		{ "__len", __int64_len },
		{ "__tostring", __tostring },
		{ NULL, NULL },
	};
	luaL_register(L, "tolua_int64_mt", mt_);
}

static luaL_Reg lib_[] = {
	{ "new", __int64_new },
	{ "tostring", __tostring },
	{ NULL, NULL },
};

int
luaopen_int64(lua_State *L) {

	int top = lua_gettop(L);

	__make_mt(L);
	s_toluafix_int64_mt = luaL_ref(L, LUA_REGISTRYINDEX);

	luaL_register(L, "int64", lib_);

	assert(1 == lua_gettop(L) - top);
	return 1;
}

#endif // #if LUA_VERSION_NUM < 502
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <lua.h>
#include <string>
#include <sstream>

namespace luacpp {

class mylua_nil_t {};

//!#########################################################################################################################
template<typename ARG_TYPE>
struct mylua_arg_t;

template<>
struct mylua_arg_t<const char*> {
	static void add(struct lvar_array_t *w, const char *arg) {
		lbind_add_string(w, arg);
	}

	static const char * get(struct lvar_array_t *w, int pos) {
		return lbind_as_string(w, pos);
	}
};

template<>
struct mylua_arg_t<char*> {
	static void add(struct lvar_array_t *w, char *arg) {
		lbind_add_string(w, arg);
	}

	static const char * get(struct lvar_array_t *w, int pos) {
		return lbind_as_string(w, pos);
	}
};

template<>
struct mylua_arg_t<mylua_nil_t> {
	static void add(struct lvar_array_t *w) {
		lbind_add_nil(w);
	}
};

template<>
struct mylua_arg_t<bool> {
	static void add(struct lvar_array_t *w, bool arg) {
		lbind_add_boolean(w, arg);
	}

	static int get(struct lvar_array_t *w, int pos) {
		return lbind_as_boolean(w, pos);
	}
};

template<>
struct mylua_arg_t<int8_t> {
	static void add(struct lvar_array_t *w, int8_t arg) {
		lbind_add_integer(w, (lua_Integer)arg);
	}

	static int8_t get(struct lvar_array_t *w, int pos) {
		return (int8_t)lbind_as_integer(w, pos);
	}
};

template<>
struct mylua_arg_t<uint8_t> {
	static void add(struct lvar_array_t *w, uint8_t arg) {
		lbind_add_integer(w, (lua_Integer)arg);
	}

	static uint8_t get(struct lvar_array_t *w, int pos) {
		return (uint8_t)lbind_as_integer(w, pos);
	}
};

template<>
struct mylua_arg_t<int16_t> {
	static void add(struct lvar_array_t *w, int16_t arg) {
		lbind_add_integer(w, (lua_Integer)arg);
	}

	static int16_t get(struct lvar_array_t *w, int pos) {
		return (int16_t)lbind_as_integer(w, pos);
	}
};

template<>
struct mylua_arg_t<uint16_t> {
	static void add(struct lvar_array_t *w, uint16_t arg) {
		lbind_add_integer(w, (lua_Integer)arg);
	}

	static uint16_t get(struct lvar_array_t *w, int pos) {
		return (uint16_t)lbind_as_integer(w, pos);
	}
};

template<>
struct mylua_arg_t<int32_t> {
	static void add(struct lvar_array_t *w, int32_t arg) {
		lbind_add_integer(w, (lua_Integer)arg);
	}

	static int32_t get(struct lvar_array_t *w, int pos) {
		return (int32_t)lbind_as_integer(w, pos);
	}
};

template<>
struct mylua_arg_t<uint32_t> {
	static void add(struct lvar_array_t *w, uint32_t arg) {
		lbind_add_integer(w, (lua_Integer)arg);
	}

	static uint32_t get(struct lvar_array_t *w, int pos) {
		return (uint32_t)lbind_as_integer(w, pos);
	}
};

template<>
struct mylua_arg_t<int64_t> {
	static void add(struct lvar_array_t *w, int64_t arg) {
#if LUA_VERSION_NUM < 502
		lbind_add_uint64(w, arg);
#else
		lbind_add_integer(w, (lua_Integer)arg);
#endif
	}

	static int64_t get(struct lvar_array_t *w, int pos) {
#if LUA_VERSION_NUM < 502
		return (int64_t)lbind_as_uint64(w, pos);
#else
		return (int64_t)lbind_as_integer(w, pos);
#endif
	}
};

template<>
struct mylua_arg_t<uint64_t> {
	static void add(struct lvar_array_t *w, uint64_t arg) {
#if LUA_VERSION_NUM < 502
		lbind_add_uint64(w, arg);
#else
		lbind_add_integer(w, (lua_Integer)arg);
#endif
	}

	static uint64_t get(struct lvar_array_t *w, int pos) {
#if LUA_VERSION_NUM < 502
		return lbind_as_uint64(w, pos);
#else
		return (uint64_t)lbind_as_integer(w, pos);
#endif
	}
};

template<>
struct mylua_arg_t<std::string> {
	static void add(struct lvar_array_t *w, const std::string& arg) {
		lbind_add_lstring(w, arg.c_str(), arg.length());
	}

	static std::string get(struct lvar_array_t *w, int pos) {
		size_t len;
		const char *ret = lbind_as_lstring(w, pos, &len);
		return std::string(ret, len);
	}
};

template<>
struct mylua_arg_t<float> {
	static void add(struct lvar_array_t *w, float arg) {
		lbind_add_real(w, (lua_Number)arg);
	}

	static float get(struct lvar_array_t *w, int pos) {
		return (float)lbind_as_real(w, pos);
	}
};

template<>
struct mylua_arg_t<double> {
	static void add(struct lvar_array_t *w, double arg) {
		lbind_add_real(w, (lua_Number)arg);
	}

	static double get(struct lvar_array_t *w, int pos) {
		return (double)lbind_as_real(w, pos);
	}
};

template<>
struct mylua_arg_t<void*> {
	static void add(struct lvar_array_t *w, void *arg) {
		lbind_add_pointer(w, arg);
	}

	static void * get(struct lvar_array_t *w, int pos) {
		return lbind_as_pointer(w, pos);
	}
};

}
/*EOF*/
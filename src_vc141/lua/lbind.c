#include "lbind.h"

#include <lualib.h>   
#include <lauxlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#include "ringbuffer.h"

#if LUA_VERSION_NUM < 502
#	include "tolua_fix_int64.h"
#endif

#define LUA_OK	0

static struct lvar_array_t *s_lbind_args = NULL;
static int s_lbind_ref_args = LUA_NOREF;

static struct lvar_array_t *s_lbind_result = NULL;
static int s_lbind_ref_result = LUA_NOREF;

static uint32_t s_lbind_func_refid = 0;
static int s_lbind_ref_func_refid_table = LUA_NOREF;
static struct ringbuffer_t *s_free_refid_pool = NULL;

//////////////////////////////////////////////////////////////////////////
// internal declaration
//////////////////////////////////////////////////////////////////////////

static void __lerrorlog(lua_State *L);
static int __lprecall(lua_State *L);
static int __lprecall_chunk(lua_State *L);
static int __lregunpack(lua_State *L);
static int __lregister(lua_State *L);
static int __ldofile(lua_State *L);
static int __ldostring(lua_State *L);

static struct lvar_t * __lalloc_var(struct lvar_array_t *w);
static struct lvar_array_t * __lalloc_var_array(lua_State *L);
static int __lrelease_var_array(lua_State *L);
static int __lpush_to_stack(lua_State *L, struct lvar_array_t *w);
static void __lpop_from_stack(lua_State *L, struct lvar_array_t *w);
static void __lreset_var_array(lua_State *L, struct lvar_array_t *w);
static struct lvar_array_t * __lget_var_array(lua_State *L, const int ref);

//////////////////////////////////////////////////////////////////////////
int
lbind_ref_function(lua_State* L, int lo)
{
	uint32_t refid;
	bool b;

	// function at lo
	if (!lua_isfunction(L, lo)) return 0;

	if (RINGBUFFER_EMPTY(s_free_refid_pool) || s_lbind_func_refid < RINGBUFFER_CAPACITY(s_free_refid_pool)) {
		refid = ++s_lbind_func_refid;
	}
	else {
		refid = ringbuffer_pop(s_free_refid_pool, &b);
	}

	lua_rawgeti(L, LUA_REGISTRYINDEX, s_lbind_ref_func_refid_table); /* stack: fun ... refid_fun */
	lua_pushinteger(L, refid);                                       /* stack: fun ... refid_fun refid */
	lua_pushvalue(L, lo);                                            /* stack: fun ... refid_fun refid fun */

	lua_rawset(L, -3);                                               /* refid_fun[refid] = fun, stack: fun ... refid_ptr */
	lua_pop(L, 1);                                                   /* stack: fun ... */

	return refid;
}

inline void
libind_get_function_by_refid(lua_State* L, int refid)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, s_lbind_ref_func_refid_table); /* stack: fun ... refid_fun */
	lua_pushinteger(L, refid);                                       /* stack: ... refid_fun refid */
	lua_rawget(L, -2);                                               /* stack: ... refid_fun fun */
	lua_remove(L, -2);                                               /* stack: ... fun */
}

void
lbind_remove_function_by_refid(lua_State* L, int refid)
{
	if (!RINGBUFFER_FULL(s_free_refid_pool)) {
		ringbuffer_push(s_free_refid_pool, refid);
	}

	lua_rawgeti(L, LUA_REGISTRYINDEX, s_lbind_ref_func_refid_table); /* stack: fun ... refid_fun */
	lua_pushinteger(L, refid);                                       /* stack: ... refid_fun refid */
	lua_pushnil(L);                                                  /* stack: ... refid_fun refid nil */
	lua_rawset(L, -3);                                               /* refid_fun[refid] = fun, stack: ... refid_ptr */
	lua_pop(L, 1);                                                   /* stack: ... */
}

inline void
lbind_call_function_by_refid_with_no_arg_return_void(lua_State* L, int refid)
{
	int ret;

	libind_get_function_by_refid(L, refid);
	ret = lua_pcall(L, 0, 0, 0);
	if (ret != LUA_OK) {
		__lerrorlog(L);
	}
}

inline void
lbind_call_function_by_refid_with_arg_int_return_void(lua_State* L, int refid, int n)
{
	int ret;

	libind_get_function_by_refid(L, refid);
	lua_pushinteger(L, n);
	ret = lua_pcall(L, 1, 0, 0);
	if (ret != LUA_OK) {
		__lerrorlog(L);
	}
}

//////////////////////////////////////////////////////////////////////////
inline int
lbind_type(struct lvar_array_t *w, int index) {
	if (index < 0 || index >= w->t) {
		return LT_NONE;
	}
	return w->v[index].type;
}

inline lua_Integer
lbind_as_integer(struct lvar_array_t *w, int index) {
	assert(index >= 0 && index < w->t);
	return w->v[index].val.i;
}

inline lua_Number
lbind_as_real(struct lvar_array_t *w, int index) {
	assert(index >= 0 && index < w->t);
	return w->v[index].val.f;
}

inline uint64_t
lbind_as_uint64(struct lvar_array_t *w, int index) {
	assert(index >= 0 && index < w->t);
	return w->v[index].val.u;
}

inline const char *
lbind_as_string(struct lvar_array_t *w, int index) {
	assert(index >= 0 && index < w->t);
	return w->v[index].val.s;
}

inline const char *
lbind_as_lstring(struct lvar_array_t *w, int index, size_t *len) {
	assert(index >= 0 && index < w->t);
	(*len) = w->v[index].len;
	return w->v[index].val.s;
}

inline int
lbind_as_boolean(struct lvar_array_t *w, int index) {
	assert(index >= 0 && index < w->t);
	return w->v[index].val.b;
}

inline void *
lbind_as_pointer(struct lvar_array_t *w, int index) {
	assert(index >= 0 && index < w->t);
	return w->v[index].val.p;
}

inline int
lbind_as_func_refid(struct lvar_array_t *w, int index) {
	assert(index >= 0 && index < w->t);
	return w->v[index].val.func_refid;
}

inline void
lbind_add_integer(struct lvar_array_t *w, lua_Integer i) {
	struct lvar_t *e = __lalloc_var(w);
	e->type = LT_INTEGER;
	e->val.i = i;
	e->len = 0;
}

inline void
lbind_add_real(struct lvar_array_t *w, lua_Number f) {
	struct lvar_t *e = __lalloc_var(w);
	e->type = LT_REAL;
	e->val.f = f;
	e->len = 0;
}

inline void
lbind_add_uint64(struct lvar_array_t *w, uint64_t u) {
	struct lvar_t *e = __lalloc_var(w);
	e->type = LT_UINT64;
	e->val.u = u;
	e->len = 0;
}

inline void
lbind_add_string(struct lvar_array_t *w, const char *str) {
	struct lvar_t *e = __lalloc_var(w);
	e->type = LT_STRING;
	e->val.s = str;
	e->len = 0;
}

inline void
lbind_add_lstring(struct lvar_array_t *w, const char *str, size_t len) {
	struct lvar_t *e = __lalloc_var(w);
	e->type = LT_LSTRING;
	e->val.s = str;
	e->len = len;
}

inline void
lbind_add_boolean(struct lvar_array_t *w, int b) {
	struct lvar_t *e = __lalloc_var(w);
	e->type = LT_BOOLEAN;
	e->val.b = b;
	e->len = 0;
}

inline void
lbind_add_nil(struct lvar_array_t *w) {
	struct lvar_t *e = __lalloc_var(w);
	e->type = LT_NIL;
	e->len = 0;
}

inline void
lbind_add_pointer(struct lvar_array_t *w, void *p) {
	struct lvar_t *e = __lalloc_var(w);
	e->type = LT_POINTER;
	e->val.p = p;
	e->len = 0;
}

inline void
lbind_add_func_refid(struct lvar_array_t *w, int func_refid) {
	struct lvar_t *e = __lalloc_var(w);
	e->type = LT_FUNCREF;
	e->val.func_refid = func_refid;
	e->len = 0;
}

inline struct lvar_array_t *
lbind_init_args(lua_State *L) {
	struct lvar_array_t *args = __lget_var_array(L, s_lbind_ref_args);
	__lreset_var_array(L, args);
	return args;
}

struct lvar_array_t *
lbind_call(lua_State *L, const char *funcname, struct lvar_array_t *args) {
	int ret;

	lua_pushcfunction(L, __lprecall);
	lua_pushlightuserdata(L, (void *)funcname);
	lua_pushlightuserdata(L, args);
	ret = lua_pcall(L, 2, 1, 0);
	if (LUA_OK == ret) {
		struct lvar_array_t *result = lua_touserdata(L, -1);
		lua_pop(L, 1);
		return result;
	}
	else {
		__lerrorlog(L);
		return NULL;
	}
}

struct lvar_array_t *
lbind_call_chunk(lua_State *L, const char *chunk, struct lvar_array_t *args) {
	int ret;

	lua_pushcfunction(L, __lprecall_chunk);
	lua_pushlightuserdata(L, (void *)chunk);
	lua_pushlightuserdata(L, args);
	ret = lua_pcall(L, 2, 1, 0);
	if (LUA_OK == ret) {
		struct lvar_array_t *result = lua_touserdata(L, -1);
		lua_pop(L, 1);
		return result;
	}
	else {
		__lerrorlog(L);
		return NULL;
	}
}

void
lbind_register(lua_State *L, const char *funcname, lbind_function f) {
	int ret;

	lua_pushcfunction(L, __lregister);
	lua_pushlightuserdata(L, (void *)funcname);
	lua_pushlightuserdata(L, f);
	ret = lua_pcall(L, 2, 0, 0);
	if (ret != LUA_OK) {
		__lerrorlog(L);
	}
}

int
lbind_dofile(lua_State *L, const char *filename) {
	int ret;

	lua_pushcfunction(L, __ldofile);
	lua_pushlightuserdata(L, (void *)filename);
	ret = lua_pcall(L, 1, 0, 0);  /* force RET==0 */
	if (LUA_OK == ret) {
		return 0;
	}
	else {
		__lerrorlog(L);
		return -1;
	}
}

int
lbind_dostring(lua_State *L, const char *str) {
	int ret;

	lua_pushcfunction(L, __ldostring);
	lua_pushlightuserdata(L, (void *)str);
	ret = lua_pcall(L, 1, 0, 0);  /* force RET==0 */
	if (LUA_OK == ret) {
		return 0;
	}
	else {
		__lerrorlog(L);
		return -1;
	}
}

//////////////////////////////////////////////////////////////////////////
// internal implementation
//////////////////////////////////////////////////////////////////////////

static void __lbind_write_error_log(const char *err_txt) {
	char log_file_name[256];
	char time_suffix[32];

	time_t t = time(NULL);

#if defined(__CYGWIN__) || defined( _WIN32)
	struct tm tp;
	localtime_s(&tp, &t);
	_snprintf_s(
		time_suffix
		, sizeof(time_suffix)
		, sizeof(time_suffix) - 1
		, "_%d-%02d-%02d_%02d_%02d_%02d"
		, tp.tm_year + 1900
		, tp.tm_mon + 1
		, tp.tm_mday
		, tp.tm_hour
		, tp.tm_min
		, tp.tm_sec);

	_snprintf_s(log_file_name, sizeof(log_file_name), sizeof(log_file_name) - 1, "syslog/lbind_%s.error",
		time_suffix);
#else
	struct tm tp;
	localtime_r(&t, &tp);
	snprintf(
		time_suffix
		, sizeof(time_suffix)
		, "_%d-%02d-%02d_%02d_%02d_%02d"
		, tp.tm_year + 1900
		, tp.tm_mon + 1
		, tp.tm_mday
		, tp.tm_hour
		, tp.tm_min
		, tp.tm_sec);

	snprintf(log_file_name, sizeof(log_file_name), "syslog/lbind_%s.error",
		time_suffix);
#endif

	// write to file
	FILE *ferr = fopen(log_file_name, "at+");
	fprintf(ferr, "%s\n\n\n",
		err_txt);
	fclose(ferr);
}

static inline void
__lerrorlog(lua_State *L) {
	const char *errtxt = lua_tostring(L, -1);
	fprintf(stderr, "Error: %s", errtxt);
	lua_pop(L, 1);

	__lbind_write_error_log(errtxt);
	system("pause");
}

static int
__lprecall(lua_State *L) {
	const char *funcname = lua_touserdata(L, 1);
	struct lvar_array_t *args = lua_touserdata(L, 2);
	struct lvar_array_t *result;

#if LUA_VERSION_NUM < 502
	lua_getfield(L, LUA_GLOBALSINDEX, funcname);
	if (!lua_isfunction(L, -1)) {
#else
	lua_getglobal(L, funcname);
	if (lua_type(L, -1) != LUA_TFUNCTION) {
#endif
		return luaL_error(L, "%s is not a function", funcname);
	}

	lua_call(L, __lpush_to_stack(L, args), LUA_MULTRET);
	luaL_checkstack(L, LUA_MINSTACK, NULL);
	result = __lget_var_array(L, s_lbind_ref_result);
	__lreset_var_array(L, result);
	__lpop_from_stack(L, result);
	lua_pushlightuserdata(L, result);
	return 1;
}

static int
__lprecall_chunk(lua_State *L) {
	const char *chunk = lua_touserdata(L, 1);
	struct lvar_array_t *args = lua_touserdata(L, 2);
	struct lvar_array_t *result;
	int ret;

	lua_pushcfunction(L, __ldostring);
	lua_pushlightuserdata(L, (void *)chunk);
	ret = lua_pcall(L, 1, 1, 0); /* force RET==1 */
	if (ret != LUA_OK) {
		__lerrorlog(L);
		lua_pushinteger(L, LF_ERRPARSE);
		return 1;
	}

	if (lua_type(L, -1) == LUA_TNIL) {
		return 0;
	}
	else if (lua_type(L, -1) != LUA_TFUNCTION) {
		return luaL_error(L, "chunk(%s) must return a function", chunk);
	}
	
	lua_call(L, __lpush_to_stack(L, args), LUA_MULTRET);
	luaL_checkstack(L, LUA_MINSTACK, NULL);
	result = __lget_var_array(L, s_lbind_ref_result);
	__lreset_var_array(L, result);
	__lpop_from_stack(L, result);
	lua_pushlightuserdata(L, result);
	return 1;
}

static int
__lregunpack(lua_State *L) {

	lbind_function f = (lbind_function)lua_touserdata(L, lua_upvalueindex(1));
	struct lvar_array_t *args = __lget_var_array(L, s_lbind_ref_args);
	struct lvar_array_t *result = __lget_var_array(L, s_lbind_ref_result);

	__lreset_var_array(L, args);
	__lreset_var_array(L, result);

	__lpop_from_stack(L, args);
	f(args, result);
	return __lpush_to_stack(L, result);
}

static int
__lregister(lua_State *L) {
	const char *funcname = lua_touserdata(L, 1);
	luaL_checktype(L, 2, LUA_TLIGHTUSERDATA);

#if LUA_VERSION_NUM < 502
	lua_getfield(L, LUA_GLOBALSINDEX, "C");
	if (!lua_istable(L, -1)) {
#else
	lua_getglobal(L, "C");
	if (lua_type(L, -1) != LUA_TTABLE) {
#endif
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "C");
	}
	lua_pushvalue(L, 2);
	lua_pushcclosure(L, __lregunpack, 1);
	lua_setfield(L, -2, funcname);
	return 0;
	}

static int
__ldofile(lua_State *L) {
	int ret, top;
	const char *filename;

	filename = (const char *)lua_touserdata(L, 1);
	ret = luaL_loadfile(L, filename);
	if (ret != LUA_OK) {
		__lerrorlog(L);
		if (ret == LUA_ERRFILE) {
			lua_pushinteger(L, LF_NOTFOUND);
		}
		else {
			lua_pushinteger(L, LF_ERRPARSE);
		}
		return 1;
	}

	top = lua_gettop(L);
	ret = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (ret != LUA_OK) {
		__lerrorlog(L);
		lua_pushinteger(L, LF_ERRRUN);
		return 1;
	}
	return lua_gettop(L) - top + 1;
}

static int
__ldostring(lua_State *L) {
	int ret, top;
	const char *str;

	str = (const char *)lua_touserdata(L, 1);
	ret = luaL_loadstring(L, str);
	if (ret != LUA_OK) {
		__lerrorlog(L);
		lua_pushinteger(L, LF_ERRPARSE);
		return 1;
	}

	top = lua_gettop(L);
	ret = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (ret != LUA_OK) {
		__lerrorlog(L);
		lua_pushinteger(L, LF_ERRRUN);
		return 1;
	}
	return lua_gettop(L) - top + 1;
}

static struct lvar_t *
__lalloc_var(struct lvar_array_t *w) {
	if (w->t >= w->sz) {
		int cap = w->sz * 2;
		if (cap == 0) {
			cap = 16;
		}
		struct lvar_t *nv = malloc(cap * sizeof(struct lvar_t));
		memcpy(nv, w->v, w->t * sizeof(*nv));
		free(w->v);
		w->v = nv;
		w->sz = cap;
	}
	struct lvar_t *ret = &w->v[w->t];
	++w->t;
	return ret;
}

static struct lvar_array_t *
__lalloc_var_array(lua_State *L) {

	struct lvar_array_t **box = (struct lvar_array_t **)lua_newuserdata(L, sizeof(struct lvar_array_t **));
	struct lvar_array_t *w = NULL;

	if (luaL_newmetatable(L, "lbind_var_array_mt")) {
		lua_pushcfunction(L, __lrelease_var_array);
		lua_setfield(L, -2, "__gc");
	}
	lua_setmetatable(L, -2);

	w = malloc(sizeof(struct lvar_array_t));
	w->t = 0;
	w->sz = 0;
	w->v = NULL;

	*box = w;
	return w;
}

static int
__lrelease_var_array(lua_State *L) {
	struct lvar_array_t **box = (struct lvar_array_t **)lua_touserdata(L, 1);
	if (*box) {
		struct lvar_array_t *w = (*box);
		if (w->v) free(w->v);
		free(w);

		*box = NULL;
	}
	return 0;
}

// v -> lua stack
static int
__lpush_to_stack(lua_State *L, struct lvar_array_t *w) {
	int n = w->t;
	int i;

	luaL_checkstack(L, n, NULL);

	for (i = 0; i < n; ++i) {
		struct lvar_t *e = &w->v[i];
		switch (e->type) {
		case LT_NIL:
			lua_pushnil(L);
			break;
		case LT_INTEGER:
			lua_pushinteger(L, e->val.i);
			break;
		case LT_REAL:
			lua_pushnumber(L, e->val.f);
			break;
		case LT_UINT64:
#if LUA_VERSION_NUM < 502
			lbind_int64_pushint64(L, e->val.u);
#else
			lua_pushinteger(L, e->val.u);
#endif
			break;
		case LT_STRING:
			lua_pushstring(L, e->val.s);
			break;
		case LT_LSTRING:
			lua_pushlstring(L, e->val.s, e->len);
			break;
		case LT_BOOLEAN:
			lua_pushboolean(L, e->val.b);
			break;
		case LT_POINTER:
			lua_pushlightuserdata(L, e->val.p);
			break;
		case LT_FUNCREF:
			libind_get_function_by_refid(L, e->val.func_refid);
			break;
		case LT_ARRAY: //todo
		case LT_MAP: //todo
		default:
			luaL_error(L, "unsupported type %d", e->type);
		}
	}
	return n;
}

// lua stack -> v
static void
__lpop_from_stack(lua_State *L, struct lvar_array_t *w) {
	int n = lua_gettop(L);
	int i;

	for (i = 1; i <= n; ++i) {

		int t = lua_type(L, i);
		switch (t) {
		case LUA_TNIL: {
			lbind_add_nil(w);
			break;
		}

		case LUA_TBOOLEAN: {
			lbind_add_boolean(w, lua_toboolean(L, i));
			break;
		}

		case LUA_TLIGHTUSERDATA: {
			lbind_add_pointer(w, lua_touserdata(L, i));
			break;
		}

		case LUA_TNUMBER: {
#if LUA_VERSION_NUM < 502
			if (lua_isnumber(L, i)) {
				lbind_add_real(w, lua_tonumber(L, i));
			}
#else
			if (lua_isinteger(L, i)) {
				lbind_add_integer(w, lua_tointeger(L, i));
			}
			else {
				lbind_add_real(w, lua_tonumber(L, i));
			}
#endif
			break;
		}

		case LUA_TSTRING: {
			lbind_add_string(w, lua_tostring(L, i));
			break;
		}

		case LUA_TFUNCTION: {
			/* Note: refid is never released by lbind for shorten code, user should do it by "lbind_remove_function_by_refid()". */
			lbind_add_func_refid(w, lbind_ref_function(L, i));
			break;
		}			  

		case LUA_TUSERDATA: {
#if LUA_VERSION_NUM < 502
			if (lbind_int64_isint64(L, i)) {
				int64_t n = *(int64_t *)lua_touserdata(L, i);
				lbind_add_uint64(w, n);
			}
			else 
			{
#endif
				luaL_error(L, "unsupported type %s", lua_typename(L, t));

#if LUA_VERSION_NUM < 502
			}
#endif
			break;
		}

		case LUA_TTABLE:	// todo
		case LUA_TTHREAD:
		default:
			luaL_error(L, "unsupported type %s", lua_typename(L, t));
		}
	}
}

// reset w
static inline void
__lreset_var_array(lua_State *L, struct lvar_array_t *w) {
	w->t = 0;
}

static inline struct lvar_array_t *
__lget_var_array(lua_State *L, const int ref) {

	struct lvar_array_t *args = NULL;
	struct lvar_array_t **box;
	lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

	box = (struct lvar_array_t **)lua_touserdata(L, -1);
	args = *box;
	lua_pop(L, 1);
	return args;
}

//////////////////////////////////////////////////////////////////////////
static void
lbind_debug_print(struct lvar_array_t *input, struct lvar_array_t *output) {

	int n = input->t;
	int i;

	printf("\n[mylua_debug_print()]--------------------------------n(%d)\n",
		n);

	for (i = 0; i < n; ++i) {
		struct lvar_t *v = &input->v[i];
		switch (v->type) {
		case LT_NIL: {
			printf("[%d/%d] type(%d) -- (nil)\n", i + 1, n, v->type);
			break;
		}

		case LT_INTEGER: {
#if LUA_VERSION_NUM < 502
			printf("[%d/%d] type(%d) -- %td\n", i + 1, n, v->type, lbind_as_integer(input, i));
#else
			printf("[%d/%d] type(%d) -- %lld\n", i + 1, n, v->type, lbind_as_integer(input, i));
#endif
			break;
		}

		case LT_REAL: {
			printf("[%d/%d] type(%d) -- %f\n", i + 1, n, v->type, lbind_as_real(input, i));
			break;
		}

		case LT_STRING:
		case LT_LSTRING: {
			printf("[%d/%d] type(%d) -- \"%s\"\n", i + 1, n, v->type, lbind_as_string(input, i));
			break;
		}

		case LT_BOOLEAN: {
			printf("[%d/%d] type(%d) -- %d\n", i + 1, n, v->type, lbind_as_boolean(input, i));
			break;
		}

		case LT_POINTER: {
			printf("[%d/%d] type(%d) -- 0x%08Ix\n", i + 1, n, v->type, (uintptr_t)lbind_as_pointer(input, i));
			break;
		}

		case LT_ARRAY:
		case LT_MAP:
		default:
			printf("[%d/%d] type(%d) -- (unsupported type!!!)\n", i + 1, n, v->type);
		}
	}
}

void
lbind_init_once(lua_State *L) {

	int top = lua_gettop(L);

	s_lbind_args = __lalloc_var_array(L);
	s_lbind_ref_args = luaL_ref(L, LUA_REGISTRYINDEX);
	assert(0 == lua_gettop(L) - top);

	s_lbind_result = __lalloc_var_array(L);
	s_lbind_ref_result = luaL_ref(L, LUA_REGISTRYINDEX);
	assert(0 == lua_gettop(L) - top);

	lua_newtable(L);
	s_lbind_ref_func_refid_table = luaL_ref(L, LUA_REGISTRYINDEX);
	assert(0 == lua_gettop(L) - top);

	// Note: call "C.lbind_debug_print(...)" in lua, DON'T FORGET the namespace "C". 
	lbind_register(L, "lbind_debug_print", lbind_debug_print);
	assert(0 == lua_gettop(L) - top);
}
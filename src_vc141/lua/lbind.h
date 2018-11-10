#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <stdint.h>

#define LF_NOTFOUND 1
#define LF_ERRPARSE 2
#define LF_ERRRUN 3

#define LT_NIL 0
#define LT_INTEGER 1
#define LT_REAL 2
#define LT_UINT64 3 // for lua51
#define LT_STRING 4
#define LT_LSTRING 5
#define LT_BOOLEAN 6
#define LT_POINTER 7
#define LT_FUNCREF 8
#define LT_NONE -1

//! not implement yet
#define LT_ARRAY 11
#define LT_MAP 12

//////////////////////////////////////////////////////////////////////////
struct lvar_t {
	int type;
	union {
		lua_Integer i;
		lua_Number f;
		uint64_t u;
		const char *s;
		int b;
		void *p;
		int func_refid;

		//! not implement yet
		//		struct larray *a;
		//		struct lmap *m;
	} val;
	size_t len;
};

struct lvar_array_t {
	int t; // tail index
	int sz; // capacity size
	struct lvar_t *v;
};

//////////////////////////////////////////////////////////////////////////
extern int lbind_ref_function(lua_State* L, int lo);
extern void libind_get_function_by_refid(lua_State* L, int refid);
extern void lbind_remove_function_by_refid(lua_State* L, int refid);
extern void lbind_call_function_by_refid_with_no_arg_return_void(lua_State* L, int refid);
extern void lbind_call_function_by_refid_with_arg_int_return_void(lua_State* L, int refid, int n);

//////////////////////////////////////////////////////////////////////////
extern int lbind_type(struct lvar_array_t *w, int index);

extern lua_Integer lbind_as_integer(struct lvar_array_t *w, int index);
extern lua_Number lbind_as_real(struct lvar_array_t *w, int index);
extern uint64_t lbind_as_uint64(struct lvar_array_t *w, int index);
extern const char * lbind_as_string(struct lvar_array_t *w, int index);
extern const char * lbind_as_lstring(struct lvar_array_t *w, int index, size_t *len);
extern int lbind_as_boolean(struct lvar_array_t *w, int index);
extern void * lbind_as_pointer(struct lvar_array_t *w, int index);
extern int lbind_as_func_refid(struct lvar_array_t *w, int index);

extern void lbind_add_integer(struct lvar_array_t *w, lua_Integer i);
extern void lbind_add_real(struct lvar_array_t *w, lua_Number f);
extern void lbind_add_uint64(struct lvar_array_t *w, uint64_t u);
extern void lbind_add_string(struct lvar_array_t *w, const char *s);
extern void lbind_add_lstring(struct lvar_array_t *w, const char *s, size_t len);
extern void lbind_add_boolean(struct lvar_array_t *w, int b);
extern void lbind_add_nil(struct lvar_array_t *w);
extern void lbind_add_pointer(struct lvar_array_t *w, void *p);
extern void lbind_add_func_refid(struct lvar_array_t *w, int func_refid);

extern struct lvar_array_t * lbind_init_args(lua_State *L);
extern struct lvar_array_t * lbind_call(lua_State *L, const char *funcname, struct lvar_array_t *args);
extern struct lvar_array_t * lbind_call_chunk(lua_State *L, const char *chunk, struct lvar_array_t *args);

typedef void(*lbind_function)(struct lvar_array_t *input, struct lvar_array_t *output);
extern void lbind_register(lua_State *L, const char *funcname, lbind_function f);
extern int lbind_dofile(lua_State *L, const char *filename);
extern int lbind_dostring(lua_State *L, const char *str);

void lbind_init_once(lua_State *L);

#ifdef __cplusplus
}
#endif
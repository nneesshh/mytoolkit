#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <assert.h>

#include "lbind.h"

static void
foobar(struct lvar_array_t *input, struct lvar_array_t *output) {
	assert(lbind_type(input, 0) == LT_INTEGER);
	int a = lbind_as_integer(input, 0);
	lbind_add_integer(output, a*2);
}

int
lbind_test() {
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	lbind_register(L, "double", foobar);

	lbind_dofile(L, "test.lua");

	struct lvar_array_t * args = lbind_init_args(L);
	lbind_add_string(args, "foobar");
	struct lvar_array_t * result = lbind_call(L, "hello", args);
	assert(lbind_type(result, 0) == LT_INTEGER);
#if LUA_VERSION_NUM < 502
	printf("sizeof 'foobar' = %td\n", lbind_as_integer(result, 0));
#else
	printf("sizeof 'foobar' = %lld\n", lbind_as_integer(result, 0));
#endif

	lua_close(L);
	return 0;
}



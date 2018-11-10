#pragma once
#define OOLUA_STORE_LAST_ERROR 1
#include "oolua_src/oolua.h"

// ClassName: declare class, must be in short pattern, no namespace prefix
// MethodEntriesList: declare METHOD and PROPERTY
#define MYLUAGLUE_CLASS(ClassName, MethodEntriesList)	\
	OOLUA_PROXY(ClassName)								\
	MethodEntriesList									\
	OOLUA_PROXY_END

#define MYLUAGLUE_CTORS(ConstructorEntriesList)	\
	OOLUA_TAGS(OOLUA::No_default_constructor)	\
	OOLUA_CTORS(ConstructorEntriesList)

#define MYLUAGLUE_CTOR				OOLUA_CTOR

#define MYLUAGLUE_ENUMS(EnumEntriesList)		\
	OOLUA_TAGS(OOLUA::Register_class_enums)		\
	OOLUA_ENUMS(EnumEntriesList)

#define MYLUAGLUE_ENUM				OOLUA_ENUM

#define MYLUAGLUE_METHOD			OOLUA_MFUNC
#define MYLUAGLUE_METHOD_CONST		OOLUA_MFUNC_CONST
#define MYLUAGLUE_METHOD_FULL		OOLUA_MEM_FUNC
#define MYLUAGLUE_METHOD_CONST_FULL	OOLUA_MEM_FUNC_CONST
#define MYLUAGLUE_METHOD_RN			OOLUA_MEM_FUNC_RENAME
#define MYLUAGLUE_METHOD_CONST_RN	OOLUA_MEM_FUNC_CONST_RENAME
#define MYLUAGLUE_METHOD_STATIC		OOLUA_SFUNC
#define MYLUAGLUE_PROPERTY			OOLUA_MGET_MSET
#define MYLUAGLUE_PROPERTY_GETTER	OOLUA_MGET
#define MYLUAGLUE_PROPERTY_SETTER	OOLUA_MSET

#define MYLUAGLUE_EXPORT_FUNCTIONS			OOLUA_EXPORT_FUNCTIONS
#define MYLUAGLUE_EXPORT_FUNCTIONS_CONST	OOLUA_EXPORT_FUNCTIONS_CONST
#define MYLUAGLUE_EXPORT_NO_FUNCTIONS		OOLUA_EXPORT_NO_FUNCTIONS

#define MYLUAGLUE_C_FUNCTION		OOLUA_CFUNC
#define MYLUAGLUE_C_FUNCTION_FULL	OOLUA_C_FUNCTION

#define MYLUAGLUE_STD_VECTOR(tVectorName, FullVectorName)	\
	using tVectorName = FullVectorName;						\
	MYLUAGLUE_CLASS(tVectorName,							\
		MYLUAGLUE_METHOD_CONST(size)						\
		MYLUAGLUE_METHOD_CONST(max_size)					\
		MYLUAGLUE_METHOD_CONST(capacity)					\
		MYLUAGLUE_METHOD_CONST(empty)						\
		MYLUAGLUE_METHOD_CONST_FULL(tVectorName::const_reference, at, tVectorName::size_type)	\
		MYLUAGLUE_METHOD_FULL(void, reserve, tVectorName::size_type)							\
		MYLUAGLUE_METHOD_FULL(void, resize, tVectorName::size_type)								\
		MYLUAGLUE_METHOD(clear)																	\
		MYLUAGLUE_METHOD_FULL(void, push_back, const tVectorName::value_type&)					\
		MYLUAGLUE_METHOD(pop_back)																\
	)																							\
	MYLUAGLUE_EXPORT_FUNCTIONS_CONST(tVectorName, size, max_size, capacity, empty, at)			\
	MYLUAGLUE_EXPORT_FUNCTIONS(tVectorName, reserve, resize, clear, push_back, pop_back)

namespace luacpp {

#define MyLuaFuncRef OOLUA::Lua_func_ref
#define MyLuaTableRef OOLUA::Lua_table_ref
#define MyLuaFunction OOLUA::Lua_function

class CMyLuaGlue {
public:
	OOLUA::Script _lvm;
	MyLuaFunction _lcaller;

	CMyLuaGlue() : _lcaller(_lvm.state()) {}
	~CMyLuaGlue() {}

	lua_State *					State() {
		return _lvm.state();
	}

	MyLuaFunction&				LCaller() {
		return _lcaller;
	}

	void						Gc() {
		_lvm.gc();
	}

	template<typename T>
	bool						GetGlobal(std::string const& name, T& instance) {
		return OOLUA::get_global(_lvm.state(), name.c_str(), instance);
	}

	void						RunFile(std::string const& filename) {
		if (!_lvm.run_file(filename)) {
			fprintf(stderr, "\n[MyLuaGlue::Call()] run lua file(%s) error -- %s\n",
				filename.c_str(),
				OOLUA::get_last_error(State()).c_str());
		}
	}

	void						RunChunk(std::string const& chunk) {
		if (!_lvm.run_chunk(chunk)) {
			fprintf(stderr, "\n[MyLuaGlue::Call()] run lua chunk(%s) error -- %s\n",
				chunk.c_str(),
				OOLUA::get_last_error(State()).c_str());
		}
	}

	void						RegisterCFunc(char const *funcname, lua_CFunction fn) {
		OOLUA::set_global(State(), funcname, fn);
	}

	template<typename ClassType>
	void						RegisterClass() {
		_lvm.register_class<ClassType>();
	}

	template<typename ClassType, typename K, typename V>
	void						RegisterClassStatic(K const& k, V const& v) {
		_lvm.register_class_static<ClassType, K, V>(k, v);
	}

	template<typename... _Args>
	void						Call(std::string const& funcname, _Args... args) {
		if (!_lvm.call(funcname, std::forward<_Args>(args)...)) {
			std::string errtxt = OOLUA::get_last_error(State());
			fprintf(stderr, "\n!!!!================LUA ERROR================!!!!\n");
			fprintf(stderr, "\n[MyLuaGlue::Call()] call lua function(\"%s\") error -- %s\n",
				funcname.c_str(), errtxt.c_str());

			WriteMyLuaGlueError(funcname.c_str(), errtxt.c_str());
			system("pause");
		}
	}

	template<typename... _Args>
	void						CallChunk(std::string const& func_chunk, _Args... args) {
		bool bSuccess = true;
		int top = lua_gettop(State());
		if (!_lvm.run_chunk(func_chunk)) {
			bSuccess = false;
		}
		else {
			int func_index = lua_gettop(State());
			assert(func_index == top + 1);

			int t = lua_type(State(), func_index);
			assert(t == LUA_TFUNCTION);

			if (!_lvm.call(func_index, std::forward<_Args>(args)...)) {
				bSuccess = false;
			}
		}

		if (!bSuccess) {
			std::string errtxt = OOLUA::get_last_error(State());
			fprintf(stderr, "\n!!!!================LUA ERROR================!!!!\n");
			fprintf(stderr, "\n[MyLuaGlue::CallChunk()] call lua chunk--[[\"%s\"]] error -- %s\n",
				func_chunk.c_str(), errtxt.c_str());

			WriteMyLuaGlueError(func_chunk.c_str(), errtxt.c_str());
			system("pause");

		}
	}

private:
	void						WriteMyLuaGlueError(const char *content_txt, const char *err_txt);

};

}
/*EOF*/
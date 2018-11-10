#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <memory>

#if defined (__cplusplus)
extern "C" {
#endif  /* defined (__cplusplus) */
	
#include "lbind.h"

#if defined (__cplusplus)
}
#endif  /* defined (__cplusplus) */

#include "MyLuaGlue.h"
#include "MyLuaTool.h"

namespace luacpp {

namespace detail {
	struct LuaStateDeleter {
		void operator()(lua_State* L) const {
			if (L) {
				lua_close(L);
			}
		}
	};
}  // namespace

// RAII wrapper around a lua_State
using LuaStatePtr = std::unique_ptr<lua_State, detail::LuaStateDeleter>;
inline LuaStatePtr luaNewState() {
	return LuaStatePtr(luaL_newstate());
}

// LuaStackGuard is a RAII guard to restore the stack to its initial
// condition.
class LuaStackGuard {
public:
	explicit LuaStackGuard(lua_State* L) : L_(L), top_(lua_gettop(L_)) { }

	~LuaStackGuard() {
		if (L_) {
			restore();
		}
	}

	void dismiss() {
		L_ = nullptr;
	}

	void restore() {
		assert(L_);
		assert(lua_gettop(L_) >= top_);
		lua_settop(L_, top_);
	}

private:
	lua_State* L_;
	int top_;
};

class CMyLua {
public:
	CMyLua();
	~CMyLua();

    void DumpStack() const {
		CMyLuaTool::DumpStack(_glue->State());
	}

    lua_State * GlueState() {
        return _glue->State();
    }

    int AddPackagePath(const std::string& sPath) {
        std::string sNewPath = "package.path = package.path .. \"";
        if (sPath.empty())
            return -1;

        if (sPath[0] != ';')
			sNewPath += ";";

        sNewPath += sPath;

        if (sPath[sPath.length() - 1] != '/')
            sNewPath += "/";

        sNewPath += "?.lua\" ";

        RunChunk(sNewPath);
        return 0;
    }

    int RunFile(const std::string& file_name) {
#ifdef _DEBUG
		LuaStackGuard guard(_glue->State());
#endif
		return lbind_dofile(_glue->State(), file_name.c_str());
	}
 
    int RunChunk(const char *str) {
#ifdef _DEBUG
		LuaStackGuard guard(_glue->State());
#endif
		return lbind_dostring(_glue->State(), str);
	}

    void RunChunk(const std::string& str) {
        RunChunk(str.c_str());
    }

	void RegisterCFunc2Lua(const char *sFuncName, lbind_function func) {
#ifdef _DEBUG
		LuaStackGuard guard(_glue->State());
#endif
		lbind_register(_glue->State(), sFuncName, func);
	}

	CMyLuaGlue&				Glue() {
		return (*_glue);
	}

private:
	CMyLuaGlue *_glue;
};

}
/*EOF*/

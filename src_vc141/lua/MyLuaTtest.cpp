#include <iostream>
#include <string>
#include <assert.h>

#include "MyLua.h"
#include "MyLuaTool.h"

#include "luna5.h"

using namespace luacpp;

class base_t
{
public:
    base_t():v(789){}
	void dump()
	{
		printf("in %s a:%d\n", __FUNCTION__, v);
	}
	int v;
};
class foo_t: public base_t
{
public:
	foo_t(int b):a(b)
	{
		printf("in %s b:%d this=%p\n", __FUNCTION__, b, this);
	}
	~foo_t()
	{
		printf("in %s\n", __FUNCTION__);
	}
	void print(int64_t a, base_t* p) const
	{
		printf("in foo_t::print a:%ld p:%p\n", (long)a, p);
	}

	static void dumy()
	{
		printf("in %s\n", __FUNCTION__);
	}
	int a;
};

class clazz{
public:
    static void static_func(){
        printf("in clazz::%s end ------------\n", __FUNCTION__);
    }
};

int mylua_test(int argc, char* argv[])
{
	CMyLua lua;
    try 
    {
        //! 注册C++ 对象到lua中
//        lua.Reg(lua_reg);
        
        //! 载入lua文件
        lua.AddPackagePath("./");
#ifdef _WIN32
        lua.RunFile("../test.lua");
#else
        lua.RunFile("test.lua");
#endif

        //! 执行lua 语句
        lua.RunChunk("print(\"exe run_string!!\")");

		/*
        //! 调用lua函数, 基本类型作为参数
        int32_t arg1 = 1;
        float   arg2 = 2;
        double  arg3 = 3;
		std::string  arg4 = "4";

//         lua.Call<bool>("test_func", arg1, arg2, arg3, arg4);
//         lua.Call<bool>("Mod:funcTest1", arg1, arg2);
        
        //! 调用lua函数，stl类型作为参数， 自动转换为lua talbe
        std::vector<int> vec;			vec.push_back(100);
        std::list<float> lt;			lt.push_back((float)99.99);
        std::set<std::string> st;		st.insert("OhNIce");
        std::map<std::string, int> mp;	mp["key"] = 200;
        lua.Call<std::string>("test_stl", vec, lt, st,  mp);
        
        //! 调用lua 函数返回 talbe，自动转换为stl结构
        vec = lua.Call<std::vector<int> >("test_return_stl_vector");
        lt  = lua.Call<std::list<float> >("test_return_stl_list");
        st  = lua.Call<std::set<std::string> >("test_return_stl_set");
        mp  = lua.Call<std::map<std::string, int> >("test_return_stl_map");
        
        //! 调用lua函数，c++ 对象作为参数, foo_t 必须被注册过
        foo_t* foo_ptr = new foo_t(456);
        lua.Call<void>("test_object", foo_ptr);
        
        //! 调用lua函数，c++ 对象作为返回值, foo_t 必须被注册过 
        assert(foo_ptr == lua.Call<foo_t*>("test_ret_object", foo_ptr));
        //! 调用lua函数，c++ 对象作为返回值, 自动转换为基类
        base_t* base_ptr = lua.Call<base_t*>("test_ret_base_object", foo_ptr);
        assert(base_ptr == foo_ptr);
		*/
 
    }
    catch (std::exception& e)
    {
        printf("exception:%s\n", e.what());


    }
#ifdef _WIN32
    system("pause");
#endif
    return 0;
}

/******************************************************************************
* Name: LuaWrapper for C++
*
* Author: Yong Lin
* Date: 2004-07-19 17:15
* E-Mail: ly4cn@21cn.com
*
* Copyright (C) 2004 Yong Lin.  All rights reserved.
* HomePage︰第二人生 http://www.d2-life.com
*      http://www.d2-life.com/LBS/blogview.asp?logID=41
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#pragma once



#include "Support_Lua.h"
#include "LuaVariant.h"


typedef void					 (*LuaOutputDebugFunction)     ( int iOwnerID, int iTargetID, const char* FunName , const char* Msg );


void	SetLuaDebugOutput( LuaOutputDebugFunction pfun );


//###############################################################################################################
//加入檢查型別
//###############################################################################################################
#define _Lua_Check_ArgNum_( l , index )   { if( CheckLuaArg_Num( l , index ) == false ) return 0; }
#define _Lua_Check_ArgStr_( l , index )   { if( CheckLuaArg_Str( l , index ) == false ) return ""; }

bool CheckLuaArg_Num( lua_State* L , int Index );

bool CheckLuaArg_Str( lua_State* L , int Index );

__int64	StrToInt64( const char* str );
//####################################################################################################################
class luaStack
{
public:
    luaStack( )
        : m_pluaVM( NULL )
    {

    }
	luaStack(lua_State* luaVM)
		: m_pluaVM(luaVM)
	{
	}
public:
	bool operator !()
	{
		return m_pluaVM == NULL;
	}
	operator bool()
	{
		return m_pluaVM != NULL;
	}
	operator lua_State* ()
	{
		return m_pluaVM;
	}

public:
	inline void Push()
	{
		lua_pushnil(m_pluaVM);
	}
	inline void Push(const char* param)
	{
		lua_pushstring(m_pluaVM, param);
	}
	inline void Push(const std::string& param)
	{
		lua_pushlstring(m_pluaVM, param.c_str(), param.size());
	}
	inline void Push(int param)
	{
		lua_pushnumber(m_pluaVM, param);
	}
	inline void Push(__int64 param)
	{
		char str[20];
		sprintf( str , "%I64X" , param );		
		lua_pushlstring(m_pluaVM, str, strlen(str) );
	}
	inline void Push(double param)
	{
		lua_pushnumber(m_pluaVM, param);
	}
	inline void Push(bool param)
	{
		lua_pushboolean(m_pluaVM, param);
	}
	inline void Push(lua_CFunction  param)
	{
		lua_pushcfunction(m_pluaVM, param);
	}
	inline void Push(const LuaTable& param)
	{
		PushTable(param);
	}
	inline void Push(const LuaVariant& value)
	{
		switch(value.GetType())
		{
		case LUAVARIANTTYPE_NUM:
			Push(value.GetValueAsNum());
			break;
		case LUAVARIANTTYPE_STR:
			Push(value.GetValueAsStdStr());
			break;
		case LUAVARIANTTYPE_TABLE:
			PushTable(value.GetValueAsTable());
			break;
		case LUAVARIANTTYPE_BOOL:
			Push(value.GetValueAsBool());
			break;
		case LUAVARIANTTYPE_NIL:
		default:
			Push();
			break;
		}
	}
	template<typename T>
	inline void Push(T* param)
	{
		T** pobj = static_cast<T**>( lua_newuserdata(m_pluaVM, sizeof(T*)) );
		*pobj = param;

		luaL_getmetatable(m_pluaVM, T::GetLuaTypeName());
		lua_setmetatable(m_pluaVM, -2);
	}

public:
	template<typename R>
	inline R Pop(int count = 1)
	{
		R temp = getValue<R>(-count);
		lua_settop(m_pluaVM, (-count)-1);
		return temp;
	}
	inline void Pop(int count = 1)
	{
		lua_settop(m_pluaVM, (-count)-1);
	}
public:
	int GetParamCount()
	{
		return lua_gettop(m_pluaVM);
	}

	template<typename R>
		R GetParam(int id)
	{
		int nTop = GetParamCount();
		LuaAssert(id != 0 && id <= nTop && id >= -nTop, id, "Not enough parameters");
        //LuaAssert( id <= nTop , id, "parameters 個數有問題");
		return getValue<R>(id);
	}

	inline void LuaAssert(bool condition, int argindex, const char* err_msg) 
	{
		luaL_argcheck(m_pluaVM, condition, argindex, err_msg);
	}

	inline void LuaAssert(bool condition, const char* err_msg)
	{
		if(!condition)
		{
			lua_Debug ar;
			lua_getstack(m_pluaVM, 0, &ar);
			lua_getinfo(m_pluaVM, "n", &ar);
			if (ar.name == NULL)
				ar.name = "?";
			luaL_error(m_pluaVM, "assert fail: %s `%s' (%s)", ar.namewhat, ar.name, err_msg);
		}
	}

	template<typename R>
		R GetGlobal(const char* name)
	{
		lua_getglobal(m_pluaVM, name);  
		return Pop<R>();
	}

	template<typename R>
		void SetGlobal(const char* name, R value)
	{
		Push(value);
		lua_setglobal(m_pluaVM, name);
	}

	template<class T>
		void DelGlobalObject(const char* name)
	{
		lua_getglobal(m_pluaVM, name);
		LuaDelObject<T>(m_pluaVM);
		lua_settop(m_pluaVM, -2);

		lua_pushnil(m_pluaVM);
		lua_setglobal(m_pluaVM, name);
	}
private:

	template<typename R>
	inline R getValue(int index = -1)
	{
		return GetValue<R>()(*this, index);
	}

	template<typename R>
	struct GetValue
	{
		R operator()(luaStack& l, int index)
		{
			!!! ERROR !!!;
			return;
		}
	};

	template<>
	struct GetValue<float>
	{
		float operator()(luaStack& l, int index)
		{
            _Lua_Check_ArgNum_( l , index );
			return static_cast<float>(lua_tonumber(l, index));
		}
	};


	template<>
	struct GetValue<double>
	{
		double operator()(luaStack& l, int index)
		{
            _Lua_Check_ArgNum_( l , index );
			return lua_tonumber(l, index);
		}
	};

	template<>
	struct GetValue<long double>
	{
		long double operator()(luaStack& l, int index)
		{
            _Lua_Check_ArgNum_( l , index );
			return lua_tonumber(l, index);
		}
	};

	template<>
	struct GetValue<int>
	{
		int operator()(luaStack& l, int index)
		{
            _Lua_Check_ArgNum_( l , index );
			return static_cast<int>(lua_tonumber(l, index));
		}
	};

	template<>
	struct GetValue<long>
	{
		long operator()(luaStack& l, int index)
		{
            _Lua_Check_ArgNum_( l , index );
			return static_cast<long>(lua_tonumber(l, index));
		}
	};

	template<>
	struct GetValue<__int64>
	{
		__int64 operator()(luaStack& l, int index)
		{
			_Lua_Check_ArgNum_( l , index );
						
			return StrToInt64( lua_tostring(l, index) );
		}
	};

	template<>
	struct GetValue<const char*>
	{
		const char* operator()(luaStack& l, int index)
		{
            _Lua_Check_ArgStr_( l , index );
			return lua_tostring(l, index);
		}
	};

	template<>
	struct GetValue<std::string>
	{
		std::string operator()(luaStack& l, int index)
		{
            _Lua_Check_ArgStr_( l , index );
			return std::string(lua_tostring(l, index), lua_strlen(l, index));
		}
	};

	template<>
	struct GetValue<bool>
	{
		bool operator()(luaStack& l, int index)
		{
//            _Lua_Check_ArgNum_( l , index );
			return lua_toboolean(l, index) != 0;
		}
	};

	template<>
	struct GetValue<LuaTable>
	{
		LuaTable operator()(luaStack& l, int index)
		{
			return l.GetTable(index);
		}
	};

	template<>
	struct GetValue<LuaVariant>
	{
		LuaVariant operator()(luaStack& l, int index)
		{
			LuaVariant value;
			if(lua_istable(l, index))
				value.Set(l.GetTable(index));
			else
			{
				if(lua_isboolean(l, index))
					value.Set(l.getValue<bool>(index));
				else
				{
					if(lua_isnumber(l, index))
						value.Set(l.getValue<double>(index));
					else
						if(lua_isstring(l, index))
							value.Set(l.getValue<std::string>(index));
				}
			}
			return value;
		}
	};

	template<typename R>
	struct GetValue<R*>
	{
		R* operator()(luaStack& l, int index)
		{
			R** pobj = static_cast<R**>( luaL_checkudata(l, index, R::GetLuaTypeName()) );	
			luaL_argcheck(l, pobj != NULL && *pobj != NULL, index, "Invalid object");	
			l.Pop();
			return *pobj;	
		}
	};

public:
	LuaTable GetTable(int index)
	{
		LuaTable table;
		lua_pushvalue(m_pluaVM, index);
		if(lua_istable(m_pluaVM, -1))
		{
			lua_pushnil(m_pluaVM);
			for (;;) 
			{
				if (lua_next(m_pluaVM, -2) == 0)
					break;
				lua_pushvalue(m_pluaVM, -2);
				lua_pushvalue(m_pluaVM, -2);
				LuaVariant key = getValue<LuaVariant>( -2);
				LuaVariant value = getValue<LuaVariant>( -1);

				table[key] = value;
				lua_pop(m_pluaVM, 3);  
			}
		}
		lua_pop(m_pluaVM, 1);
		return table;
	}

	void PushTable(const LuaTable& table)
	{
		lua_newtable(m_pluaVM);
		LuaTable::const_iterator end = table.end();
		for(LuaTable::const_iterator i = table.begin(); i != end; ++i)
		{
			Push(i->first);
			Push(i->second);
			lua_settable(m_pluaVM, -3);		
		}
	}

private:
	inline void beginCall(const char* func)
	{
		lua_getglobal(m_pluaVM, func);  
	}

	template<typename R>
		R endCall(int nArg)
	{
		if(lua_pcall(m_pluaVM, nArg, 1, 0) != 0)
		{
			Pop(); // Pop error message
			return 0;
		}

		return Pop<R>();
	}

	template<>
		void endCall(int nArg)
	{
		if(lua_pcall(m_pluaVM, nArg, 0, 0) != 0)
			Pop(); // Pop error message
	}
public:
	// Call Lua function
	//   func:	Lua function name
	//   R:		Return type. (void, float, double, int, long, bool, const char*, std::string)
	// Sample:	double f = lua.Call<double>("test0", 1.0, 3, "param");
	template<typename R>
		R Call(const char* func)
	{
		beginCall(func);
		return endCall<R>(0);
	}

	template<typename R, typename P1>
		R Call(const char* func, P1 p1)
	{
		beginCall(func);
		Push(p1);
		return endCall<R>(1);
	}

	template<typename R, typename P1, typename P2>
		R Call(const char* func, P1 p1, P2 p2)
	{
		beginCall(func);
		Push(p1);
		Push(p2);
		return endCall<R>(2);
	}

	template<typename R, typename P1, typename P2, typename P3>
		R Call(const char* func, P1 p1, P2 p2, P3 p3)
	{
		beginCall(func);
		Push(p1);
		Push(p2);
		Push(p3);
		return endCall<R>(3);
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4>
		R Call(const char* func, P1 p1, P2 p2, P3 p3, P4 p4)
	{
		beginCall(func);
		Push(p1);
		Push(p2);
		Push(p3);
		Push(p4);
		return endCall<R>(4);
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		R Call(const char* func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
	{
		beginCall(func);
		Push(p1);
		Push(p2);
		Push(p3);
		Push(p4);
		Push(p5);
		return endCall<R>(5);
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		R Call(const char* func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
	{
		beginCall(func);
		Push(p1);
		Push(p2);
		Push(p3);
		Push(p4);
		Push(p5);
		Push(p6);
		return endCall<R>(6);
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		R Call(const char* func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
	{
		beginCall(func);
		Push(p1);
		Push(p2);
		Push(p3);
		Push(p4);
		Push(p5);
		Push(p6);
		Push(p7);
		return endCall<R>(7);
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		R Call(const char* func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
	{
		beginCall(func);
		Push(p1);
		Push(p2);
		Push(p3);
		Push(p4);
		Push(p5);
		Push(p6);
		Push(p7);
		Push(p8);
		return endCall<R>(8);
	}


	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		R Call(const char* func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
	{
		beginCall(func);
		Push(p1);
		Push(p2);
		Push(p3);
		Push(p4);
		Push(p5);
		Push(p6);
		Push(p7);
		Push(p8);
		Push(p9);
		return endCall<R>(9);
	}



	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
		R Call(const char* func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
	{
		beginCall(func);
		Push(p1);
		Push(p2);
		Push(p3);
		Push(p4);
		Push(p5);
		Push(p6);
		Push(p7);
		Push(p8);
		Push(p9);
		Push(p10);
		return endCall<R>(10);
	}

public:
	/************************************************************************
	* Here:
	* This function "const char* Call(const char*, const char*, ...)" is come from "Programming in Lua"
	*
	* BOOK:	Programming in Lua
	*		by Roberto Ierusalimschy 
	*		Lua.org, December 2003 
	*		ISBN 85-903798-1-7 
	************************************************************************/

	// 調用Lua函數，當 Call<>()不能滿足要求時，可以使用此函數
	//   func 是函數名
	//   sig  是函數簽名
	//        格式如p*[:r*]或p*[>r*], 冒號或大于號前面為參數，冒號或大于號后面為返回值，每個字母為一個參數或返回值的類型
	//        類型表示為︰ f 或 e 表示浮點數； i 或 n 或 d 表示整數； b 表示布爾值； s 表示字元串
	//                     S 表示指定長度的字元串，長度在前，字元串指針在后。
	//   返回 返回值如果為 NULL， 表示函數調用成功。否則為失敗的訊息。
	//
	// 例1︰ double f; const char* error_msg = lua.Call("test01", "nnnn:f", 1,2,3,4,&f);
	// 例2︰ const char* s; int len; const char* error_msg = lua.Call("test01", "S:S", 11, "Hello\0World", &len, &s);
	const char* Call (const char *func, const char *sig, ...)
	{
		va_list vl;
		va_start(vl, sig);

		lua_getglobal(m_pluaVM, func);  

		/* 壓入調用參數 */
		int narg = 0;
		while (*sig) 
		{  /* push arguments */
			switch (*sig++) 
			{
			case 'f':	/* 浮點數 */
			case 'e':	/* 浮點數 */
				lua_pushnumber(m_pluaVM, va_arg(vl, double));
				break;

			case 'i':	/* 整數 */
			case 'n':	/* 整數 */
			case 'd':	/* 整數 */
				lua_pushnumber(m_pluaVM, va_arg(vl, int));
				break;

			case 'b':	/* 布爾值 */
				lua_pushboolean(m_pluaVM, va_arg(vl, int));
				break;

			case 's':	/* 字元串 */
				lua_pushstring(m_pluaVM, va_arg(vl, char *));
				break;

			case 'S':	/* 字元串 */
				{
					int len = va_arg(vl, int);
					lua_pushlstring(m_pluaVM, va_arg(vl, char *), len);
				}
				break;

			case '>':
			case ':':
				goto L_LuaCall;

			default:
				//assert(("Lua call option is invalid!", false));
				//error(m_pluaVM, "invalid option (%c)", *(sig - 1));
				lua_pushnumber(m_pluaVM, 0);
			}
			narg++;
			luaL_checkstack(m_pluaVM, 1, "too many arguments");
		} 

L_LuaCall:
		int nres = static_cast<int>(strlen(sig)); 
		const char* sresult = NULL;
		if (lua_pcall(m_pluaVM, narg, nres, 0) != 0)
		{
			sresult = lua_tostring(m_pluaVM, -1);
			nres = 1;
		}
		else
		{
			// 取得返回值
			int index = -nres;  
			while (*sig)
			{  
				switch (*sig++) 
				{
				case 'f':	/* 浮點數 */
				case 'e':	/* 浮點數 */
					*va_arg(vl, double *) = lua_tonumber(m_pluaVM, index);
					break;

				case 'i':	/* 整數 */
				case 'n':	/* 整數 */
				case 'd':	/* 整數 */
					*va_arg(vl, int *) = static_cast<int>(lua_tonumber(m_pluaVM, index));
					break;

				case 'b':	/* 布爾值 */
					*va_arg(vl, int *) = static_cast<int>(lua_toboolean(m_pluaVM, index));
					break;

				case 's':	/* 字元串 */
					*va_arg(vl, const char **) = lua_tostring(m_pluaVM, index);
					break;

				case 'S':	/* 字元串 */
					*va_arg(vl, int *) = static_cast<int>(lua_strlen(m_pluaVM, index));
					*va_arg(vl, const char **) = lua_tostring(m_pluaVM, index);
					break;

				default:
					//assert(("Lua call invalid option!", false));
					//error(m_pluaVM, "invalid option (%c)", *(sig - 1));
					;
				}
				index++;
			}	
		}
		va_end(vl);

		lua_pop(m_pluaVM, nres);
		return sresult;
	}

protected:
	lua_State* m_pluaVM;
};

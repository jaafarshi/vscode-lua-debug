#if defined(DEBUGGER_BRIDGE)

#include <debugger/bridge/lua.h>
#include <Windows.h>
#include <lobject.h>

namespace lua { 
	version ver = version::v53;

	version get_version() {
		return ver;
	}

	void check_version(void* m) {
		if (m && GetProcAddress((HMODULE)m, "lua_getiuservalue")) {
			ver = version::v54;
		}
	}

	lua_Integer lua_getprotohash(lua_State *L, int idx) {
		if (!lua_isfunction(L, idx) || lua_iscfunction(L, idx))
			return 0;
		const LClosure *c = (const LClosure *)lua_topointer(L, idx);
		return c->p ? (lua_Integer)c->p ^ (lua_Integer)c->p->code : 0;
	}

namespace lua54 {
	int(__cdecl* lua_getiuservalue)(lua_State *L, int idx, int n);
	int __cdecl lua_getuservalue(lua_State* L, int idx) {
		return lua_getiuservalue(L, idx, 1);
	}

	int(__cdecl* lua_setiuservalue)(lua_State *L, int idx, int n);
	int __cdecl lua_setuservalue(lua_State* L, int idx) {
		return lua_setiuservalue(L, idx, 1);
	}

	void*(__cdecl* lua_newuserdatauv)(lua_State *L, size_t size, int nuvalue);
	void* __cdecl lua_newuserdata(lua_State* L, size_t size, int nuvalue) {
		return lua_newuserdatauv(L, size, 1);
	}
}}
#endif

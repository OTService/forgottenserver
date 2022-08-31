// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LUAPOSITION_H
#define FS_LUAPOSITION_H

#include "luascript.h"

class LuaPosition : public LuaScriptInterface
{
public:
	static int luaPositionCreate(lua_State* L);
	static int luaPositionAdd(lua_State* L);
	static int luaPositionSub(lua_State* L);
	static int luaPositionCompare(lua_State* L);

	static int luaPositionGetDistance(lua_State* L);
	static int luaPositionIsSightClear(lua_State* L);

	static int luaPositionSendMagicEffect(lua_State* L);
	static int luaPositionSendDistanceEffect(lua_State* L);

private:
	friend class LuaScriptInterface;
};

#endif // FS_LUAPOSITION_H

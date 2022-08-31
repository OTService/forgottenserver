// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LUAMONSTER_H
#define FS_LUAMONSTER_H

#include "luascript.h"

class LuaMonster : public LuaScriptInterface
{
public:
	static int luaMonsterCreate(lua_State* L);

	static int luaMonsterIsMonster(lua_State* L);

	static int luaMonsterGetType(lua_State* L);

	static int luaMonsterRename(lua_State* L);

	static int luaMonsterGetSpawnPosition(lua_State* L);
	static int luaMonsterIsInSpawnRange(lua_State* L);

	static int luaMonsterIsIdle(lua_State* L);
	static int luaMonsterSetIdle(lua_State* L);

	static int luaMonsterIsTarget(lua_State* L);
	static int luaMonsterIsOpponent(lua_State* L);
	static int luaMonsterIsFriend(lua_State* L);

	static int luaMonsterAddFriend(lua_State* L);
	static int luaMonsterRemoveFriend(lua_State* L);
	static int luaMonsterGetFriendList(lua_State* L);
	static int luaMonsterGetFriendCount(lua_State* L);

	static int luaMonsterAddTarget(lua_State* L);
	static int luaMonsterRemoveTarget(lua_State* L);
	static int luaMonsterGetTargetList(lua_State* L);
	static int luaMonsterGetTargetCount(lua_State* L);

	static int luaMonsterSelectTarget(lua_State* L);
	static int luaMonsterSearchTarget(lua_State* L);

	static int luaMonsterIsWalkingToSpawn(lua_State* L);
	static int luaMonsterWalkToSpawn(lua_State* L);

private:
	friend class LuaScriptInterface;
};

#endif // FS_LUAMONSTER_H

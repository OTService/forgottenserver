// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LUAGAME_H
#define FS_LUAGAME_H

#include "luascript.h"

class LuaGame : public LuaScriptInterface
{
public:
	static int luaGameGetSpectators(lua_State* L);
	static int luaGameGetPlayers(lua_State* L);
	static int luaGameGetNpcs(lua_State* L);
	static int luaGameGetMonsters(lua_State* L);
	static int luaGameLoadMap(lua_State* L);

	static int luaGameGetExperienceStage(lua_State* L);
	static int luaGameGetExperienceForLevel(lua_State* L);
	static int luaGameGetMonsterCount(lua_State* L);
	static int luaGameGetPlayerCount(lua_State* L);
	static int luaGameGetNpcCount(lua_State* L);
	static int luaGameGetMonsterTypes(lua_State* L);
	static int luaGameGetCurrencyItems(lua_State* L);
	static int luaGameGetItemTypeByClientId(lua_State* L);
	static int luaGameGetMountIdByLookType(lua_State* L);

	static int luaGameGetTowns(lua_State* L);
	static int luaGameGetHouses(lua_State* L);
	static int luaGameGetOutfits(lua_State* L);
	static int luaGameGetMounts(lua_State* L);

	static int luaGameGetGameState(lua_State* L);
	static int luaGameSetGameState(lua_State* L);

	static int luaGameGetWorldType(lua_State* L);
	static int luaGameSetWorldType(lua_State* L);

	static int luaGameGetItemAttributeByName(lua_State* L);
	static int luaGameGetReturnMessage(lua_State* L);

	static int luaGameCreateItem(lua_State* L);
	static int luaGameCreateContainer(lua_State* L);
	static int luaGameCreateMonster(lua_State* L);
	static int luaGameCreateNpc(lua_State* L);
	static int luaGameCreateTile(lua_State* L);
	static int luaGameCreateMonsterType(lua_State* L);

	static int luaGameStartRaid(lua_State* L);

	static int luaGameGetClientVersion(lua_State* L);

	static int luaGameReload(lua_State* L);

	static int luaGameGetAccountStorageValue(lua_State* L);
	static int luaGameSetAccountStorageValue(lua_State* L);
	static int luaGameSaveAccountStorageValues(lua_State* L);

private:
	friend class LuaScriptInterface;
};

#endif // FS_LUAGAME_H

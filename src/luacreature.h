// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LUACREATURE_H
#define FS_LUACREATURE_H

#include "luascript.h"

class LuaCreature : public LuaScriptInterface
{
public:
	static int luaCreatureCreate(lua_State* L);

	static int luaCreatureGetEvents(lua_State* L);
	static int luaCreatureRegisterEvent(lua_State* L);
	static int luaCreatureUnregisterEvent(lua_State* L);

	static int luaCreatureIsRemoved(lua_State* L);
	static int luaCreatureIsCreature(lua_State* L);
	static int luaCreatureIsInGhostMode(lua_State* L);
	static int luaCreatureIsHealthHidden(lua_State* L);
	static int luaCreatureIsMovementBlocked(lua_State* L);
	static int luaCreatureIsImmune(lua_State* L);

	static int luaCreatureCanSee(lua_State* L);
	static int luaCreatureCanSeeCreature(lua_State* L);
	static int luaCreatureCanSeeGhostMode(lua_State* L);
	static int luaCreatureCanSeeInvisibility(lua_State* L);

	static int luaCreatureGetParent(lua_State* L);

	static int luaCreatureGetId(lua_State* L);
	static int luaCreatureGetName(lua_State* L);

	static int luaCreatureGetTarget(lua_State* L);
	static int luaCreatureSetTarget(lua_State* L);

	static int luaCreatureGetFollowCreature(lua_State* L);
	static int luaCreatureSetFollowCreature(lua_State* L);

	static int luaCreatureGetMaster(lua_State* L);
	static int luaCreatureSetMaster(lua_State* L);

	static int luaCreatureGetLight(lua_State* L);
	static int luaCreatureSetLight(lua_State* L);

	static int luaCreatureGetSpeed(lua_State* L);
	static int luaCreatureGetBaseSpeed(lua_State* L);
	static int luaCreatureChangeSpeed(lua_State* L);

	static int luaCreatureSetDropLoot(lua_State* L);
	static int luaCreatureSetSkillLoss(lua_State* L);

	static int luaCreatureGetPosition(lua_State* L);
	static int luaCreatureGetTile(lua_State* L);
	static int luaCreatureGetDirection(lua_State* L);
	static int luaCreatureSetDirection(lua_State* L);

	static int luaCreatureGetHealth(lua_State* L);
	static int luaCreatureSetHealth(lua_State* L);
	static int luaCreatureAddHealth(lua_State* L);
	static int luaCreatureGetMaxHealth(lua_State* L);
	static int luaCreatureSetMaxHealth(lua_State* L);
	static int luaCreatureSetHiddenHealth(lua_State* L);
	static int luaCreatureSetMovementBlocked(lua_State* L);

	static int luaCreatureGetSkull(lua_State* L);
	static int luaCreatureSetSkull(lua_State* L);

	static int luaCreatureGetOutfit(lua_State* L);
	static int luaCreatureSetOutfit(lua_State* L);

	static int luaCreatureGetCondition(lua_State* L);
	static int luaCreatureAddCondition(lua_State* L);
	static int luaCreatureRemoveCondition(lua_State* L);
	static int luaCreatureHasCondition(lua_State* L);

	static int luaCreatureRemove(lua_State* L);
	static int luaCreatureTeleportTo(lua_State* L);
	static int luaCreatureSay(lua_State* L);

	static int luaCreatureGetDamageMap(lua_State* L);

	static int luaCreatureGetSummons(lua_State* L);

	static int luaCreatureGetDescription(lua_State* L);

	static int luaCreatureGetPathTo(lua_State* L);
	static int luaCreatureMove(lua_State* L);

	static int luaCreatureGetZone(lua_State* L);

private:
	friend class LuaScriptInterface;
};

#endif // FS_LUACREATURE_H

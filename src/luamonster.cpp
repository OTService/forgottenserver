// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "game.h"
#include "luascript.h"
#include "monster.h"

extern Game g_game;

using namespace Lua;

static int luaMonsterCreate(lua_State* L)
{
	// Monster(id or userdata)
	Monster* monster;
	if (isNumber(L, 2)) {
		monster = g_game.getMonsterByID(getNumber<uint32_t>(L, 2));
	} else if (isUserdata(L, 2)) {
		if (getUserdataType(L, 2) != LuaData_Monster) {
			lua_pushnil(L);
			return 1;
		}
		monster = getUserdata<Monster>(L, 2);
	} else {
		monster = nullptr;
	}

	if (monster) {
		pushUserdata<Monster>(L, monster);
		setMetatable(L, -1, "Monster");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterIsMonster(lua_State* L)
{
	// monster:isMonster()
	pushBoolean(L, getUserdata<const Monster>(L, 1) != nullptr);
	return 1;
}

static int luaMonsterGetType(lua_State* L)
{
	// monster:getType()
	const Monster* monster = getUserdata<const Monster>(L, 1);
	if (monster) {
		pushUserdata<MonsterType>(L, monster->mType);
		setMetatable(L, -1, "MonsterType");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterRename(lua_State* L)
{
	// monster:rename(name[, nameDescription])
	Monster* monster = getUserdata<Monster>(L, 1);
	if (!monster) {
		lua_pushnil(L);
		return 1;
	}

	monster->setName(getString(L, 2));
	if (lua_gettop(L) >= 3) {
		monster->setNameDescription(getString(L, 3));
	}

	pushBoolean(L, true);
	return 1;
}

static int luaMonsterGetSpawnPosition(lua_State* L)
{
	// monster:getSpawnPosition()
	const Monster* monster = getUserdata<const Monster>(L, 1);
	if (monster) {
		pushPosition(L, monster->getMasterPos());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterIsInSpawnRange(lua_State* L)
{
	// monster:isInSpawnRange([position])
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		pushBoolean(L, monster->isInSpawnRange(lua_gettop(L) >= 2 ? getPosition(L, 2) : monster->getPosition()));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterIsIdle(lua_State* L)
{
	// monster:isIdle()
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		pushBoolean(L, monster->getIdleStatus());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterSetIdle(lua_State* L)
{
	// monster:setIdle(idle)
	Monster* monster = getUserdata<Monster>(L, 1);
	if (!monster) {
		lua_pushnil(L);
		return 1;
	}

	monster->setIdle(getBoolean(L, 2));
	pushBoolean(L, true);
	return 1;
}

static int luaMonsterIsTarget(lua_State* L)
{
	// monster:isTarget(creature)
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		const Creature* creature = getCreature(L, 2);
		if (!creature) {
			reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			pushBoolean(L, false);
			return 1;
		}

		pushBoolean(L, monster->isTarget(creature));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterIsOpponent(lua_State* L)
{
	// monster:isOpponent(creature)
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		const Creature* creature = getCreature(L, 2);
		if (!creature) {
			reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			pushBoolean(L, false);
			return 1;
		}

		pushBoolean(L, monster->isOpponent(creature));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterIsFriend(lua_State* L)
{
	// monster:isFriend(creature)
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		const Creature* creature = getCreature(L, 2);
		if (!creature) {
			reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			pushBoolean(L, false);
			return 1;
		}

		pushBoolean(L, monster->isFriend(creature));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterAddFriend(lua_State* L)
{
	// monster:addFriend(creature)
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		Creature* creature = getCreature(L, 2);
		if (!creature) {
			reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			pushBoolean(L, false);
			return 1;
		}

		monster->addFriend(creature);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterRemoveFriend(lua_State* L)
{
	// monster:removeFriend(creature)
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		Creature* creature = getCreature(L, 2);
		if (!creature) {
			reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			pushBoolean(L, false);
			return 1;
		}

		monster->removeFriend(creature);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterGetFriendList(lua_State* L)
{
	// monster:getFriendList()
	Monster* monster = getUserdata<Monster>(L, 1);
	if (!monster) {
		lua_pushnil(L);
		return 1;
	}

	const auto& friendList = monster->getFriendList();
	lua_createtable(L, friendList.size(), 0);

	int index = 0;
	for (Creature* creature : friendList) {
		pushUserdata<Creature>(L, creature);
		setCreatureMetatable(L, -1, creature);
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaMonsterGetFriendCount(lua_State* L)
{
	// monster:getFriendCount()
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		lua_pushnumber(L, monster->getFriendList().size());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterAddTarget(lua_State* L)
{
	// monster:addTarget(creature[, pushFront = false])
	Monster* monster = getUserdata<Monster>(L, 1);
	if (!monster) {
		lua_pushnil(L);
		return 1;
	}

	Creature* creature = getCreature(L, 2);
	if (!creature) {
		reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	bool pushFront = getBoolean(L, 3, false);
	monster->addTarget(creature, pushFront);
	pushBoolean(L, true);
	return 1;
}

static int luaMonsterRemoveTarget(lua_State* L)
{
	// monster:removeTarget(creature)
	Monster* monster = getUserdata<Monster>(L, 1);
	if (!monster) {
		lua_pushnil(L);
		return 1;
	}

	Creature* creature = getCreature(L, 2);
	if (!creature) {
		reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	monster->removeTarget(creature);
	pushBoolean(L, true);
	return 1;
}

static int luaMonsterGetTargetList(lua_State* L)
{
	// monster:getTargetList()
	Monster* monster = getUserdata<Monster>(L, 1);
	if (!monster) {
		lua_pushnil(L);
		return 1;
	}

	const auto& targetList = monster->getTargetList();
	lua_createtable(L, targetList.size(), 0);

	int index = 0;
	for (Creature* creature : targetList) {
		pushUserdata<Creature>(L, creature);
		setCreatureMetatable(L, -1, creature);
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaMonsterGetTargetCount(lua_State* L)
{
	// monster:getTargetCount()
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		lua_pushnumber(L, monster->getTargetList().size());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterSelectTarget(lua_State* L)
{
	// monster:selectTarget(creature)
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		Creature* creature = getCreature(L, 2);
		if (!creature) {
			reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			pushBoolean(L, false);
			return 1;
		}

		pushBoolean(L, monster->selectTarget(creature));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterSearchTarget(lua_State* L)
{
	// monster:searchTarget([searchType = TARGETSEARCH_DEFAULT])
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		TargetSearchType_t searchType = getNumber<TargetSearchType_t>(L, 2, TARGETSEARCH_DEFAULT);
		pushBoolean(L, monster->searchTarget(searchType));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterIsWalkingToSpawn(lua_State* L)
{
	// monster:isWalkingToSpawn()
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		pushBoolean(L, monster->isWalkingToSpawn());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterWalkToSpawn(lua_State* L)
{
	// monster:walkToSpawn()
	Monster* monster = getUserdata<Monster>(L, 1);
	if (monster) {
		pushBoolean(L, monster->walkToSpawn());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

void LuaScriptInterface::registerMonsterFunctions()
{
	registerClass("Monster", "Creature", luaMonsterCreate);
	registerMetaMethod("Monster", "__eq", luaUserdataCompare);

	registerMethod("Monster", "isMonster", luaMonsterIsMonster);

	registerMethod("Monster", "getType", luaMonsterGetType);

	registerMethod("Monster", "rename", luaMonsterRename);

	registerMethod("Monster", "getSpawnPosition", luaMonsterGetSpawnPosition);
	registerMethod("Monster", "isInSpawnRange", luaMonsterIsInSpawnRange);

	registerMethod("Monster", "isIdle", luaMonsterIsIdle);
	registerMethod("Monster", "setIdle", luaMonsterSetIdle);

	registerMethod("Monster", "isTarget", luaMonsterIsTarget);
	registerMethod("Monster", "isOpponent", luaMonsterIsOpponent);
	registerMethod("Monster", "isFriend", luaMonsterIsFriend);

	registerMethod("Monster", "addFriend", luaMonsterAddFriend);
	registerMethod("Monster", "removeFriend", luaMonsterRemoveFriend);
	registerMethod("Monster", "getFriendList", luaMonsterGetFriendList);
	registerMethod("Monster", "getFriendCount", luaMonsterGetFriendCount);

	registerMethod("Monster", "addTarget", luaMonsterAddTarget);
	registerMethod("Monster", "removeTarget", luaMonsterRemoveTarget);
	registerMethod("Monster", "getTargetList", luaMonsterGetTargetList);
	registerMethod("Monster", "getTargetCount", luaMonsterGetTargetCount);

	registerMethod("Monster", "selectTarget", luaMonsterSelectTarget);
	registerMethod("Monster", "searchTarget", luaMonsterSearchTarget);

	registerMethod("Monster", "isWalkingToSpawn", luaMonsterIsWalkingToSpawn);
	registerMethod("Monster", "walkToSpawn", luaMonsterWalkToSpawn);
}

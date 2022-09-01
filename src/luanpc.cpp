// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "game.h"
#include "luascript.h"
#include "npc.h"

extern Game g_game;

using namespace Lua;

// Npc
static int luaNpcCreate(lua_State* L)
{
	// Npc([id or name or userdata])
	Npc* npc;
	if (lua_gettop(L) >= 2) {
		if (isNumber(L, 2)) {
			npc = g_game.getNpcByID(getNumber<uint32_t>(L, 2));
		} else if (isString(L, 2)) {
			npc = g_game.getNpcByName(getString(L, 2));
		} else if (isUserdata(L, 2)) {
			if (getUserdataType(L, 2) != LuaData_Npc) {
				lua_pushnil(L);
				return 1;
			}
			npc = getUserdata<Npc>(L, 2);
		} else {
			npc = nullptr;
		}
	} else {
		npc = LuaScriptInterface::getScriptEnv()->getNpc();
	}

	if (npc) {
		pushUserdata<Npc>(L, npc);
		setMetatable(L, -1, "Npc");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaNpcIsNpc(lua_State* L)
{
	// npc:isNpc()
	pushBoolean(L, getUserdata<const Npc>(L, 1) != nullptr);
	return 1;
}

static int luaNpcSetMasterPos(lua_State* L)
{
	// npc:setMasterPos(pos[, radius])
	Npc* npc = getUserdata<Npc>(L, 1);
	if (!npc) {
		lua_pushnil(L);
		return 1;
	}

	const Position& pos = getPosition(L, 2);
	int32_t radius = getNumber<int32_t>(L, 3, 1);
	npc->setMasterPos(pos, radius);
	pushBoolean(L, true);
	return 1;
}

static int luaNpcGetSpeechBubble(lua_State* L)
{
	// npc:getSpeechBubble()
	Npc* npc = getUserdata<Npc>(L, 1);
	if (npc) {
		lua_pushnumber(L, npc->getSpeechBubble());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaNpcSetSpeechBubble(lua_State* L)
{
	// npc:setSpeechBubble(speechBubble)
	Npc* npc = getUserdata<Npc>(L, 1);
	if (!npc) {
		lua_pushnil(L);
		return 1;
	}

	if (!isNumber(L, 2)) {
		lua_pushnil(L);
		return 1;
	}

	uint8_t speechBubble = getNumber<uint8_t>(L, 2);
	if (speechBubble > SPEECHBUBBLE_LAST) {
		lua_pushnil(L);
	} else {
		npc->setSpeechBubble(speechBubble);
		pushBoolean(L, true);
	}
	return 1;
}

namespace LuaNpc {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("Npc", "Creature", luaNpcCreate);
	interface->registerMetaMethod("Npc", "__eq", interface->luaUserdataCompare);

	interface->registerMethod("Npc", "isNpc", luaNpcIsNpc);

	interface->registerMethod("Npc", "setMasterPos", luaNpcSetMasterPos);

	interface->registerMethod("Npc", "getSpeechBubble", luaNpcGetSpeechBubble);
	interface->registerMethod("Npc", "setSpeechBubble", luaNpcSetSpeechBubble);
}
} // namespace LuaNpc

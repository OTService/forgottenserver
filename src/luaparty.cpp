// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "game.h"
#include "luascript.h"
#include "party.h"

extern Game g_game;

using namespace Lua;

static int luaPartyCreate(lua_State* L)
{
	// Party(userdata)
	Player* player = getUserdata<Player>(L, 2);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	Party* party = player->getParty();
	if (!party) {
		party = new Party(player);
		g_game.updatePlayerShield(player);
		player->sendCreatureSkull(player);
		pushUserdata<Party>(L, party);
		setMetatable(L, -1, "Party");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartyDisband(lua_State* L)
{
	// party:disband()
	Party** partyPtr = getRawUserdata<Party>(L, 1);
	if (partyPtr && *partyPtr) {
		Party*& party = *partyPtr;
		party->disband();
		party = nullptr;
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartyGetLeader(lua_State* L)
{
	// party:getLeader()
	Party* party = getUserdata<Party>(L, 1);
	if (!party) {
		lua_pushnil(L);
		return 1;
	}

	Player* leader = party->getLeader();
	if (leader) {
		pushUserdata<Player>(L, leader);
		setMetatable(L, -1, "Player");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartySetLeader(lua_State* L)
{
	// party:setLeader(player)
	Player* player = getPlayer(L, 2);
	Party* party = getUserdata<Party>(L, 1);
	if (party && player) {
		pushBoolean(L, party->passPartyLeadership(player));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartyGetMembers(lua_State* L)
{
	// party:getMembers()
	Party* party = getUserdata<Party>(L, 1);
	if (!party) {
		lua_pushnil(L);
		return 1;
	}

	int index = 0;
	lua_createtable(L, party->getMemberCount(), 0);
	for (Player* player : party->getMembers()) {
		pushUserdata<Player>(L, player);
		setMetatable(L, -1, "Player");
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaPartyGetMemberCount(lua_State* L)
{
	// party:getMemberCount()
	Party* party = getUserdata<Party>(L, 1);
	if (party) {
		lua_pushnumber(L, party->getMemberCount());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartyGetInvitees(lua_State* L)
{
	// party:getInvitees()
	Party* party = getUserdata<Party>(L, 1);
	if (party) {
		lua_createtable(L, party->getInvitationCount(), 0);

		int index = 0;
		for (Player* player : party->getInvitees()) {
			pushUserdata<Player>(L, player);
			setMetatable(L, -1, "Player");
			lua_rawseti(L, -2, ++index);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartyGetInviteeCount(lua_State* L)
{
	// party:getInviteeCount()
	Party* party = getUserdata<Party>(L, 1);
	if (party) {
		lua_pushnumber(L, party->getInvitationCount());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartyAddInvite(lua_State* L)
{
	// party:addInvite(player)
	Player* player = getPlayer(L, 2);
	Party* party = getUserdata<Party>(L, 1);
	if (party && player) {
		pushBoolean(L, party->invitePlayer(*player));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartyRemoveInvite(lua_State* L)
{
	// party:removeInvite(player)
	Player* player = getPlayer(L, 2);
	Party* party = getUserdata<Party>(L, 1);
	if (party && player) {
		pushBoolean(L, party->removeInvite(*player));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartyAddMember(lua_State* L)
{
	// party:addMember(player)
	Player* player = getPlayer(L, 2);
	Party* party = getUserdata<Party>(L, 1);
	if (party && player) {
		pushBoolean(L, party->joinParty(*player));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartyRemoveMember(lua_State* L)
{
	// party:removeMember(player)
	Player* player = getPlayer(L, 2);
	Party* party = getUserdata<Party>(L, 1);
	if (party && player) {
		pushBoolean(L, party->leaveParty(player));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartyIsSharedExperienceActive(lua_State* L)
{
	// party:isSharedExperienceActive()
	Party* party = getUserdata<Party>(L, 1);
	if (party) {
		pushBoolean(L, party->isSharedExperienceActive());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartyIsSharedExperienceEnabled(lua_State* L)
{
	// party:isSharedExperienceEnabled()
	Party* party = getUserdata<Party>(L, 1);
	if (party) {
		pushBoolean(L, party->isSharedExperienceEnabled());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartyShareExperience(lua_State* L)
{
	// party:shareExperience(experience)
	uint64_t experience = getNumber<uint64_t>(L, 2);
	Party* party = getUserdata<Party>(L, 1);
	if (party) {
		party->shareExperience(experience);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPartySetSharedExperience(lua_State* L)
{
	// party:setSharedExperience(active)
	bool active = getBoolean(L, 2);
	Party* party = getUserdata<Party>(L, 1);
	if (party) {
		pushBoolean(L, party->setSharedExperience(party->getLeader(), active));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

namespace LuaParty {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("Party", "", luaPartyCreate);
	interface->registerMetaMethod("Party", "__eq", interface->luaUserdataCompare);

	interface->registerMethod("Party", "disband", luaPartyDisband);

	interface->registerMethod("Party", "getLeader", luaPartyGetLeader);
	interface->registerMethod("Party", "setLeader", luaPartySetLeader);

	interface->registerMethod("Party", "getMembers", luaPartyGetMembers);
	interface->registerMethod("Party", "getMemberCount", luaPartyGetMemberCount);

	interface->registerMethod("Party", "getInvitees", luaPartyGetInvitees);
	interface->registerMethod("Party", "getInviteeCount", luaPartyGetInviteeCount);

	interface->registerMethod("Party", "addInvite", luaPartyAddInvite);
	interface->registerMethod("Party", "removeInvite", luaPartyRemoveInvite);

	interface->registerMethod("Party", "addMember", luaPartyAddMember);
	interface->registerMethod("Party", "removeMember", luaPartyRemoveMember);

	interface->registerMethod("Party", "isSharedExperienceActive", luaPartyIsSharedExperienceActive);
	interface->registerMethod("Party", "isSharedExperienceEnabled", luaPartyIsSharedExperienceEnabled);
	interface->registerMethod("Party", "shareExperience", luaPartyShareExperience);
	interface->registerMethod("Party", "setSharedExperience", luaPartySetSharedExperience);
}
} // namespace LuaParty

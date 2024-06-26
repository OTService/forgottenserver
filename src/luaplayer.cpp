// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "chat.h"
#include "creature.h"
#include "depotchest.h"
#include "game.h"
#include "inbox.h"
#include "iologindata.h"
#include "item.h"
#include "luascript.h"
#include "player.h"
#include "spectators.h"
#include "spells.h"
#include "storeinbox.h"
#include "vocation.h"

extern Game g_game;
extern Vocations g_vocations;
extern Chat* g_chat;
extern Spells* g_spells;

using namespace Lua;

// Player
static int luaPlayerCreate(lua_State* L)
{
	// Player(id or guid or name or userdata)
	Player* player;
	if (isNumber(L, 2)) {
		uint32_t id = getNumber<uint32_t>(L, 2);
		if (id >= CREATURE_ID_MIN && id <= Player::playerIDLimit) {
			player = g_game.getPlayerByID(id);
		} else {
			player = g_game.getPlayerByGUID(id);
		}
	} else if (isString(L, 2)) {
		ReturnValue ret = g_game.getPlayerByNameWildcard(getString(L, 2), player);
		if (ret != RETURNVALUE_NOERROR) {
			lua_pushnil(L);
			lua_pushnumber(L, ret);
			return 2;
		}
	} else if (isUserdata(L, 2)) {
		if (getUserdataType(L, 2) != LuaData_Player) {
			lua_pushnil(L);
			return 1;
		}
		player = getUserdata<Player>(L, 2);
	} else {
		player = nullptr;
	}

	if (player) {
		pushUserdata<Player>(L, player);
		setMetatable(L, -1, "Player");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerIsPlayer(lua_State* L)
{
	// player:isPlayer()
	pushBoolean(L, getUserdata<const Player>(L, 1) != nullptr);
	return 1;
}

static int luaPlayerGetGuid(lua_State* L)
{
	// player:getGuid()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getGUID());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetIp(lua_State* L)
{
	// player:getIp()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getIP());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetAccountId(lua_State* L)
{
	// player:getAccountId()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getAccount());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetLastLoginSaved(lua_State* L)
{
	// player:getLastLoginSaved()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getLastLoginSaved());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetLastLogout(lua_State* L)
{
	// player:getLastLogout()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getLastLogout());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetAccountType(lua_State* L)
{
	// player:getAccountType()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getAccountType());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetAccountType(lua_State* L)
{
	// player:setAccountType(accountType)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->accountType = getNumber<AccountType_t>(L, 2);
		IOLoginData::setAccountType(player->getAccount(), player->accountType);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetCapacity(lua_State* L)
{
	// player:getCapacity()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getCapacity());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetCapacity(lua_State* L)
{
	// player:setCapacity(capacity)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->capacity = getNumber<uint32_t>(L, 2);
		player->sendStats();
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetFreeCapacity(lua_State* L)
{
	// player:getFreeCapacity()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getFreeCapacity());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetDepotChest(lua_State* L)
{
	// player:getDepotChest(depotId[, autoCreate = false])
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	uint32_t depotId = getNumber<uint32_t>(L, 2);
	bool autoCreate = getBoolean(L, 3, false);
	DepotChest* depotChest = player->getDepotChest(depotId, autoCreate);
	if (depotChest) {
		pushUserdata<Item>(L, depotChest);
		setItemMetatable(L, -1, depotChest);
	} else {
		pushBoolean(L, false);
	}
	return 1;
}

static int luaPlayerGetInbox(lua_State* L)
{
	// player:getInbox()
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	Inbox* inbox = player->getInbox();
	if (inbox) {
		pushUserdata<Item>(L, inbox);
		setItemMetatable(L, -1, inbox);
	} else {
		pushBoolean(L, false);
	}
	return 1;
}

static int luaPlayerGetSkullTime(lua_State* L)
{
	// player:getSkullTime()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getSkullTicks());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetSkullTime(lua_State* L)
{
	// player:setSkullTime(skullTime)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->setSkullTicks(getNumber<int64_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetDeathPenalty(lua_State* L)
{
	// player:getDeathPenalty()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getLostPercent() * 100);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetExperience(lua_State* L)
{
	// player:getExperience()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getExperience());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddExperience(lua_State* L)
{
	// player:addExperience(experience[, sendText = false])
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		uint64_t experience = getNumber<uint64_t>(L, 2);
		bool sendText = getBoolean(L, 3, false);
		player->addExperience(nullptr, experience, sendText);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerRemoveExperience(lua_State* L)
{
	// player:removeExperience(experience[, sendText = false])
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		uint64_t experience = getNumber<uint64_t>(L, 2);
		bool sendText = getBoolean(L, 3, false);
		player->removeExperience(experience, sendText);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetLevel(lua_State* L)
{
	// player:getLevel()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getLevel());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetMagicLevel(lua_State* L)
{
	// player:getMagicLevel()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getMagicLevel());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetBaseMagicLevel(lua_State* L)
{
	// player:getBaseMagicLevel()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getBaseMagicLevel());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetMana(lua_State* L)
{
	// player:getMana()
	const Player* player = getUserdata<const Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getMana());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddMana(lua_State* L)
{
	// player:addMana(manaChange[, animationOnLoss = false])
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	int32_t manaChange = getNumber<int32_t>(L, 2);
	bool animationOnLoss = getBoolean(L, 3, false);
	if (!animationOnLoss && manaChange < 0) {
		player->changeMana(manaChange);
	} else {
		CombatDamage damage;
		damage.primary.value = manaChange;
		damage.origin = ORIGIN_NONE;
		g_game.combatChangeMana(nullptr, player, damage);
	}
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerGetMaxMana(lua_State* L)
{
	// player:getMaxMana()
	const Player* player = getUserdata<const Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getMaxMana());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetMaxMana(lua_State* L)
{
	// player:setMaxMana(maxMana)
	Player* player = getPlayer(L, 1);
	if (player) {
		player->manaMax = getNumber<int32_t>(L, 2);
		player->mana = std::min<int32_t>(player->mana, player->manaMax);
		player->sendStats();
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetManaSpent(lua_State* L)
{
	// player:getManaSpent()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getSpentMana());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddManaSpent(lua_State* L)
{
	// player:addManaSpent(amount)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->addManaSpent(getNumber<uint64_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerRemoveManaSpent(lua_State* L)
{
	// player:removeManaSpent(amount[, notify = true])
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->removeManaSpent(getNumber<uint64_t>(L, 2), getBoolean(L, 3, true));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetBaseMaxHealth(lua_State* L)
{
	// player:getBaseMaxHealth()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->healthMax);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetBaseMaxMana(lua_State* L)
{
	// player:getBaseMaxMana()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->manaMax);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetSkillLevel(lua_State* L)
{
	// player:getSkillLevel(skillType)
	skills_t skillType = getNumber<skills_t>(L, 2);
	Player* player = getUserdata<Player>(L, 1);
	if (player && skillType <= SKILL_LAST) {
		lua_pushnumber(L, player->skills[skillType].level);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetEffectiveSkillLevel(lua_State* L)
{
	// player:getEffectiveSkillLevel(skillType)
	skills_t skillType = getNumber<skills_t>(L, 2);
	Player* player = getUserdata<Player>(L, 1);
	if (player && skillType <= SKILL_LAST) {
		lua_pushnumber(L, player->getSkillLevel(skillType));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetSkillPercent(lua_State* L)
{
	// player:getSkillPercent(skillType)
	skills_t skillType = getNumber<skills_t>(L, 2);
	Player* player = getUserdata<Player>(L, 1);
	if (player && skillType <= SKILL_LAST) {
		lua_pushnumber(L, player->skills[skillType].percent);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetSkillTries(lua_State* L)
{
	// player:getSkillTries(skillType)
	skills_t skillType = getNumber<skills_t>(L, 2);
	Player* player = getUserdata<Player>(L, 1);
	if (player && skillType <= SKILL_LAST) {
		lua_pushnumber(L, player->skills[skillType].tries);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddSkillTries(lua_State* L)
{
	// player:addSkillTries(skillType, tries)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		skills_t skillType = getNumber<skills_t>(L, 2);
		uint64_t tries = getNumber<uint64_t>(L, 3);
		player->addSkillAdvance(skillType, tries);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerRemoveSkillTries(lua_State* L)
{
	// player:removeSkillTries(skillType, tries[, notify = true])
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		skills_t skillType = getNumber<skills_t>(L, 2);
		uint64_t tries = getNumber<uint64_t>(L, 3);
		player->removeSkillTries(skillType, tries, getBoolean(L, 4, true));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetSpecialSkill(lua_State* L)
{
	// player:getSpecialSkill(specialSkillType)
	SpecialSkills_t specialSkillType = getNumber<SpecialSkills_t>(L, 2);
	Player* player = getUserdata<Player>(L, 1);
	if (player && specialSkillType <= SPECIALSKILL_LAST) {
		lua_pushnumber(L, player->getSpecialSkill(specialSkillType));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddSpecialSkill(lua_State* L)
{
	// player:addSpecialSkill(specialSkillType, value)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	SpecialSkills_t specialSkillType = getNumber<SpecialSkills_t>(L, 2);
	if (specialSkillType > SPECIALSKILL_LAST) {
		lua_pushnil(L);
		return 1;
	}

	player->setVarSpecialSkill(specialSkillType, getNumber<int32_t>(L, 3));
	player->sendSkills();
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerAddOfflineTrainingTime(lua_State* L)
{
	// player:addOfflineTrainingTime(time)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		int32_t time = getNumber<int32_t>(L, 2);
		player->addOfflineTrainingTime(time);
		player->sendStats();
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetOfflineTrainingTime(lua_State* L)
{
	// player:getOfflineTrainingTime()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getOfflineTrainingTime());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerRemoveOfflineTrainingTime(lua_State* L)
{
	// player:removeOfflineTrainingTime(time)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		int32_t time = getNumber<int32_t>(L, 2);
		player->removeOfflineTrainingTime(time);
		player->sendStats();
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddOfflineTrainingTries(lua_State* L)
{
	// player:addOfflineTrainingTries(skillType, tries)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		skills_t skillType = getNumber<skills_t>(L, 2);
		uint64_t tries = getNumber<uint64_t>(L, 3);
		pushBoolean(L, player->addOfflineTrainingTries(skillType, tries));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetOfflineTrainingSkill(lua_State* L)
{
	// player:getOfflineTrainingSkill()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getOfflineTrainingSkill());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetOfflineTrainingSkill(lua_State* L)
{
	// player:setOfflineTrainingSkill(skillId)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		int32_t skillId = getNumber<int32_t>(L, 2);
		player->setOfflineTrainingSkill(skillId);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetItemCount(lua_State* L)
{
	// player:getItemCount(itemId[, subType = -1])
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	uint16_t itemId;
	if (isNumber(L, 2)) {
		itemId = getNumber<uint16_t>(L, 2);
	} else {
		itemId = Item::items.getItemIdByName(getString(L, 2));
		if (itemId == 0) {
			lua_pushnil(L);
			return 1;
		}
	}

	int32_t subType = getNumber<int32_t>(L, 3, -1);
	lua_pushnumber(L, player->getItemTypeCount(itemId, subType));
	return 1;
}

static int luaPlayerGetItemById(lua_State* L)
{
	// player:getItemById(itemId, deepSearch[, subType = -1])
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	uint16_t itemId;
	if (isNumber(L, 2)) {
		itemId = getNumber<uint16_t>(L, 2);
	} else {
		itemId = Item::items.getItemIdByName(getString(L, 2));
		if (itemId == 0) {
			lua_pushnil(L);
			return 1;
		}
	}
	bool deepSearch = getBoolean(L, 3);
	int32_t subType = getNumber<int32_t>(L, 4, -1);

	Item* item = g_game.findItemOfType(player, itemId, deepSearch, subType);
	if (item) {
		pushUserdata<Item>(L, item);
		setItemMetatable(L, -1, item);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetVocation(lua_State* L)
{
	// player:getVocation()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		pushUserdata<Vocation>(L, player->getVocation());
		setMetatable(L, -1, "Vocation");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetVocation(lua_State* L)
{
	// player:setVocation(id or name or userdata)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	Vocation* vocation;
	if (isNumber(L, 2)) {
		vocation = g_vocations.getVocation(getNumber<uint16_t>(L, 2));
	} else if (isString(L, 2)) {
		vocation = g_vocations.getVocation(g_vocations.getVocationId(getString(L, 2)));
	} else if (isUserdata(L, 2)) {
		vocation = getUserdata<Vocation>(L, 2);
	} else {
		vocation = nullptr;
	}

	if (!vocation) {
		pushBoolean(L, false);
		return 1;
	}

	player->setVocation(vocation->getId());
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerGetSex(lua_State* L)
{
	// player:getSex()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getSex());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetSex(lua_State* L)
{
	// player:setSex(newSex)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		PlayerSex_t newSex = getNumber<PlayerSex_t>(L, 2);
		player->setSex(newSex);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetTown(lua_State* L)
{
	// player:getTown()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		pushUserdata<Town>(L, player->getTown());
		setMetatable(L, -1, "Town");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetTown(lua_State* L)
{
	// player:setTown(town)
	Town* town = getUserdata<Town>(L, 2);
	if (!town) {
		pushBoolean(L, false);
		return 1;
	}

	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->setTown(town);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetGuild(lua_State* L)
{
	// player:getGuild()
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	Guild* guild = player->getGuild();
	if (!guild) {
		lua_pushnil(L);
		return 1;
	}

	pushUserdata<Guild>(L, guild);
	setMetatable(L, -1, "Guild");
	return 1;
}

static int luaPlayerSetGuild(lua_State* L)
{
	// player:setGuild(guild)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	player->setGuild(getUserdata<Guild>(L, 2));
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerGetGuildLevel(lua_State* L)
{
	// player:getGuildLevel()
	Player* player = getUserdata<Player>(L, 1);
	if (player && player->getGuild()) {
		lua_pushnumber(L, player->getGuildRank()->level);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetGuildLevel(lua_State* L)
{
	// player:setGuildLevel(level)
	uint8_t level = getNumber<uint8_t>(L, 2);
	Player* player = getUserdata<Player>(L, 1);
	if (!player || !player->getGuild()) {
		lua_pushnil(L);
		return 1;
	}

	GuildRank_ptr rank = player->getGuild()->getRankByLevel(level);
	if (!rank) {
		pushBoolean(L, false);
	} else {
		player->setGuildRank(rank);
		pushBoolean(L, true);
	}

	return 1;
}

static int luaPlayerGetGuildNick(lua_State* L)
{
	// player:getGuildNick()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		pushString(L, player->getGuildNick());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetGuildNick(lua_State* L)
{
	// player:setGuildNick(nick)
	const std::string& nick = getString(L, 2);
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->setGuildNick(nick);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetGroup(lua_State* L)
{
	// player:getGroup()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		pushUserdata<Group>(L, player->getGroup());
		setMetatable(L, -1, "Group");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetGroup(lua_State* L)
{
	// player:setGroup(group)
	Group* group = getUserdata<Group>(L, 2);
	if (!group) {
		pushBoolean(L, false);
		return 1;
	}

	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->setGroup(group);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetStamina(lua_State* L)
{
	// player:getStamina()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getStaminaMinutes());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetStamina(lua_State* L)
{
	// player:setStamina(stamina)
	uint16_t stamina = getNumber<uint16_t>(L, 2);
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->staminaMinutes = std::min<uint16_t>(2520, stamina);
		player->sendStats();
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetSoul(lua_State* L)
{
	// player:getSoul()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getSoul());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddSoul(lua_State* L)
{
	// player:addSoul(soulChange)
	int32_t soulChange = getNumber<int32_t>(L, 2);
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->changeSoul(soulChange);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetMaxSoul(lua_State* L)
{
	// player:getMaxSoul()
	Player* player = getUserdata<Player>(L, 1);
	if (player && player->vocation) {
		lua_pushnumber(L, player->vocation->getSoulMax());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetBankBalance(lua_State* L)
{
	// player:getBankBalance()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getBankBalance());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetBankBalance(lua_State* L)
{
	// player:setBankBalance(bankBalance)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	int64_t balance = getNumber<int64_t>(L, 2);
	if (balance < 0) {
		reportErrorFunc(L, "Invalid bank balance value.");
		lua_pushnil(L);
		return 1;
	}

	player->setBankBalance(balance);
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerGetStorageValue(lua_State* L)
{
	// player:getStorageValue(key)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	uint32_t key = getNumber<uint32_t>(L, 2);
	int32_t value;
	if (player->getStorageValue(key, value)) {
		lua_pushnumber(L, value);
	} else {
		lua_pushnumber(L, -1);
	}
	return 1;
}

static int luaPlayerSetStorageValue(lua_State* L)
{
	// player:setStorageValue(key, value)
	int32_t value = getNumber<int32_t>(L, 3);
	uint32_t key = getNumber<uint32_t>(L, 2);
	Player* player = getUserdata<Player>(L, 1);
	if (IS_IN_KEYRANGE(key, RESERVED_RANGE)) {
		reportErrorFunc(L, fmt::format("Accessing reserved range: {:d}", key));
		pushBoolean(L, false);
		return 1;
	}

	if (player) {
		player->addStorageValue(key, value);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddItem(lua_State* L)
{
	// player:addItem(itemId[, count = 1[, canDropOnMap = true[, subType = 1[, slot = CONST_SLOT_WHEREEVER]]]])
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		pushBoolean(L, false);
		return 1;
	}

	uint16_t itemId;
	if (isNumber(L, 2)) {
		itemId = getNumber<uint16_t>(L, 2);
	} else {
		itemId = Item::items.getItemIdByName(getString(L, 2));
		if (itemId == 0) {
			lua_pushnil(L);
			return 1;
		}
	}

	int32_t count = getNumber<int32_t>(L, 3, 1);
	int32_t subType = getNumber<int32_t>(L, 5, 1);

	const ItemType& it = Item::items[itemId];

	int32_t itemCount = 1;
	int parameters = lua_gettop(L);
	if (parameters >= 5) {
		itemCount = std::max<int32_t>(1, count);
	} else if (it.hasSubType()) {
		if (it.stackable) {
			itemCount = std::ceil(count / 100.f);
		}

		subType = count;
	} else {
		itemCount = std::max<int32_t>(1, count);
	}

	bool hasTable = itemCount > 1;
	if (hasTable) {
		lua_newtable(L);
	} else if (itemCount == 0) {
		lua_pushnil(L);
		return 1;
	}

	bool canDropOnMap = getBoolean(L, 4, true);
	slots_t slot = getNumber<slots_t>(L, 6, CONST_SLOT_WHEREEVER);
	for (int32_t i = 1; i <= itemCount; ++i) {
		int32_t stackCount = subType;
		if (it.stackable) {
			stackCount = std::min<int32_t>(stackCount, 100);
			subType -= stackCount;
		}

		Item* item = Item::CreateItem(itemId, stackCount);
		if (!item) {
			if (!hasTable) {
				lua_pushnil(L);
			}
			return 1;
		}

		ReturnValue ret = g_game.internalPlayerAddItem(player, item, canDropOnMap, slot);
		if (ret != RETURNVALUE_NOERROR) {
			delete item;
			if (!hasTable) {
				lua_pushnil(L);
			}
			return 1;
		}

		if (hasTable) {
			lua_pushnumber(L, i);
			pushUserdata<Item>(L, item);
			setItemMetatable(L, -1, item);
			lua_settable(L, -3);
		} else {
			pushUserdata<Item>(L, item);
			setItemMetatable(L, -1, item);
		}
	}
	return 1;
}

static int luaPlayerAddItemEx(lua_State* L)
{
	// player:addItemEx(item[, canDropOnMap = false[, index = INDEX_WHEREEVER[, flags = 0]]]) player:addItemEx(item[,
	// canDropOnMap = true[, slot = CONST_SLOT_WHEREEVER]])
	Item* item = getUserdata<Item>(L, 2);
	if (!item) {
		reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_ITEM_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	if (item->getParent() != VirtualCylinder::virtualCylinder) {
		reportErrorFunc(L, "Item already has a parent");
		pushBoolean(L, false);
		return 1;
	}

	bool canDropOnMap = getBoolean(L, 3, false);
	ReturnValue returnValue;
	if (canDropOnMap) {
		slots_t slot = getNumber<slots_t>(L, 4, CONST_SLOT_WHEREEVER);
		returnValue = g_game.internalPlayerAddItem(player, item, true, slot);
	} else {
		int32_t index = getNumber<int32_t>(L, 4, INDEX_WHEREEVER);
		uint32_t flags = getNumber<uint32_t>(L, 5, 0);
		returnValue = g_game.internalAddItem(player, item, index, flags);
	}

	if (returnValue == RETURNVALUE_NOERROR) {
		ScriptEnvironment::removeTempItem(item);
	}
	lua_pushnumber(L, returnValue);
	return 1;
}

static int luaPlayerRemoveItem(lua_State* L)
{
	// player:removeItem(itemId, count[, subType = -1[, ignoreEquipped = false]])
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	uint16_t itemId;
	if (isNumber(L, 2)) {
		itemId = getNumber<uint16_t>(L, 2);
	} else {
		itemId = Item::items.getItemIdByName(getString(L, 2));
		if (itemId == 0) {
			lua_pushnil(L);
			return 1;
		}
	}

	uint32_t count = getNumber<uint32_t>(L, 3);
	int32_t subType = getNumber<int32_t>(L, 4, -1);
	bool ignoreEquipped = getBoolean(L, 5, false);
	pushBoolean(L, player->removeItemOfType(itemId, count, subType, ignoreEquipped));
	return 1;
}

static int luaPlayerSendSupplyUsed(lua_State* L)
{
	// player:sendSupplyUsed(item)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_PLAYER_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	Item* item = getUserdata<Item>(L, 2);
	if (!item) {
		reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_ITEM_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	player->sendSupplyUsed(item->getClientID());
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerGetMoney(lua_State* L)
{
	// player:getMoney()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getMoney());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddMoney(lua_State* L)
{
	// player:addMoney(money)
	uint64_t money = getNumber<uint64_t>(L, 2);
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		g_game.addMoney(player, money);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerRemoveMoney(lua_State* L)
{
	// player:removeMoney(money)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		uint64_t money = getNumber<uint64_t>(L, 2);
		pushBoolean(L, g_game.removeMoney(player, money));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerShowTextDialog(lua_State* L)
{
	// player:showTextDialog(id or name or userdata[, text[, canWrite[, length]]])
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	int32_t length = getNumber<int32_t>(L, 5, -1);
	bool canWrite = getBoolean(L, 4, false);
	std::string text;

	int parameters = lua_gettop(L);
	if (parameters >= 3) {
		text = getString(L, 3);
	}

	Item* item;
	if (isNumber(L, 2)) {
		item = Item::CreateItem(getNumber<uint16_t>(L, 2));
	} else if (isString(L, 2)) {
		item = Item::CreateItem(Item::items.getItemIdByName(getString(L, 2)));
	} else if (isUserdata(L, 2)) {
		if (getUserdataType(L, 2) != LuaData_Item) {
			pushBoolean(L, false);
			return 1;
		}

		item = getUserdata<Item>(L, 2);
	} else {
		item = nullptr;
	}

	if (!item) {
		reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_ITEM_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	if (length < 0) {
		length = Item::items[item->getID()].maxTextLen;
	}

	if (!text.empty()) {
		item->setText(text);
		length = std::max<int32_t>(text.size(), length);
	}

	item->setParent(player);
	player->windowTextId++;
	player->writeItem = item;
	player->maxWriteLen = length;
	player->sendTextWindow(item, length, canWrite);
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerSendTextMessage(lua_State* L)
{
	// player:sendTextMessage(type, text[, position, primaryValue = 0, primaryColor = TEXTCOLOR_NONE[, secondaryValue =
	// 0, secondaryColor = TEXTCOLOR_NONE]]) player:sendTextMessage(type, text, channelId)

	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	int parameters = lua_gettop(L);

	TextMessage message(getNumber<MessageClasses>(L, 2), getString(L, 3));
	if (parameters == 4) {
		uint16_t channelId = getNumber<uint16_t>(L, 4);
		ChatChannel* channel = g_chat->getChannel(*player, channelId);
		if (!channel || !channel->hasUser(*player)) {
			pushBoolean(L, false);
			return 1;
		}
		message.channelId = channelId;
	} else {
		if (parameters >= 6) {
			message.position = getPosition(L, 4);
			message.primary.value = getNumber<int32_t>(L, 5);
			message.primary.color = getNumber<TextColor_t>(L, 6);
		}

		if (parameters >= 8) {
			message.secondary.value = getNumber<int32_t>(L, 7);
			message.secondary.color = getNumber<TextColor_t>(L, 8);
		}
	}

	player->sendTextMessage(message);
	pushBoolean(L, true);

	return 1;
}

static int luaPlayerSendChannelMessage(lua_State* L)
{
	// player:sendChannelMessage(author, text, type, channelId)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	uint16_t channelId = getNumber<uint16_t>(L, 5);
	SpeakClasses type = getNumber<SpeakClasses>(L, 4);
	const std::string& text = getString(L, 3);
	const std::string& author = getString(L, 2);
	player->sendChannelMessage(author, text, type, channelId);
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerSendPrivateMessage(lua_State* L)
{
	// player:sendPrivateMessage(speaker, text[, type])
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	const Player* speaker = getUserdata<const Player>(L, 2);
	const std::string& text = getString(L, 3);
	SpeakClasses type = getNumber<SpeakClasses>(L, 4, TALKTYPE_PRIVATE_FROM);
	player->sendPrivateMessage(speaker, type, text);
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerChannelSay(lua_State* L)
{
	// player:channelSay(speaker, type, text, channelId)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	Creature* speaker = getCreature(L, 2);
	SpeakClasses type = getNumber<SpeakClasses>(L, 3);
	const std::string& text = getString(L, 4);
	uint16_t channelId = getNumber<uint16_t>(L, 5);
	player->sendToChannel(speaker, type, text, channelId);
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerOpenChannel(lua_State* L)
{
	// player:openChannel(channelId)
	uint16_t channelId = getNumber<uint16_t>(L, 2);
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		g_game.playerOpenChannel(player->getID(), channelId);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetSlotItem(lua_State* L)
{
	// player:getSlotItem(slot)
	const Player* player = getUserdata<const Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	uint32_t slot = getNumber<uint32_t>(L, 2);
	Thing* thing = player->getThing(slot);
	if (!thing) {
		lua_pushnil(L);
		return 1;
	}

	Item* item = thing->getItem();
	if (item) {
		pushUserdata<Item>(L, item);
		setItemMetatable(L, -1, item);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetParty(lua_State* L)
{
	// player:getParty()
	const Player* player = getUserdata<const Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	Party* party = player->getParty();
	if (party) {
		pushUserdata<Party>(L, party);
		setMetatable(L, -1, "Party");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddOutfit(lua_State* L)
{
	// player:addOutfit(lookType)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->addOutfit(getNumber<uint16_t>(L, 2), 0);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddOutfitAddon(lua_State* L)
{
	// player:addOutfitAddon(lookType, addon)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		uint16_t lookType = getNumber<uint16_t>(L, 2);
		uint8_t addon = getNumber<uint8_t>(L, 3);
		player->addOutfit(lookType, addon);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerRemoveOutfit(lua_State* L)
{
	// player:removeOutfit(lookType)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		uint16_t lookType = getNumber<uint16_t>(L, 2);
		pushBoolean(L, player->removeOutfit(lookType));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerRemoveOutfitAddon(lua_State* L)
{
	// player:removeOutfitAddon(lookType, addon)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		uint16_t lookType = getNumber<uint16_t>(L, 2);
		uint8_t addon = getNumber<uint8_t>(L, 3);
		pushBoolean(L, player->removeOutfitAddon(lookType, addon));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerHasOutfit(lua_State* L)
{
	// player:hasOutfit(lookType[, addon = 0])
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		uint16_t lookType = getNumber<uint16_t>(L, 2);
		uint8_t addon = getNumber<uint8_t>(L, 3, 0);
		pushBoolean(L, player->hasOutfit(lookType, addon));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerCanWearOutfit(lua_State* L)
{
	// player:canWearOutfit(lookType[, addon = 0])
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		uint16_t lookType = getNumber<uint16_t>(L, 2);
		uint8_t addon = getNumber<uint8_t>(L, 3, 0);
		pushBoolean(L, player->canWear(lookType, addon));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSendOutfitWindow(lua_State* L)
{
	// player:sendOutfitWindow()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->sendOutfitWindow();
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSendEditPodium(lua_State* L)
{
	// player:sendEditPodium(item)
	Player* player = getUserdata<Player>(L, 1);
	Item* item = getUserdata<Item>(L, 2);
	if (player && item) {
		player->sendPodiumWindow(item);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddMount(lua_State* L)
{
	// player:addMount(mountId or mountName)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	uint8_t mountId;
	if (isNumber(L, 2)) {
		mountId = getNumber<uint8_t>(L, 2);
	} else {
		Mount* mount = g_game.mounts.getMountByName(getString(L, 2));
		if (!mount) {
			lua_pushnil(L);
			return 1;
		}
		mountId = mount->id;
	}
	pushBoolean(L, player->tameMount(mountId));
	return 1;
}

static int luaPlayerRemoveMount(lua_State* L)
{
	// player:removeMount(mountId or mountName)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	uint8_t mountId;
	if (isNumber(L, 2)) {
		mountId = getNumber<uint8_t>(L, 2);
	} else {
		Mount* mount = g_game.mounts.getMountByName(getString(L, 2));
		if (!mount) {
			lua_pushnil(L);
			return 1;
		}
		mountId = mount->id;
	}
	pushBoolean(L, player->untameMount(mountId));
	return 1;
}

static int luaPlayerHasMount(lua_State* L)
{
	// player:hasMount(mountId or mountName)
	const Player* player = getUserdata<const Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	Mount* mount = nullptr;
	if (isNumber(L, 2)) {
		mount = g_game.mounts.getMountByID(getNumber<uint8_t>(L, 2));
	} else {
		mount = g_game.mounts.getMountByName(getString(L, 2));
	}

	if (mount) {
		pushBoolean(L, player->hasMount(mount));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetPremiumEndsAt(lua_State* L)
{
	// player:getPremiumEndsAt()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->premiumEndsAt);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSetPremiumEndsAt(lua_State* L)
{
	// player:setPremiumEndsAt(timestamp)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	time_t timestamp = getNumber<time_t>(L, 2);

	player->setPremiumTime(timestamp);
	IOLoginData::updatePremiumTime(player->getAccount(), timestamp);
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerHasBlessing(lua_State* L)
{
	// player:hasBlessing(blessing)
	uint8_t blessing = getNumber<uint8_t>(L, 2) - 1;
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		pushBoolean(L, player->hasBlessing(blessing));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddBlessing(lua_State* L)
{
	// player:addBlessing(blessing)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	uint8_t blessing = getNumber<uint8_t>(L, 2) - 1;
	if (player->hasBlessing(blessing)) {
		pushBoolean(L, false);
		return 1;
	}

	player->addBlessing(blessing);
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerRemoveBlessing(lua_State* L)
{
	// player:removeBlessing(blessing)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	uint8_t blessing = getNumber<uint8_t>(L, 2) - 1;
	if (!player->hasBlessing(blessing)) {
		pushBoolean(L, false);
		return 1;
	}

	player->removeBlessing(blessing);
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerCanLearnSpell(lua_State* L)
{
	// player:canLearnSpell(spellName)
	const Player* player = getUserdata<const Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	const std::string& spellName = getString(L, 2);
	InstantSpell* spell = g_spells->getInstantSpellByName(spellName);
	if (!spell) {
		reportErrorFunc(L, "Spell \"" + spellName + "\" not found");
		pushBoolean(L, false);
		return 1;
	}

	if (player->hasFlag(PlayerFlag_IgnoreSpellCheck)) {
		pushBoolean(L, true);
		return 1;
	}

	const auto& vocMap = spell->getVocMap();
	if (vocMap.count(player->getVocationId()) == 0) {
		pushBoolean(L, false);
	} else if (player->getLevel() < spell->getLevel()) {
		pushBoolean(L, false);
	} else if (player->getMagicLevel() < spell->getMagicLevel()) {
		pushBoolean(L, false);
	} else {
		pushBoolean(L, true);
	}
	return 1;
}

static int luaPlayerLearnSpell(lua_State* L)
{
	// player:learnSpell(spellName)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		const std::string& spellName = getString(L, 2);
		player->learnInstantSpell(spellName);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerForgetSpell(lua_State* L)
{
	// player:forgetSpell(spellName)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		const std::string& spellName = getString(L, 2);
		player->forgetInstantSpell(spellName);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerHasLearnedSpell(lua_State* L)
{
	// player:hasLearnedSpell(spellName)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		const std::string& spellName = getString(L, 2);
		pushBoolean(L, player->hasLearnedInstantSpell(spellName));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSendTutorial(lua_State* L)
{
	// player:sendTutorial(tutorialId)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		uint8_t tutorialId = getNumber<uint8_t>(L, 2);
		player->sendTutorial(tutorialId);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerAddMapMark(lua_State* L)
{
	// player:addMapMark(position, type, description)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		const Position& position = getPosition(L, 2);
		uint8_t type = getNumber<uint8_t>(L, 3);
		const std::string& description = getString(L, 4);
		player->sendAddMarker(position, type, description);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSave(lua_State* L)
{
	// player:save()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		player->loginPosition = player->getPosition();
		pushBoolean(L, IOLoginData::savePlayer(player));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerPopupFYI(lua_State* L)
{
	// player:popupFYI(message)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		const std::string& message = getString(L, 2);
		player->sendFYIBox(message);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerIsPzLocked(lua_State* L)
{
	// player:isPzLocked()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		pushBoolean(L, player->isPzLocked());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetClient(lua_State* L)
{
	// player:getClient()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_createtable(L, 0, 2);
		setField(L, "version", player->getProtocolVersion());
		setField(L, "os", player->getOperatingSystem());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetHouse(lua_State* L)
{
	// player:getHouse()
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	House* house = g_game.map.houses.getHouseByPlayerId(player->getGUID());
	if (house) {
		pushUserdata<House>(L, house);
		setMetatable(L, -1, "House");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerSendHouseWindow(lua_State* L)
{
	// player:sendHouseWindow(house, listId)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	House* house = getUserdata<House>(L, 2);
	if (!house) {
		lua_pushnil(L);
		return 1;
	}

	uint32_t listId = getNumber<uint32_t>(L, 3);
	player->sendHouseWindow(house, listId);
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerSetEditHouse(lua_State* L)
{
	// player:setEditHouse(house, listId)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	House* house = getUserdata<House>(L, 2);
	if (!house) {
		lua_pushnil(L);
		return 1;
	}

	uint32_t listId = getNumber<uint32_t>(L, 3);
	player->setEditHouse(house, listId);
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerSetGhostMode(lua_State* L)
{
	// player:setGhostMode(enabled[, magicEffect = CONST_ME_TELEPORT])
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	bool enabled = getBoolean(L, 2);
	if (player->isInGhostMode() == enabled) {
		pushBoolean(L, true);
		return 1;
	}

	MagicEffectClasses magicEffect = getNumber<MagicEffectClasses>(L, 3, CONST_ME_TELEPORT);

	player->switchGhostMode();

	Tile* tile = player->getTile();
	const Position& position = player->getPosition();
	const bool isInvisible = player->isInvisible();

	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, position, true, true);
	for (Creature* spectator : spectators) {
		Player* tmpPlayer = spectator->getPlayer();
		if (tmpPlayer != player && !tmpPlayer->isAccessPlayer()) {
			if (enabled) {
				tmpPlayer->sendRemoveTileCreature(player, position, tile->getClientIndexOfCreature(tmpPlayer, player));
			} else {
				tmpPlayer->sendCreatureAppear(player, position, magicEffect);
			}
		} else {
			if (isInvisible) {
				continue;
			}

			tmpPlayer->sendCreatureChangeVisible(player, !enabled);
		}
	}

	if (player->isInGhostMode()) {
		for (const auto& it : g_game.getPlayers()) {
			if (!it.second->isAccessPlayer()) {
				it.second->notifyStatusChange(player, VIPSTATUS_OFFLINE);
			}
		}
		IOLoginData::updateOnlineStatus(player->getGUID(), false);
	} else {
		for (const auto& it : g_game.getPlayers()) {
			if (!it.second->isAccessPlayer()) {
				it.second->notifyStatusChange(player, VIPSTATUS_ONLINE);
			}
		}
		IOLoginData::updateOnlineStatus(player->getGUID(), true);
	}
	pushBoolean(L, true);
	return 1;
}

static int luaPlayerGetContainerId(lua_State* L)
{
	// player:getContainerId(container)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	Container* container = getUserdata<Container>(L, 2);
	if (container) {
		lua_pushnumber(L, player->getContainerID(container));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetContainerById(lua_State* L)
{
	// player:getContainerById(id)
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	Container* container = player->getContainerByID(getNumber<uint8_t>(L, 2));
	if (container) {
		pushUserdata<Container>(L, container);
		setMetatable(L, -1, "Container");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetContainerIndex(lua_State* L)
{
	// player:getContainerIndex(id)
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->getContainerIndex(getNumber<uint8_t>(L, 2)));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetInstantSpells(lua_State* L)
{
	// player:getInstantSpells()
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	std::vector<const InstantSpell*> spells;
	for (auto& spell : g_spells->getInstantSpells()) {
		if (spell.second.canCast(player)) {
			spells.push_back(&spell.second);
		}
	}

	lua_createtable(L, spells.size(), 0);

	int index = 0;
	for (auto spell : spells) {
		pushInstantSpell(L, *spell);
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaPlayerCanCast(lua_State* L)
{
	// player:canCast(spell)
	Player* player = getUserdata<Player>(L, 1);
	InstantSpell* spell = getUserdata<InstantSpell>(L, 2);
	if (player && spell) {
		pushBoolean(L, spell->canCast(player));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerHasChaseMode(lua_State* L)
{
	// player:hasChaseMode()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		pushBoolean(L, player->chaseMode);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerHasSecureMode(lua_State* L)
{
	// player:hasSecureMode()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		pushBoolean(L, player->secureMode);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetFightMode(lua_State* L)
{
	// player:getFightMode()
	Player* player = getUserdata<Player>(L, 1);
	if (player) {
		lua_pushnumber(L, player->fightMode);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaPlayerGetStoreInbox(lua_State* L)
{
	// player:getStoreInbox()
	Player* player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	Container* storeInbox = player->getStoreInbox();
	if (!storeInbox) {
		lua_pushnil(L);
		return 1;
	}

	pushUserdata<Container>(L, storeInbox);
	setMetatable(L, -1, "Container");
	return 1;
}

static int luaPlayerIsNearDepotBox(lua_State* L)
{
	// player:isNearDepotBox()
	const Player* const player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	pushBoolean(L, player->isNearDepotBox());
	return 1;
}

static int luaPlayerGetIdleTime(lua_State* L)
{
	// player:getIdleTime()
	const Player* const player = getUserdata<Player>(L, 1);
	if (!player) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushnumber(L, player->getIdleTime());
	return 1;
}

namespace LuaPlayer {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("Player", "Creature", luaPlayerCreate);
	interface->registerMetaMethod("Player", "__eq", interface->luaUserdataCompare);

	interface->registerMethod("Player", "isPlayer", luaPlayerIsPlayer);

	interface->registerMethod("Player", "getGuid", luaPlayerGetGuid);
	interface->registerMethod("Player", "getIp", luaPlayerGetIp);
	interface->registerMethod("Player", "getAccountId", luaPlayerGetAccountId);
	interface->registerMethod("Player", "getLastLoginSaved", luaPlayerGetLastLoginSaved);
	interface->registerMethod("Player", "getLastLogout", luaPlayerGetLastLogout);

	interface->registerMethod("Player", "getAccountType", luaPlayerGetAccountType);
	interface->registerMethod("Player", "setAccountType", luaPlayerSetAccountType);

	interface->registerMethod("Player", "getCapacity", luaPlayerGetCapacity);
	interface->registerMethod("Player", "setCapacity", luaPlayerSetCapacity);

	interface->registerMethod("Player", "getFreeCapacity", luaPlayerGetFreeCapacity);

	interface->registerMethod("Player", "getDepotChest", luaPlayerGetDepotChest);
	interface->registerMethod("Player", "getInbox", luaPlayerGetInbox);

	interface->registerMethod("Player", "getSkullTime", luaPlayerGetSkullTime);
	interface->registerMethod("Player", "setSkullTime", luaPlayerSetSkullTime);
	interface->registerMethod("Player", "getDeathPenalty", luaPlayerGetDeathPenalty);

	interface->registerMethod("Player", "getExperience", luaPlayerGetExperience);
	interface->registerMethod("Player", "addExperience", luaPlayerAddExperience);
	interface->registerMethod("Player", "removeExperience", luaPlayerRemoveExperience);
	interface->registerMethod("Player", "getLevel", luaPlayerGetLevel);

	interface->registerMethod("Player", "getMagicLevel", luaPlayerGetMagicLevel);
	interface->registerMethod("Player", "getBaseMagicLevel", luaPlayerGetBaseMagicLevel);
	interface->registerMethod("Player", "getMana", luaPlayerGetMana);
	interface->registerMethod("Player", "addMana", luaPlayerAddMana);
	interface->registerMethod("Player", "getMaxMana", luaPlayerGetMaxMana);
	interface->registerMethod("Player", "setMaxMana", luaPlayerSetMaxMana);
	interface->registerMethod("Player", "getManaSpent", luaPlayerGetManaSpent);
	interface->registerMethod("Player", "addManaSpent", luaPlayerAddManaSpent);
	interface->registerMethod("Player", "removeManaSpent", luaPlayerRemoveManaSpent);

	interface->registerMethod("Player", "getBaseMaxHealth", luaPlayerGetBaseMaxHealth);
	interface->registerMethod("Player", "getBaseMaxMana", luaPlayerGetBaseMaxMana);

	interface->registerMethod("Player", "getSkillLevel", luaPlayerGetSkillLevel);
	interface->registerMethod("Player", "getEffectiveSkillLevel", luaPlayerGetEffectiveSkillLevel);
	interface->registerMethod("Player", "getSkillPercent", luaPlayerGetSkillPercent);
	interface->registerMethod("Player", "getSkillTries", luaPlayerGetSkillTries);
	interface->registerMethod("Player", "addSkillTries", luaPlayerAddSkillTries);
	interface->registerMethod("Player", "removeSkillTries", luaPlayerRemoveSkillTries);
	interface->registerMethod("Player", "getSpecialSkill", luaPlayerGetSpecialSkill);
	interface->registerMethod("Player", "addSpecialSkill", luaPlayerAddSpecialSkill);

	interface->registerMethod("Player", "addOfflineTrainingTime", luaPlayerAddOfflineTrainingTime);
	interface->registerMethod("Player", "getOfflineTrainingTime", luaPlayerGetOfflineTrainingTime);
	interface->registerMethod("Player", "removeOfflineTrainingTime", luaPlayerRemoveOfflineTrainingTime);

	interface->registerMethod("Player", "addOfflineTrainingTries", luaPlayerAddOfflineTrainingTries);

	interface->registerMethod("Player", "getOfflineTrainingSkill", luaPlayerGetOfflineTrainingSkill);
	interface->registerMethod("Player", "setOfflineTrainingSkill", luaPlayerSetOfflineTrainingSkill);

	interface->registerMethod("Player", "getItemCount", luaPlayerGetItemCount);
	interface->registerMethod("Player", "getItemById", luaPlayerGetItemById);

	interface->registerMethod("Player", "getVocation", luaPlayerGetVocation);
	interface->registerMethod("Player", "setVocation", luaPlayerSetVocation);

	interface->registerMethod("Player", "getSex", luaPlayerGetSex);
	interface->registerMethod("Player", "setSex", luaPlayerSetSex);

	interface->registerMethod("Player", "getTown", luaPlayerGetTown);
	interface->registerMethod("Player", "setTown", luaPlayerSetTown);

	interface->registerMethod("Player", "getGuild", luaPlayerGetGuild);
	interface->registerMethod("Player", "setGuild", luaPlayerSetGuild);

	interface->registerMethod("Player", "getGuildLevel", luaPlayerGetGuildLevel);
	interface->registerMethod("Player", "setGuildLevel", luaPlayerSetGuildLevel);

	interface->registerMethod("Player", "getGuildNick", luaPlayerGetGuildNick);
	interface->registerMethod("Player", "setGuildNick", luaPlayerSetGuildNick);

	interface->registerMethod("Player", "getGroup", luaPlayerGetGroup);
	interface->registerMethod("Player", "setGroup", luaPlayerSetGroup);

	interface->registerMethod("Player", "getStamina", luaPlayerGetStamina);
	interface->registerMethod("Player", "setStamina", luaPlayerSetStamina);

	interface->registerMethod("Player", "getSoul", luaPlayerGetSoul);
	interface->registerMethod("Player", "addSoul", luaPlayerAddSoul);
	interface->registerMethod("Player", "getMaxSoul", luaPlayerGetMaxSoul);

	interface->registerMethod("Player", "getBankBalance", luaPlayerGetBankBalance);
	interface->registerMethod("Player", "setBankBalance", luaPlayerSetBankBalance);

	interface->registerMethod("Player", "getStorageValue", luaPlayerGetStorageValue);
	interface->registerMethod("Player", "setStorageValue", luaPlayerSetStorageValue);

	interface->registerMethod("Player", "addItem", luaPlayerAddItem);
	interface->registerMethod("Player", "addItemEx", luaPlayerAddItemEx);
	interface->registerMethod("Player", "removeItem", luaPlayerRemoveItem);
	interface->registerMethod("Player", "sendSupplyUsed", luaPlayerSendSupplyUsed);

	interface->registerMethod("Player", "getMoney", luaPlayerGetMoney);
	interface->registerMethod("Player", "addMoney", luaPlayerAddMoney);
	interface->registerMethod("Player", "removeMoney", luaPlayerRemoveMoney);

	interface->registerMethod("Player", "showTextDialog", luaPlayerShowTextDialog);

	interface->registerMethod("Player", "sendTextMessage", luaPlayerSendTextMessage);
	interface->registerMethod("Player", "sendChannelMessage", luaPlayerSendChannelMessage);
	interface->registerMethod("Player", "sendPrivateMessage", luaPlayerSendPrivateMessage);
	interface->registerMethod("Player", "channelSay", luaPlayerChannelSay);
	interface->registerMethod("Player", "openChannel", luaPlayerOpenChannel);

	interface->registerMethod("Player", "getSlotItem", luaPlayerGetSlotItem);

	interface->registerMethod("Player", "getParty", luaPlayerGetParty);

	interface->registerMethod("Player", "addOutfit", luaPlayerAddOutfit);
	interface->registerMethod("Player", "addOutfitAddon", luaPlayerAddOutfitAddon);
	interface->registerMethod("Player", "removeOutfit", luaPlayerRemoveOutfit);
	interface->registerMethod("Player", "removeOutfitAddon", luaPlayerRemoveOutfitAddon);
	interface->registerMethod("Player", "hasOutfit", luaPlayerHasOutfit);
	interface->registerMethod("Player", "canWearOutfit", luaPlayerCanWearOutfit);
	interface->registerMethod("Player", "sendOutfitWindow", luaPlayerSendOutfitWindow);

	interface->registerMethod("Player", "sendEditPodium", luaPlayerSendEditPodium);

	interface->registerMethod("Player", "addMount", luaPlayerAddMount);
	interface->registerMethod("Player", "removeMount", luaPlayerRemoveMount);
	interface->registerMethod("Player", "hasMount", luaPlayerHasMount);

	interface->registerMethod("Player", "getPremiumEndsAt", luaPlayerGetPremiumEndsAt);
	interface->registerMethod("Player", "setPremiumEndsAt", luaPlayerSetPremiumEndsAt);

	interface->registerMethod("Player", "hasBlessing", luaPlayerHasBlessing);
	interface->registerMethod("Player", "addBlessing", luaPlayerAddBlessing);
	interface->registerMethod("Player", "removeBlessing", luaPlayerRemoveBlessing);

	interface->registerMethod("Player", "canLearnSpell", luaPlayerCanLearnSpell);
	interface->registerMethod("Player", "learnSpell", luaPlayerLearnSpell);
	interface->registerMethod("Player", "forgetSpell", luaPlayerForgetSpell);
	interface->registerMethod("Player", "hasLearnedSpell", luaPlayerHasLearnedSpell);

	interface->registerMethod("Player", "sendTutorial", luaPlayerSendTutorial);
	interface->registerMethod("Player", "addMapMark", luaPlayerAddMapMark);

	interface->registerMethod("Player", "save", luaPlayerSave);
	interface->registerMethod("Player", "popupFYI", luaPlayerPopupFYI);

	interface->registerMethod("Player", "isPzLocked", luaPlayerIsPzLocked);

	interface->registerMethod("Player", "getClient", luaPlayerGetClient);

	interface->registerMethod("Player", "getHouse", luaPlayerGetHouse);
	interface->registerMethod("Player", "sendHouseWindow", luaPlayerSendHouseWindow);
	interface->registerMethod("Player", "setEditHouse", luaPlayerSetEditHouse);

	interface->registerMethod("Player", "setGhostMode", luaPlayerSetGhostMode);

	interface->registerMethod("Player", "getContainerId", luaPlayerGetContainerId);
	interface->registerMethod("Player", "getContainerById", luaPlayerGetContainerById);
	interface->registerMethod("Player", "getContainerIndex", luaPlayerGetContainerIndex);

	interface->registerMethod("Player", "getInstantSpells", luaPlayerGetInstantSpells);
	interface->registerMethod("Player", "canCast", luaPlayerCanCast);

	interface->registerMethod("Player", "hasChaseMode", luaPlayerHasChaseMode);
	interface->registerMethod("Player", "hasSecureMode", luaPlayerHasSecureMode);
	interface->registerMethod("Player", "getFightMode", luaPlayerGetFightMode);

	interface->registerMethod("Player", "getStoreInbox", luaPlayerGetStoreInbox);

	interface->registerMethod("Player", "isNearDepotBox", luaPlayerIsNearDepotBox);

	interface->registerMethod("Player", "getIdleTime", luaPlayerGetIdleTime);
}
} // namespace LuaPlayer

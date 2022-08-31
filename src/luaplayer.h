// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LUAPLAYER_H
#define FS_LUAPLAYER_H

#include "otpch.h"

#include "luascript.h"
#include "player.h"

class Creature;
class Player;

class LuaPlayer : public LuaScriptInterface
{
public:
	static int luaPlayerCreate(lua_State* L);

	static int luaPlayerIsPlayer(lua_State* L);

	static int luaPlayerGetGuid(lua_State* L);
	static int luaPlayerGetIp(lua_State* L);
	static int luaPlayerGetAccountId(lua_State* L);
	static int luaPlayerGetLastLoginSaved(lua_State* L);
	static int luaPlayerGetLastLogout(lua_State* L);

	static int luaPlayerGetAccountType(lua_State* L);
	static int luaPlayerSetAccountType(lua_State* L);

	static int luaPlayerGetCapacity(lua_State* L);
	static int luaPlayerSetCapacity(lua_State* L);

	static int luaPlayerGetFreeCapacity(lua_State* L);

	static int luaPlayerGetDepotChest(lua_State* L);
	static int luaPlayerGetInbox(lua_State* L);

	static int luaPlayerGetSkullTime(lua_State* L);
	static int luaPlayerSetSkullTime(lua_State* L);
	static int luaPlayerGetDeathPenalty(lua_State* L);

	static int luaPlayerGetExperience(lua_State* L);
	static int luaPlayerAddExperience(lua_State* L);
	static int luaPlayerRemoveExperience(lua_State* L);
	static int luaPlayerGetLevel(lua_State* L);

	static int luaPlayerGetMagicLevel(lua_State* L);
	static int luaPlayerGetBaseMagicLevel(lua_State* L);
	static int luaPlayerGetMana(lua_State* L);
	static int luaPlayerAddMana(lua_State* L);
	static int luaPlayerGetMaxMana(lua_State* L);
	static int luaPlayerSetMaxMana(lua_State* L);
	static int luaPlayerGetManaSpent(lua_State* L);
	static int luaPlayerAddManaSpent(lua_State* L);
	static int luaPlayerRemoveManaSpent(lua_State* L);

	static int luaPlayerGetBaseMaxHealth(lua_State* L);
	static int luaPlayerGetBaseMaxMana(lua_State* L);

	static int luaPlayerGetSkillLevel(lua_State* L);
	static int luaPlayerGetEffectiveSkillLevel(lua_State* L);
	static int luaPlayerGetSkillPercent(lua_State* L);
	static int luaPlayerGetSkillTries(lua_State* L);
	static int luaPlayerAddSkillTries(lua_State* L);
	static int luaPlayerRemoveSkillTries(lua_State* L);
	static int luaPlayerGetSpecialSkill(lua_State* L);
	static int luaPlayerAddSpecialSkill(lua_State* L);

	static int luaPlayerAddOfflineTrainingTime(lua_State* L);
	static int luaPlayerGetOfflineTrainingTime(lua_State* L);
	static int luaPlayerRemoveOfflineTrainingTime(lua_State* L);

	static int luaPlayerAddOfflineTrainingTries(lua_State* L);

	static int luaPlayerGetOfflineTrainingSkill(lua_State* L);
	static int luaPlayerSetOfflineTrainingSkill(lua_State* L);

	static int luaPlayerGetItemCount(lua_State* L);
	static int luaPlayerGetItemById(lua_State* L);

	static int luaPlayerGetVocation(lua_State* L);
	static int luaPlayerSetVocation(lua_State* L);

	static int luaPlayerGetSex(lua_State* L);
	static int luaPlayerSetSex(lua_State* L);

	static int luaPlayerGetTown(lua_State* L);
	static int luaPlayerSetTown(lua_State* L);

	static int luaPlayerGetGuild(lua_State* L);
	static int luaPlayerSetGuild(lua_State* L);

	static int luaPlayerGetGuildLevel(lua_State* L);
	static int luaPlayerSetGuildLevel(lua_State* L);

	static int luaPlayerGetGuildNick(lua_State* L);
	static int luaPlayerSetGuildNick(lua_State* L);

	static int luaPlayerGetGroup(lua_State* L);
	static int luaPlayerSetGroup(lua_State* L);

	static int luaPlayerGetStamina(lua_State* L);
	static int luaPlayerSetStamina(lua_State* L);

	static int luaPlayerGetSoul(lua_State* L);
	static int luaPlayerAddSoul(lua_State* L);
	static int luaPlayerGetMaxSoul(lua_State* L);

	static int luaPlayerGetBankBalance(lua_State* L);
	static int luaPlayerSetBankBalance(lua_State* L);

	static int luaPlayerGetStorageValue(lua_State* L);
	static int luaPlayerSetStorageValue(lua_State* L);

	static int luaPlayerAddItem(lua_State* L);
	static int luaPlayerAddItemEx(lua_State* L);
	static int luaPlayerRemoveItem(lua_State* L);
	static int luaPlayerSendSupplyUsed(lua_State* L);

	static int luaPlayerGetMoney(lua_State* L);
	static int luaPlayerAddMoney(lua_State* L);
	static int luaPlayerRemoveMoney(lua_State* L);

	static int luaPlayerShowTextDialog(lua_State* L);

	static int luaPlayerSendTextMessage(lua_State* L);
	static int luaPlayerSendChannelMessage(lua_State* L);
	static int luaPlayerSendPrivateMessage(lua_State* L);

	static int luaPlayerChannelSay(lua_State* L);
	static int luaPlayerOpenChannel(lua_State* L);

	static int luaPlayerGetSlotItem(lua_State* L);

	static int luaPlayerGetParty(lua_State* L);

	static int luaPlayerAddOutfit(lua_State* L);
	static int luaPlayerAddOutfitAddon(lua_State* L);
	static int luaPlayerRemoveOutfit(lua_State* L);
	static int luaPlayerRemoveOutfitAddon(lua_State* L);
	static int luaPlayerHasOutfit(lua_State* L);
	static int luaPlayerCanWearOutfit(lua_State* L);
	static int luaPlayerSendOutfitWindow(lua_State* L);

	static int luaPlayerSendEditPodium(lua_State* L);

	static int luaPlayerAddMount(lua_State* L);
	static int luaPlayerRemoveMount(lua_State* L);
	static int luaPlayerHasMount(lua_State* L);

	static int luaPlayerGetPremiumEndsAt(lua_State* L);
	static int luaPlayerSetPremiumEndsAt(lua_State* L);

	static int luaPlayerHasBlessing(lua_State* L);
	static int luaPlayerAddBlessing(lua_State* L);
	static int luaPlayerRemoveBlessing(lua_State* L);

	static int luaPlayerCanLearnSpell(lua_State* L);
	static int luaPlayerLearnSpell(lua_State* L);
	static int luaPlayerForgetSpell(lua_State* L);
	static int luaPlayerHasLearnedSpell(lua_State* L);

	static int luaPlayerSendTutorial(lua_State* L);
	static int luaPlayerAddMapMark(lua_State* L);

	static int luaPlayerSave(lua_State* L);
	static int luaPlayerPopupFYI(lua_State* L);

	static int luaPlayerIsPzLocked(lua_State* L);

	static int luaPlayerGetClient(lua_State* L);

	static int luaPlayerGetHouse(lua_State* L);
	static int luaPlayerSendHouseWindow(lua_State* L);
	static int luaPlayerSetEditHouse(lua_State* L);

	static int luaPlayerSetGhostMode(lua_State* L);

	static int luaPlayerGetContainerId(lua_State* L);
	static int luaPlayerGetContainerById(lua_State* L);
	static int luaPlayerGetContainerIndex(lua_State* L);

	static int luaPlayerGetInstantSpells(lua_State* L);
	static int luaPlayerCanCast(lua_State* L);

	static int luaPlayerHasChaseMode(lua_State* L);
	static int luaPlayerHasSecureMode(lua_State* L);
	static int luaPlayerGetFightMode(lua_State* L);

	static int luaPlayerGetStoreInbox(lua_State* L);

	static int luaPlayerIsNearDepotBox(lua_State* L);

	static int luaPlayerGetIdleTime(lua_State* L);

protected:
private:
	friend class LuaScriptInterface;
};

#endif // FS_LUAPLAYER_H

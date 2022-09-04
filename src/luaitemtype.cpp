// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "condition.h"
#include "iomarket.h"
#include "item.h"
#include "items.h"
#include "luascript.h"
#include "tile.h"
#include "tools.h"

using namespace Lua;

const std::unordered_map<std::string, ItemTypes_t> ItemTypesMap = {
    {"key", ITEM_TYPE_KEY},
    {"magicfield", ITEM_TYPE_MAGICFIELD},
    {"container", ITEM_TYPE_CONTAINER},
    {"depot", ITEM_TYPE_DEPOT},
    {"mailbox", ITEM_TYPE_MAILBOX},
    {"trashholder", ITEM_TYPE_TRASHHOLDER},
    {"teleport", ITEM_TYPE_TELEPORT},
    {"door", ITEM_TYPE_DOOR},
    {"bed", ITEM_TYPE_BED},
    {"rune", ITEM_TYPE_RUNE},
    {"podium", ITEM_TYPE_PODIUM},
};

const std::unordered_map<std::string, itemgroup_t> ItemGroupMap = {
    {"ground", ITEM_GROUP_GROUND},     {"container", ITEM_GROUP_CONTAINER},   {"weapon", ITEM_GROUP_WEAPON},
    {"ammo", ITEM_GROUP_AMMUNITION},   {"armor", ITEM_GROUP_ARMOR},           {"charges", ITEM_GROUP_CHARGES},
    {"teleport", ITEM_GROUP_TELEPORT}, {"magicfield", ITEM_GROUP_MAGICFIELD}, {"writeable", ITEM_GROUP_WRITEABLE},
    {"key", ITEM_GROUP_KEY},           {"splash", ITEM_GROUP_SPLASH},         {"fluid", ITEM_GROUP_FLUID},
    {"door", ITEM_GROUP_DOOR},         {"deprecated", ITEM_GROUP_DEPRECATED}, {"podium", ITEM_GROUP_PODIUM},
};

const std::unordered_map<std::string, tileflags_t> TileStatesMap = {
    {"down", TILESTATE_FLOORCHANGE_DOWN},        {"north", TILESTATE_FLOORCHANGE_NORTH},
    {"south", TILESTATE_FLOORCHANGE_SOUTH},      {"southalt", TILESTATE_FLOORCHANGE_SOUTH_ALT},
    {"west", TILESTATE_FLOORCHANGE_WEST},        {"east", TILESTATE_FLOORCHANGE_EAST},
    {"eastalt", TILESTATE_FLOORCHANGE_EAST_ALT},
};

const std::unordered_map<std::string, RaceType_t> RaceTypesMap = {
    {"venom", RACE_VENOM}, {"blood", RACE_BLOOD}, {"undead", RACE_UNDEAD}, {"fire", RACE_FIRE}, {"energy", RACE_ENERGY},
};

const std::unordered_map<std::string, WeaponType_t> WeaponTypesMap = {
    {"sword", WEAPON_SWORD},       {"club", WEAPON_CLUB}, {"axe", WEAPON_AXE},         {"shield", WEAPON_SHIELD},
    {"distance", WEAPON_DISTANCE}, {"wand", WEAPON_WAND}, {"ammunition", WEAPON_AMMO}, {"quiver", WEAPON_QUIVER},
};

const std::unordered_map<std::string, FluidTypes_t> FluidTypesMap = {
    {"water", FLUID_WATER},
    {"blood", FLUID_BLOOD},
    {"beer", FLUID_BEER},
    {"slime", FLUID_SLIME},
    {"lemonade", FLUID_LEMONADE},
    {"milk", FLUID_MILK},
    {"mana", FLUID_MANA},
    {"life", FLUID_LIFE},
    {"oil", FLUID_OIL},
    {"urine", FLUID_URINE},
    {"coconut", FLUID_COCONUTMILK},
    {"wine", FLUID_WINE},
    {"mud", FLUID_MUD},
    {"fruitjuice", FLUID_FRUITJUICE},
    {"lava", FLUID_LAVA},
    {"rum", FLUID_RUM},
    {"swamp", FLUID_SWAMP},
    {"tea", FLUID_TEA},
    {"mead", FLUID_MEAD},
};

const std::unordered_map<std::string, skills_t> AbilitieSkillMap = {
    {"fist", SKILL_FIST},          {"club", SKILL_CLUB},
    {"sword", SKILL_SWORD},        {"axe", SKILL_AXE},
    {"distance", SKILL_DISTANCE},  {"shield", SKILL_SHIELD},
    {"fishing", SKILL_FISHING},    {"skillfist", SKILL_FIST},
    {"skillclub", SKILL_CLUB},     {"skillsword", SKILL_SWORD},
    {"skillaxe", SKILL_AXE},       {"skilldist", SKILL_DISTANCE},
    {"skillshield", SKILL_SHIELD}, {"skillfishing", SKILL_FISHING},
};

const std::unordered_map<std::string, stats_t> AbilitieStatMap = {
    {"maxhitpoints", STAT_MAXHITPOINTS}, {"maxmanapoints", STAT_MAXMANAPOINTS},  {"soulpoints", STAT_SOULPOINTS},
    {"magicpoints", STAT_MAGICPOINTS},   {"magiclevelpoints", STAT_MAGICPOINTS},
};

const std::unordered_map<std::string, stats_t> AbilitieStatPercentMap = {
    {"maxhitpointspercent", STAT_MAXHITPOINTS},
    {"maxmanapointspercent", STAT_MAXMANAPOINTS},
    {"magicpointspercent", STAT_MAGICPOINTS},
};

const std::unordered_map<std::string, CombatType_t> AbilitieAbsorbPercentMap = {
    {"absorbpercentphysical", COMBAT_PHYSICALDAMAGE},   {"absorbpercentenergy", COMBAT_ENERGYDAMAGE},
    {"absorbpercentearth", COMBAT_EARTHDAMAGE},         {"absorbpercentfire", COMBAT_FIREDAMAGE},
    {"absorbpercentundefined", COMBAT_UNDEFINEDDAMAGE}, {"absorbpercentlifedrain", COMBAT_LIFEDRAIN},
    {"absorbpercentmanadrain", COMBAT_MANADRAIN},       {"absorbpercenthealing", COMBAT_HEALING},
    {"absorbpercentdrown", COMBAT_DROWNDAMAGE},         {"absorbpercentice", COMBAT_ICEDAMAGE},
    {"absorbpercentholy", COMBAT_HOLYDAMAGE},           {"absorbpercentdeath", COMBAT_DEATHDAMAGE},
};

const std::unordered_map<std::string, CombatType_t> AbilitieFieldAbsorbPercentMap = {
    {"fieldabsorbpercentphysical", COMBAT_PHYSICALDAMAGE},   {"fieldabsorbpercentenergy", COMBAT_ENERGYDAMAGE},
    {"fieldabsorbpercentearth", COMBAT_EARTHDAMAGE},         {"fieldabsorbpercentfire", COMBAT_FIREDAMAGE},
    {"fieldabsorbpercentundefined", COMBAT_UNDEFINEDDAMAGE}, {"fieldabsorbpercentlifedrain", COMBAT_LIFEDRAIN},
    {"fieldabsorbpercentmanadrain", COMBAT_MANADRAIN},       {"fieldabsorbpercenthealing", COMBAT_HEALING},
    {"fieldabsorbpercentdrown", COMBAT_DROWNDAMAGE},         {"fieldabsorbpercentice", COMBAT_ICEDAMAGE},
    {"fieldabsorbpercentholy", COMBAT_HOLYDAMAGE},           {"fieldabsorbpercentdeath", COMBAT_DEATHDAMAGE},
};

const std::unordered_map<std::string, CombatType_t> AbilitieElementsMap = {
    {"elementphysical", COMBAT_PHYSICALDAMAGE}, {"elementenergy", COMBAT_ENERGYDAMAGE},
    {"elementearth", COMBAT_EARTHDAMAGE},       {"elementfire", COMBAT_FIREDAMAGE},
    {"elementdrown", COMBAT_DROWNDAMAGE},       {"elementice", COMBAT_ICEDAMAGE},
    {"elementholy", COMBAT_HOLYDAMAGE},         {"elementdeath", COMBAT_DEATHDAMAGE},
};

const std::unordered_map<std::string, ConditionType_t> AbilitieConditionTypeMap = {
    {"suppressdrunk", CONDITION_DRUNK},     {"suppressenergy", CONDITION_ENERGY},
    {"suppressfire", CONDITION_FIRE},       {"suppresspoison", CONDITION_POISON},
    {"suppressdrown", CONDITION_DROWN},     {"suppressphysical", CONDITION_BLEEDING},
    {"suppressfreeze", CONDITION_FREEZING}, {"suppressdazzle", CONDITION_DAZZLED},
    {"suppresscurse", CONDITION_CURSED},
};

const std::unordered_map<std::string, SpecialSkills_t> AbilitieSpecialSkillMap = {
    {"criticalchance", SPECIALSKILL_CRITICALHITCHANCE},    {"criticalamount", SPECIALSKILL_CRITICALHITAMOUNT},
    {"criticalhitchance", SPECIALSKILL_CRITICALHITCHANCE}, {"criticalhitamount", SPECIALSKILL_CRITICALHITAMOUNT},
    {"lifeleechchance", SPECIALSKILL_LIFELEECHCHANCE},     {"lifeleechamount", SPECIALSKILL_LIFELEECHAMOUNT},
    {"manaleechchance", SPECIALSKILL_MANALEECHCHANCE},     {"manaleechamount", SPECIALSKILL_MANALEECHAMOUNT},
};

static int luaItemTypeCreate(lua_State* L)
{
	// ItemType(id or name)
	uint32_t id;
	if (isNumber(L, 2)) {
		id = getNumber<uint32_t>(L, 2);
	} else if (isString(L, 2)) {
		id = Item::items.getItemIdByName(getString(L, 2));
	} else {
		lua_pushnil(L);
		return 1;
	}

	const ItemType& itemType = Item::items[id];
	pushUserdata<const ItemType>(L, &itemType);
	setMetatable(L, -1, "ItemType");
	return 1;
}

static int luaItemTypeCorpse(lua_State* L)
{
	// get: itemType:corpseType() set: itemType:corpse(raceType)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->corpseType);
		} else {
			const std::string& tmpStrValue = boost::algorithm::to_lower_copy(getString(L, 2));
			auto it2 = RaceTypesMap.find(tmpStrValue);
			if (it2 != RaceTypesMap.end()) {
				itemType->corpseType = it2->second;
				pushBoolean(L, true);
			} else {
				std::cout << "[Warning - Items::parseItemLua] Unknown corpseType: " << tmpStrValue << std::endl;
				pushBoolean(L, false);
			}
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeDoor(lua_State* L)
{
	// get: itemType:door() set: itemType:door(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->isDoor());
		} else {
			if (getBoolean(L, 2)) {
				itemType->type = ITEM_TYPE_DOOR;
			} else {
				itemType->type = ITEM_TYPE_NONE;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeContainer(lua_State* L)
{
	// get: itemType:container() set: itemType:container(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->isContainer());
		} else {
			if (getBoolean(L, 2)) {
				itemType->group = ITEM_GROUP_CONTAINER;
				itemType->type = ITEM_TYPE_CONTAINER;
			} else {
				itemType->group = ITEM_GROUP_NONE;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeFluidContainer(lua_State* L)
{
	// get: itemType:fluidContainer() set: itemType:fluidContainer(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->isFluidContainer());
		} else {
			if (getBoolean(L, 2)) {
				itemType->group = ITEM_GROUP_FLUID;
			} else {
				itemType->group = ITEM_GROUP_NONE;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeRune(lua_State* L)
{
	// get: itemType:rune() set: itemType:rune(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->isRune());
		} else {
			if (getBoolean(L, 2)) {
				itemType->type = ITEM_TYPE_RUNE;
			} else {
				itemType->type = ITEM_TYPE_NONE;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeWritable(lua_State* L)
{
	// get: itemType:writeable() set: itemType:writeable(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->canWriteText);
		} else {
			itemType->canWriteText = getBoolean(L, 2);
			itemType->canReadText = itemType->canWriteText;
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeStopDuration(lua_State* L)
{
	// get: itemType:stopDuration() set: itemType:stopDuration(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->stopTime);
		} else {
			itemType->stopTime = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeShowDuration(lua_State* L)
{
	// get: itemType:showDuration() set: itemType:showDuration(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->showDuration);
		} else {
			itemType->showDuration = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeShowAttributes(lua_State* L)
{
	// get: itemType:showAttributes() set: itemType:showAttributes(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->showAttributes);
		} else {
			itemType->showAttributes = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeShowCharges(lua_State* L)
{
	// get: itemType:showCharges() set: itemType:showCharges(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->showCharges);
		} else {
			itemType->showCharges = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeShowCount(lua_State* L)
{
	// get: itemType:showCount() set: itemType:showCount(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->showCount);
		} else {
			itemType->showCount = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeReplaceable(lua_State* L)
{
	// get: itemType:replaceable() set: itemType:replaceable(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->replaceable);
		} else {
			itemType->replaceable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeDuration(lua_State* L)
{
	// get: itemType:duration() set: itemType:duration(time)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->decayTime);
		} else {
			itemType->decayTime = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeFloorChange(lua_State* L)
{
	// get: itemType:floorChange() set: itemType:floorChange(dir)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->floorChange);
		} else {
			if (isNumber(L, 2)) {
				itemType->floorChange = getNumber<uint8_t>(L, 2);
			} else {
				const std::string& tmpStrValue = boost::algorithm::to_lower_copy(getString(L, 2));
				auto it2 = TileStatesMap.find(tmpStrValue);
				if (it2 != TileStatesMap.end()) {
					itemType->floorChange |= it2->second;
				} else {
					std::cout << "[Warning - Items::parseItemLua] Unknown floorChange: " << tmpStrValue << std::endl;
				}
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeLevelDoor(lua_State* L)
{
	// get: itemType:levelDoor() set: itemType:levelDoor(level)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->levelDoor);
		} else {
			itemType->levelDoor = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeVocationString(lua_State* L)
{
	// get: itemType:vocationString() set: itemType:vocationString(string)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushString(L, itemType->vocationString);
		} else {
			itemType->vocationString = getString(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeMinReqLevel(lua_State* L)
{
	// get: itemType:minReqLevel() set: itemType:minReqLevel(uint32_t)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushinteger(L, itemType->minReqLevel);
		} else {
			itemType->minReqLevel = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeMinReqMagicLevel(lua_State* L)
{
	// get: itemType:minReqMagicLevel() set: itemType:minReqMagicLevel(uint32_t)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushinteger(L, itemType->minReqMagicLevel);
		} else {
			itemType->minReqMagicLevel = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeGetMarketBuyStatistics(lua_State* L)
{
	// itemType:getMarketBuyStatistics()
	const ItemType* itemType = getUserdata<const ItemType>(L, 1);
	if (itemType) {
		MarketStatistics* statistics = IOMarket::getInstance().getPurchaseStatistics(itemType->id);
		if (statistics) {
			lua_createtable(L, 0, 4);
			setField(L, "numTransactions", statistics->numTransactions);
			setField(L, "totalPrice", statistics->totalPrice);
			setField(L, "highestPrice", statistics->highestPrice);
			setField(L, "lowestPrice", statistics->lowestPrice);
		} else {
			lua_pushnil(L);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeGetMarketSellStatistics(lua_State* L)
{
	// itemType:getMarketSellStatistics()
	const ItemType* itemType = getUserdata<const ItemType>(L, 1);
	if (itemType) {
		MarketStatistics* statistics = IOMarket::getInstance().getSaleStatistics(itemType->id);
		if (statistics) {
			lua_createtable(L, 0, 4);
			setField(L, "numTransactions", statistics->numTransactions);
			setField(L, "totalPrice", statistics->totalPrice);
			setField(L, "highestPrice", statistics->highestPrice);
			setField(L, "lowestPrice", statistics->lowestPrice);
		} else {
			lua_pushnil(L);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeEffect(lua_State* L)
{
	// get: itemType:effect() set: itemType:effect(magicEffect)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->magicEffect);
		} else {
			const std::string& effectName = getString(L, 2);
			MagicEffectClasses effect = getMagicEffect(boost::algorithm::to_lower_copy(effectName));
			if (effect != CONST_ME_NONE) {
				itemType->magicEffect = effect;
			} else {
				std::cout << "[Warning - Items::parseItemLua] Unknown effect: " << effectName << std::endl;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeContainerSize(lua_State* L)
{
	// get: itemType:containerSize() set: itemType:containerSize(size)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->maxItems);
		} else {
			itemType->maxItems = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeRotateTo(lua_State* L)
{
	// get: itemType:rotateTo() set: itemType:rotateTo(id)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->rotateTo);
		} else {
			itemType->rotateTo = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeBedPartnerDirection(lua_State* L)
{
	// get: itemType:bedPartnerDirection() set: itemType:bedPartnerDirection(dir)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->bedPartnerDir);
		} else {
			itemType->bedPartnerDir = getDirection(getString(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeFemaleSleeper(lua_State* L)
{
	// get: itemType:femaleSleeper() set: itemType:femaleSleeper(id)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->transformToOnUse[PLAYERSEX_FEMALE]);
		} else {
			uint16_t value = getNumber<uint16_t>(L, 2);
			itemType->transformToOnUse[PLAYERSEX_FEMALE] = value;

			ItemType& other = Item::items.getItemType(value);
			if (other.transformToFree == 0) {
				other.transformToFree = itemType->id;
			}

			if (itemType->transformToOnUse[PLAYERSEX_MALE] == 0) {
				itemType->transformToOnUse[PLAYERSEX_MALE] = value;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeMaleSleeper(lua_State* L)
{
	// get: itemType:maleSleeper() set: itemType:maleSleeper(id)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->transformToOnUse[PLAYERSEX_FEMALE]);
		} else {
			uint16_t value = getNumber<uint16_t>(L, 2);
			itemType->transformToOnUse[PLAYERSEX_MALE] = value;

			ItemType& other = Item::items.getItemType(value);
			if (other.transformToFree == 0) {
				other.transformToFree = itemType->id;
			}

			if (itemType->transformToOnUse[PLAYERSEX_FEMALE] == 0) {
				itemType->transformToOnUse[PLAYERSEX_FEMALE] = value;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeMaxTextLen(lua_State* L)
{
	// get: itemType:maxTextLen() set: itemType:maxTextLen(size)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->maxTextLen);
		} else {
			itemType->maxTextLen = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeWriteOnceItemId(lua_State* L)
{
	// get: itemType:writeOnceItemId() set: itemType:writeOnceItemId(id)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->writeOnceItemId);
		} else {
			itemType->writeOnceItemId = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeRuneSpellName(lua_State* L)
{
	// get: itemType:runeSpellName() set: itemType:runeSpellName(name)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType && itemType->isRune()) {
		if (lua_gettop(L) == 1) {
			pushString(L, itemType->runeSpellName);
		} else {
			itemType->runeSpellName = getString(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeWorth(lua_State* L)
{
	// get: itemType:worth() set: itemType:worth(ammount)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->worth);
		} else {
			itemType->worth = getNumber<uint64_t>(L, 2);
			Item::items.parseItemWorth(itemType->worth, itemType->id);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeField(lua_State* L)
{
	// get: itemType:field() set: itemType:field(fied)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		auto fieldBlock = itemType->getFieldBlock();
		if (lua_gettop(L) == 1) {
			pushFieldBlock(L, itemType->getFieldBlock());
		} else {
			fieldBlock = getFieldBlock(L, 2);
			bool onlyName = false;
			if (isString(L, 2)) {
				fieldBlock.name = getString(L, 2);
				onlyName = true;
			} else {
				fieldBlock = getFieldBlock(L, 2);
			}

			itemType->group = ITEM_GROUP_MAGICFIELD;
			itemType->type = ITEM_TYPE_MAGICFIELD;

			CombatType_t combatType = COMBAT_NONE;
			ConditionDamage* conditionDamage = nullptr;

			const std::string& tmpStrValue = boost::algorithm::to_lower_copy(fieldBlock.name);
			if (tmpStrValue == "fire") {
				conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_FIRE);
				combatType = COMBAT_FIREDAMAGE;
			} else if (tmpStrValue == "energy") {
				conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_ENERGY);
				combatType = COMBAT_ENERGYDAMAGE;
			} else if (tmpStrValue == "poison") {
				conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_POISON);
				combatType = COMBAT_EARTHDAMAGE;
			} else if (tmpStrValue == "drown") {
				conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_DROWN);
				combatType = COMBAT_DROWNDAMAGE;
			} else if (tmpStrValue == "physical") {
				conditionDamage = new ConditionDamage(CONDITIONID_COMBAT, CONDITION_BLEEDING);
				combatType = COMBAT_PHYSICALDAMAGE;
			} else {
				std::cout << "[Warning - Items::parseItemLua] Unknown field value: " << tmpStrValue
				          << " itemId: " << itemType->id << std::endl;
			}

			if (combatType != COMBAT_NONE) {
				itemType->combatType = combatType;
				itemType->conditionDamage.reset(conditionDamage);

				if (!onlyName) {
					if (fieldBlock.start > 0) {
						std::list<int32_t> damageList;
						ConditionDamage::generateDamageList(fieldBlock.damage, fieldBlock.start, damageList);
						for (int32_t damageValue : damageList) {
							conditionDamage->addDamage(1, fieldBlock.ticks, -damageValue);
						}
						fieldBlock.start = 0;
					} else {
						conditionDamage->addDamage(fieldBlock.count, fieldBlock.ticks, fieldBlock.damage);
					}
				}

				// datapack compatibility, presume damage to be initialdamage if initialdamage is not declared.
				// initDamage = 0 (dont override initDamage with damage, dont set any initDamage)
				// initDamage = -1 (undefined, override initDamage with damage)
				if (fieldBlock.initDamage > 0 || fieldBlock.initDamage < -1) {
					conditionDamage->setInitDamage(-fieldBlock.initDamage);
				} else if (fieldBlock.initDamage == -1 && fieldBlock.damage != 0) {
					conditionDamage->setInitDamage(fieldBlock.damage);
				}

				conditionDamage->setParam(CONDITION_PARAM_FIELD, 1);

				if (conditionDamage->getTotalDamage() > 0) {
					conditionDamage->setParam(CONDITION_PARAM_FORCEUPDATE, 1);
				}
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeWieldInfo(lua_State* L)
{
	// itemType:getWieldInfo()
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->wieldInfo);
		} else {
			itemType->wieldInfo = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeRotatable(lua_State* L)
{
	// get: itemType:rotatable() set: itemType:rotatable(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->rotatable);
		} else {
			itemType->rotatable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeGroundTile(lua_State* L)
{
	// get: itemType:groundTile() set: itemType:groundTile(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->isGroundTile());
		} else {
			if (getBoolean(L, 2)) {
				itemType->group = ITEM_GROUP_GROUND;
			} else {
				itemType->group = ITEM_GROUP_NONE;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeMagicField(lua_State* L)
{
	// get: itemType:magicField() set: itemType:magicField(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->isMagicField());
		} else {
			if (getBoolean(L, 2)) {
				itemType->type = ITEM_TYPE_MAGICFIELD;
			} else {
				itemType->type = ITEM_TYPE_NONE;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeAllowPickupable(lua_State* L)
{
	// get: itemType:allowPickupable() set: itemType:allowPickupable(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->allowPickupable);
		} else {
			itemType->allowPickupable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeId(lua_State* L)
{
	// get: itemType:id() set: itemType:id(uint16_t)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->id);
		} else {
			itemType->id = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeClientId(lua_State* L)
{
	// get: itemType:clientId() set: itemType:clientId(uint16_t)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->clientId);
		} else {
			itemType->clientId = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeName(lua_State* L)
{
	// get: itemType:name() set: itemType:name(name)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushString(L, itemType->name);
		} else {
			itemType->name = getString(L, 2);
			Item::items.parseItemName(itemType->name, itemType->id);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypePluralName(lua_State* L)
{
	// get: itemType:pluralName() set: itemType:pluralName(name)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushString(L, itemType->getPluralName());
		} else {
			itemType->pluralName = getString(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeArticle(lua_State* L)
{
	// get: itemType:article() set: itemType:article(article)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushString(L, itemType->article);
		} else {
			itemType->article = getString(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeDescription(lua_State* L)
{
	// get: itemType:description() set: itemType:description(description)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushString(L, itemType->description);
		} else {
			itemType->description = getString(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeSlotPosition(lua_State* L)
{
	// get: itemType:slotPosition() set: itemType:slotPosition(slotPos)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->slotPosition);
		} else {
			const std::string& tmpStrValue = boost::algorithm::to_lower_copy(getString(L, 2));
			if (tmpStrValue == "head") {
				itemType->slotPosition |= SLOTP_HEAD;
			} else if (tmpStrValue == "body") {
				itemType->slotPosition |= SLOTP_ARMOR;
			} else if (tmpStrValue == "legs") {
				itemType->slotPosition |= SLOTP_LEGS;
			} else if (tmpStrValue == "feet") {
				itemType->slotPosition |= SLOTP_FEET;
			} else if (tmpStrValue == "backpack") {
				itemType->slotPosition |= SLOTP_BACKPACK;
			} else if (tmpStrValue == "two-handed") {
				itemType->slotPosition |= SLOTP_TWO_HAND;
			} else if (tmpStrValue == "right-hand") {
				itemType->slotPosition &= ~SLOTP_LEFT;
			} else if (tmpStrValue == "left-hand") {
				itemType->slotPosition &= ~SLOTP_RIGHT;
			} else if (tmpStrValue == "necklace") {
				itemType->slotPosition |= SLOTP_NECKLACE;
			} else if (tmpStrValue == "ring") {
				itemType->slotPosition |= SLOTP_RING;
			} else if (tmpStrValue == "ammo") {
				itemType->slotPosition |= SLOTP_AMMO;
			} else if (tmpStrValue == "hand") {
				itemType->slotPosition |= SLOTP_HAND;
			} else {
				std::cout << "[Warning - Items::parseItemLua] Unknown slotType: " << tmpStrValue << std::endl;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeCharges(lua_State* L)
{
	// get: itemType:charges() set: itemType:charges(charges)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->charges);
		} else {
			itemType->charges = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeFluidSource(lua_State* L)
{
	// get: itemType:fluidSource() set: itemType:fluidSource(fluidSource)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->fluidSource);
		} else {
			const std::string& tmpStrValue = boost::algorithm::to_lower_copy(getString(L, 2));
			auto it2 = FluidTypesMap.find(tmpStrValue);
			if (it2 != FluidTypesMap.end()) {
				itemType->fluidSource = it2->second;
			} else {
				std::cout << "[Warning - Items::parseItemLua] Unknown fluidSource: " << tmpStrValue << std::endl;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeCapacity(lua_State* L)
{
	// get: itemType:capacity() set: itemType:capacity(cap)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->maxItems);
		} else {
			itemType->maxItems = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeWeight(lua_State* L)
{
	// get: itemType:weight() set: itemType:weight(weight)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->weight);
		} else {
			itemType->weight = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeHitChance(lua_State* L)
{
	// get: itemType:hitChance() set: itemType:hitChance(chance)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->hitChance);
		} else {
			itemType->hitChance = std::min<int8_t>(100, std::max<int8_t>(-100, getNumber<int8_t>(L, 2)));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeMaxHitChance(lua_State* L)
{
	// get: itemType:maxHitChance() set: itemType:maxHitChance(chance)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->maxHitChance);
		} else {
			itemType->maxHitChance = std::min<uint32_t>(100, getNumber<uint32_t>(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeShootRange(lua_State* L)
{
	// get: itemType:shootRange() set: itemType:shootRange(range)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->shootRange);
		} else {
			itemType->shootRange = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeShootType(lua_State* L)
{
	// get: itemType:shootType() set: itemType:shootType(shootType)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->shootType);
		} else {
			const std::string& shootName = getString(L, 2);
			ShootType_t shoot = getShootType(boost::algorithm::to_lower_copy(shootName));
			if (shoot != CONST_ANI_NONE) {
				itemType->shootType = shoot;
			} else {
				std::cout << "[Warning - Items::parseItemLua] Unknown shootType: " << shootName << std::endl;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeAttack(lua_State* L)
{
	// get: itemType:attack() set: itemType:attack(atk)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->attack);
		} else {
			itemType->attack = getNumber<int32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeAttackSpeed(lua_State* L)
{
	// get: itemType:attackSpeed() set: itemType:attackSpeed(speed)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->attackSpeed);
		} else {
			itemType->attackSpeed = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeDefense(lua_State* L)
{
	// get: itemType:defense() set: itemType:defense(def)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->defense);
		} else {
			itemType->defense = getNumber<int32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeExtraDefense(lua_State* L)
{
	// get: itemType:extraDefense() set: itemType:extraDefense(extraDef)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->extraDefense);
		} else {
			itemType->extraDefense = getNumber<int32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeArmor(lua_State* L)
{
	// get: itemType:armor() set: itemType:armor(arm)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->armor);
		} else {
			itemType->armor = getNumber<int32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeWeaponType(lua_State* L)
{
	// get: itemType:weaponType() set: itemType:weaponType(weaponType)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->weaponType);
		} else {
			const std::string& tmpStrValue = boost::algorithm::to_lower_copy(getString(L, 2));
			auto it2 = WeaponTypesMap.find(tmpStrValue);
			if (it2 != WeaponTypesMap.end()) {
				itemType->weaponType = it2->second;
			} else {
				std::cout << "[Warning - Items::parseItemLua] Unknown weaponType: " << tmpStrValue << std::endl;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeAmmoType(lua_State* L)
{
	// get: itemType:ammoType() set: itemType:ammoType(ammoType)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->ammoType);
		} else {
			const std::string& ammoName = getString(L, 2);
			itemType->ammoType = getAmmoType(boost::algorithm::to_lower_copy(ammoName));
			if (itemType->ammoType == AMMO_NONE) {
				std::cout << "[Warning - Items::parseItemLua] Unknown ammoType: " << ammoName << std::endl;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeElementType(lua_State* L)
{
	// get: itemType:elementType() set: itemType:elementType(damage, elementType)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (!itemType) {
		lua_pushnil(L);
		return 1;
	}

	if (lua_gettop(L) == 1) {
		auto& abilities = itemType->abilities;
		if (abilities) {
			lua_pushnumber(L, abilities->elementType);
		} else {
			lua_pushnil(L);
		}
	} else {
		Abilities& abilities = itemType->getAbilities();
		abilities.elementDamage = getNumber<uint16_t>(L, 2);
		abilities.elementType = getNumber<CombatType_t>(L, 3, COMBAT_NONE);
		pushBoolean(L, true);
	}

	return 1;
}

static int luaItemTypeElementDamage(lua_State* L)
{
	// get: itemType:elementDamage() set: itemType:elementDamage(elementDamage)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (!itemType) {
		lua_pushnil(L);
		return 1;
	}

	if (lua_gettop(L) == 1) {
		auto& abilities = itemType->abilities;
		if (abilities) {
			lua_pushnumber(L, abilities->elementDamage);
		} else {
			lua_pushnil(L);
		}
	} else {
		Abilities& abilities = itemType->getAbilities();
		abilities.elementDamage = getNumber<uint16_t>(L, 2);
		pushBoolean(L, true);
	}

	return 1;
}

static int luaItemTypeTransformEquipId(lua_State* L)
{
	// get: itemType:transformEquipId() set: itemType:transformEquipId(id)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->transformEquipTo);
		} else {
			itemType->transformEquipTo = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeTransformDeEquipId(lua_State* L)
{
	// get: itemType:transformDeEquipId() set: itemType:transformDeEquipId(id)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->transformDeEquipTo);
		} else {
			itemType->transformDeEquipTo = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeDestroyId(lua_State* L)
{
	// get: itemType:destroyId() set: itemType:destroyId(id)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->destroyTo);
		} else {
			itemType->destroyTo = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeDecayId(lua_State* L)
{
	// get: itemType:decayId() set: itemType:decayId(id)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->decayTo);
		} else {
			itemType->decayTo = getNumber<int32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeRequiredLevel(lua_State* L)
{
	// get: itemType:requiredLevel() set: itemType:requiredLevel(level)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->minReqLevel);
		} else {
			itemType->minReqLevel = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeSubType(lua_State* L)
{
	// get: itemType:subType()
	const ItemType* itemType = getUserdata<const ItemType>(L, 1);
	if (itemType) {
		pushBoolean(L, itemType->hasSubType());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeAbilities(lua_State* L)
{
	// get: itemType:abilities() set: itemType:abilities(ability, value)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		Abilities& abilities = itemType->getAbilities();
		if (lua_gettop(L) == 1) {
			lua_createtable(L, 10, 12);
			setField(L, "healthGain", abilities.healthGain);
			setField(L, "healthTicks", abilities.healthTicks);
			setField(L, "manaGain", abilities.manaGain);
			setField(L, "manaTicks", abilities.manaTicks);
			setField(L, "conditionImmunities", abilities.conditionImmunities);
			setField(L, "conditionSuppressions", abilities.conditionSuppressions);
			setField(L, "speed", abilities.speed);
			setField(L, "elementDamage", abilities.elementDamage);
			setField(L, "elementType", abilities.elementType);

			lua_pushboolean(L, abilities.manaShield);
			lua_setfield(L, -2, "manaShield");
			lua_pushboolean(L, abilities.invisible);
			lua_setfield(L, -2, "invisible");
			lua_pushboolean(L, abilities.regeneration);
			lua_setfield(L, -2, "regeneration");

			// Stats
			lua_createtable(L, 0, STAT_LAST + 1);
			for (int32_t i = STAT_FIRST; i <= STAT_LAST; i++) {
				lua_pushnumber(L, abilities.stats[i]);
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "stats");

			// Stats percent
			lua_createtable(L, 0, STAT_LAST + 1);
			for (int32_t i = STAT_FIRST; i <= STAT_LAST; i++) {
				lua_pushnumber(L, abilities.statsPercent[i]);
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "statsPercent");

			// Skills
			lua_createtable(L, 0, SKILL_LAST + 1);
			for (int32_t i = SKILL_FIRST; i <= SKILL_LAST; i++) {
				lua_pushnumber(L, abilities.skills[i]);
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "skills");

			// Special skills
			lua_createtable(L, 0, SPECIALSKILL_LAST + 1);
			for (int32_t i = SPECIALSKILL_FIRST; i <= SPECIALSKILL_LAST; i++) {
				lua_pushnumber(L, abilities.specialSkills[i]);
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "specialSkills");

			// Field absorb percent
			lua_createtable(L, 0, COMBAT_COUNT);
			for (int32_t i = 0; i < COMBAT_COUNT; i++) {
				lua_pushnumber(L, abilities.fieldAbsorbPercent[i]);
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "fieldAbsorbPercent");

			// Absorb percent
			lua_createtable(L, 0, COMBAT_COUNT);
			for (int32_t i = 0; i < COMBAT_COUNT; i++) {
				lua_pushnumber(L, abilities.absorbPercent[i]);
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "absorbPercent");

			// special magic level
			lua_createtable(L, 0, COMBAT_COUNT);
			for (int32_t i = 0; i < COMBAT_COUNT; i++) {
				lua_pushnumber(L, abilities.specialMagicLevelSkill[i]);
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "specialMagicLevel");

			// Damage boost percent
			lua_createtable(L, 0, COMBAT_COUNT);
			for (int32_t i = 0; i < COMBAT_COUNT; i++) {
				lua_pushnumber(L, abilities.boostPercent[i]);
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "boostPercent");

			// Reflect chance
			lua_createtable(L, 0, COMBAT_COUNT);
			for (int32_t i = 0; i < COMBAT_COUNT; i++) {
				lua_pushnumber(L, abilities.reflect[i].chance);
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "reflectChance");

			// Reflect percent
			lua_createtable(L, 0, COMBAT_COUNT);
			for (int32_t i = 0; i < COMBAT_COUNT; i++) {
				lua_pushnumber(L, abilities.reflect[i].percent);
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "reflectPercent");
			return 1;
		} else {
			const std::string& abilitieName = boost::algorithm::to_lower_copy(getString(L, 2));
			// std::cout << abilitieName << std::endl;
			bool foundAbilitie = true;
			if (abilitieName == "healthgain") {
				abilities.regeneration = true;
				abilities.healthGain = getNumber<uint32_t>(L, 3);
			} else if (abilitieName == "healthticks") {
				abilities.regeneration = true;
				abilities.healthTicks = getNumber<uint32_t>(L, 3);
			} else if (abilitieName == "managain") {
				abilities.regeneration = true;
				abilities.manaGain = getNumber<uint32_t>(L, 3);
			} else if (abilitieName == "manaticks") {
				abilities.regeneration = true;
				abilities.manaTicks = getNumber<uint32_t>(L, 3);
			} else if (abilitieName == "speed") {
				abilities.speed = getNumber<int32_t>(L, 3);
			} else if (abilitieName == "elementdamage") {
				abilities.elementDamage = getNumber<uint16_t>(L, 3);
			} else if (abilitieName == "elementtype") {
				abilities.elementType = getNumber<CombatType_t>(L, 3, COMBAT_NONE);
			} /*/ else if (abilitieName == "shoottype") {
			    // abilities.shootType = getNumber<uint8_t>(L, 3, CONST_ANI_NONE);
			}*/
			else if (abilitieName == "manashield") {
				abilities.manaShield = getBoolean(L, 3);
			} else if (abilitieName == "invisible") {
				abilities.invisible = getBoolean(L, 3);
			} else if (abilitieName == "regeneration") {
				abilities.regeneration = getBoolean(L, 3);
			} else {
				foundAbilitie = false;
			}

			if (foundAbilitie) {
				pushBoolean(L, true);
				return 1;
			}

			if (abilitieName == "conditionsuppressions") {
				if (isNumber(L, 3)) {
					abilities.conditionSuppressions = getNumber<uint32_t>(L, 3);
					pushBoolean(L, true);
					return 1;
				}
			}

			// Stats
			auto statVal = AbilitieStatMap.find(abilitieName);
			if (statVal != AbilitieStatMap.end()) {
				abilities.stats[statVal->second] = getNumber<int32_t>(L, 3);
				pushBoolean(L, true);
				return 1;
			}

			auto statPcVal = AbilitieStatPercentMap.find(abilitieName);
			if (statPcVal != AbilitieStatPercentMap.end()) {
				abilities.statsPercent[statPcVal->second] = getNumber<int32_t>(L, 3);
				pushBoolean(L, true);
				return 1;
			}

			// Skills
			auto skillVal = AbilitieSkillMap.find(abilitieName);
			if (skillVal != AbilitieSkillMap.end()) {
				abilities.skills[skillVal->second] = getNumber<int32_t>(L, 3);
				pushBoolean(L, true);
				return 1;
			}

			// Special Skills
			auto specialSkillVal = AbilitieSpecialSkillMap.find(abilitieName);
			if (specialSkillVal != AbilitieSpecialSkillMap.end()) {
				abilities.specialSkills[specialSkillVal->second] = getNumber<int32_t>(L, 3);
				pushBoolean(L, true);
				return 1;
			}

			// AbsorbPercents
			auto absorbPercentVal = AbilitieAbsorbPercentMap.find(abilitieName);
			if (absorbPercentVal != AbilitieAbsorbPercentMap.end()) {
				abilities.absorbPercent[combatTypeToIndex(absorbPercentVal->second)] += getNumber<int16_t>(L, 3);
				pushBoolean(L, true);
				return 1;
			} else if (abilitieName == "absorbpercentmagic") {
				int16_t value = getNumber<int16_t>(L, 3);
				abilities.absorbPercent[combatTypeToIndex(COMBAT_ENERGYDAMAGE)] += value;
				abilities.absorbPercent[combatTypeToIndex(COMBAT_FIREDAMAGE)] += value;
				abilities.absorbPercent[combatTypeToIndex(COMBAT_EARTHDAMAGE)] += value;
				abilities.absorbPercent[combatTypeToIndex(COMBAT_ICEDAMAGE)] += value;
				abilities.absorbPercent[combatTypeToIndex(COMBAT_HOLYDAMAGE)] += value;
				abilities.absorbPercent[combatTypeToIndex(COMBAT_DEATHDAMAGE)] += value;
				pushBoolean(L, true);
				return 1;
			} else if (abilitieName == "absorbpercentall") {
				int16_t value = getNumber<int16_t>(L, 3);
				for (uint8_t i = COMBAT_NONE; i <= COMBAT_COUNT; i++) {
					abilities.absorbPercent[indexToCombatType(i)] += value;
				}
				pushBoolean(L, true);
				return 1;
			}

			// FieldAbsorbPercents
			auto fieldAbsorbPercentVal = AbilitieFieldAbsorbPercentMap.find(abilitieName);
			if (fieldAbsorbPercentVal != AbilitieFieldAbsorbPercentMap.end()) {
				abilities.fieldAbsorbPercent[combatTypeToIndex(fieldAbsorbPercentVal->second)] +=
				    getNumber<int16_t>(L, 3);
				pushBoolean(L, true);
				return 1;
			} else if (abilitieName == "fieldabsorbpercentall") {
				int16_t value = getNumber<int16_t>(L, 3);
				for (uint8_t i = COMBAT_NONE; i <= COMBAT_COUNT; i++) {
					abilities.fieldAbsorbPercent[indexToCombatType(i)] += value;
				}
				pushBoolean(L, true);
				return 1;
			}

			// Elements
			auto elementVal = AbilitieElementsMap.find(abilitieName);
			if (elementVal != AbilitieElementsMap.end()) {
				abilities.elementDamage = getNumber<uint16_t>(L, 3);
				abilities.elementType = elementVal->second;
				pushBoolean(L, true);
				return 1;
			}

			if (getBoolean(L, 3)) {
				auto suppressVal = AbilitieConditionTypeMap.find(abilitieName);
				if (suppressVal != AbilitieConditionTypeMap.end()) {
					abilities.conditionSuppressions |= suppressVal->second;
					pushBoolean(L, true);
					return 1;
				}
			}

			std::cout << "[Warning - Items::parseItemLua] Unknown abilitie: " << itemType->id << std::endl;
			pushBoolean(L, false);
			return 1;
		}
	}

	lua_pushnil(L);
	return 1;
}

static int luaItemTypeStoreItem(lua_State* L)
{
	// get: itemType:storeItem() set: itemType:storeItem(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->storeItem);
		} else {
			itemType->storeItem = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeRegister(lua_State* L)
{
	// itemType:register()
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (itemType->id != 0) {
			pushBoolean(L, true);
		} else {
			pushBoolean(L, false);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeSpeed(lua_State* L)
{
	// get: itemType:speed() set: itemType:speed(speed)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->speed);
		} else {
			itemType->speed = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeWareId(lua_State* L)
{
	// get: itemType:wareId() set: itemType:wareId(id)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->wareId);
		} else {
			itemType->wareId = getNumber<uint16_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeLight(lua_State* L)
{
	// get: itemType:light() set: itemType:light(id)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_createtable(L, 0, 2);
			setField(L, "level", itemType->lightLevel);
			setField(L, "color", itemType->lightColor);
		} else {
			itemType->lightLevel = getNumber<uint8_t>(L, 2);
			itemType->lightColor = getNumber<uint8_t>(L, 3);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeAlwaysOnTopOrder(lua_State* L)
{
	// get: itemType:alwaysOnTopOrder() set: itemType:alwaysOnTopOrder(order)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->alwaysOnTopOrder);
		} else {
			itemType->alwaysOnTopOrder = getNumber<uint8_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeClassification(lua_State* L)
{
	// get: itemType:classification() set: itemType:classification(int)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->classification);
		} else {
			itemType->classification = getNumber<uint8_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeGroup(lua_State* L)
{
	// itemType:group()
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->group);
		} else {
			const std::string& tmpStrValue = boost::algorithm::to_lower_copy(getString(L, 2));
			auto it2 = ItemGroupMap.find(tmpStrValue);
			if (it2 != ItemGroupMap.end()) {
				itemType->group = it2->second;
			} else {
				std::cout << "[Warning - Items::parseItemLua] Unknown group: " << tmpStrValue << std::endl;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeType(lua_State* L)
{
	// get: itemType:type() set: itemType:type(type)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, itemType->type);
		} else {
			const std::string& tmpStrValue = boost::algorithm::to_lower_copy(getString(L, 2));
			auto it2 = ItemTypesMap.find(tmpStrValue);
			if (it2 != ItemTypesMap.end()) {
				itemType->type = it2->second;
				if (itemType->type == ITEM_TYPE_CONTAINER) {
					itemType->group = ITEM_GROUP_CONTAINER;
				}
			} else {
				std::cout << "[Warning - Items::parseItemLua] Unknown type: " << tmpStrValue << std::endl;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeBlockSolid(lua_State* L)
{
	// get: itemType:blockSolid() set: itemType:blockSolid(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->blockSolid);
		} else {
			itemType->blockSolid = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeBlockProjectile(lua_State* L)
{
	// get: itemType:blockProjectile() set: itemType:blockProjectile(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->blockProjectile);
		} else {
			itemType->blockProjectile = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeBlockPathFind(lua_State* L)
{
	// get: itemType:blockPathFind() set: itemType:blockPathFind(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->blockPathFind);
		} else {
			itemType->blockPathFind = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeHasHeight(lua_State* L)
{
	// get: itemType:hasHeight() set: itemType:hasHeight(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->hasHeight);
		} else {
			itemType->hasHeight = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeUseable(lua_State* L)
{
	// get: itemType:useable() set: itemType:useable(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->isUseable());
		} else {
			itemType->useable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypePickupable(lua_State* L)
{
	// get: itemType:pickupable() set: itemType:pickupable(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->isPickupable());
		} else {
			itemType->pickupable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeMoveable(lua_State* L)
{
	// get: itemType:moveable() set: itemType:moveable(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->moveable);
		} else {
			itemType->moveable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeStackable(lua_State* L)
{
	// get: itemType:stackable() set: itemType:stackable(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->stackable);
		} else {
			itemType->stackable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeAlwaysOnTop(lua_State* L)
{
	// get: itemType:alwaysOnTop() set: itemType:alwaysOnTop(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->alwaysOnTop);
		} else {
			itemType->alwaysOnTop = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeIsVertical(lua_State* L)
{
	// get: itemType:isVertical() set: itemType:isVertical(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->isVertical);
		} else {
			itemType->isVertical = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeIsHorizontal(lua_State* L)
{
	// get: itemType:isHorizontal() set: itemType:isHorizontal(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->isHorizontal);
		} else {
			itemType->isHorizontal = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeIsHangable(lua_State* L)
{
	// get: itemType:isHangable() set: itemType:isHangable(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->isHangable);
		} else {
			itemType->isHangable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeAllowDistRead(lua_State* L)
{
	// get: itemType:allowDistRead() set: itemType:allowDistRead(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->allowDistRead);
		} else {
			itemType->allowDistRead = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeRotateable(lua_State* L)
{
	// get: itemType:rotateable() set: itemType:rotateable(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->rotatable);
		} else {
			itemType->rotatable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeReadable(lua_State* L)
{
	// get: itemType:readable() set: itemType:readable(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->canReadText);
		} else {
			itemType->canReadText = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeLookThrough(lua_State* L)
{
	// get: itemType:lookThrough() set: itemType:lookThrough(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->lookThrough);
		} else {
			itemType->lookThrough = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeIsAnimation(lua_State* L)
{
	// get: itemType:isAnimation() set: itemType:isAnimation(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->isAnimation);
		} else {
			itemType->isAnimation = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeWalkStack(lua_State* L)
{
	// get: itemType:walkStack() set: itemType:walkStack(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->walkStack);
		} else {
			itemType->walkStack = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaItemTypeForceUse(lua_State* L)
{
	// get: itemType:forceUse() set: itemType:forceUse(bool)
	ItemType* itemType = getUserdata<ItemType>(L, 1);
	if (itemType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, itemType->forceUse);
		} else {
			itemType->forceUse = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

namespace LuaItemType {
static void registerFunctions(LuaScriptInterface* interface)
{
	// ItemType
	interface->registerClass("ItemType", "", luaItemTypeCreate);
	interface->registerMetaMethod("ItemType", "__eq", interface->luaUserdataCompare);

	interface->registerMethod("ItemType", "corpseType", luaItemTypeCorpse);
	interface->registerMethod("ItemType", "door", luaItemTypeDoor);
	interface->registerMethod("ItemType", "container", luaItemTypeContainer);
	interface->registerMethod("ItemType", "fluidContainer", luaItemTypeFluidContainer);
	interface->registerMethod("ItemType", "rune", luaItemTypeRune);
	interface->registerMethod("ItemType", "writable", luaItemTypeWritable);
	interface->registerMethod("ItemType", "effect", luaItemTypeEffect);
	interface->registerMethod("ItemType", "containerSize", luaItemTypeContainerSize);
	interface->registerMethod("ItemType", "rotateTo", luaItemTypeRotateTo);
	interface->registerMethod("ItemType", "bedPartnerDirection", luaItemTypeBedPartnerDirection);
	interface->registerMethod("ItemType", "femaleSleeper", luaItemTypeFemaleSleeper);
	interface->registerMethod("ItemType", "maleSleeper", luaItemTypeMaleSleeper);
	interface->registerMethod("ItemType", "maxTextLen", luaItemTypeMaxTextLen);
	interface->registerMethod("ItemType", "writeOnceItemId", luaItemTypeWriteOnceItemId);
	interface->registerMethod("ItemType", "runeSpellName", luaItemTypeRuneSpellName);
	interface->registerMethod("ItemType", "worth", luaItemTypeWorth);
	interface->registerMethod("ItemType", "field", luaItemTypeField);
	interface->registerMethod("ItemType", "groundTile", luaItemTypeGroundTile);
	interface->registerMethod("ItemType", "magicField", luaItemTypeMagicField);

	interface->registerMethod("ItemType", "name", luaItemTypeName);
	interface->registerMethod("ItemType", "pluralName", luaItemTypePluralName);
	interface->registerMethod("ItemType", "article", luaItemTypeArticle);
	interface->registerMethod("ItemType", "description", luaItemTypeDescription);
	interface->registerMethod("ItemType", "slotPosition", luaItemTypeSlotPosition);

	interface->registerMethod("ItemType", "charges", luaItemTypeCharges);
	interface->registerMethod("ItemType", "fluidSource", luaItemTypeFluidSource);
	interface->registerMethod("ItemType", "capacity", luaItemTypeCapacity);
	interface->registerMethod("ItemType", "weight", luaItemTypeWeight);

	interface->registerMethod("ItemType", "hitChance", luaItemTypeHitChance);
	interface->registerMethod("ItemType", "maxHitChance", luaItemTypeMaxHitChance);
	interface->registerMethod("ItemType", "shootRange", luaItemTypeShootRange);
	interface->registerMethod("ItemType", "shootType", luaItemTypeShootType);

	interface->registerMethod("ItemType", "attack", luaItemTypeAttack);
	interface->registerMethod("ItemType", "attackSpeed", luaItemTypeAttackSpeed);
	interface->registerMethod("ItemType", "defense", luaItemTypeDefense);
	interface->registerMethod("ItemType", "extraDefense", luaItemTypeExtraDefense);
	interface->registerMethod("ItemType", "armor", luaItemTypeArmor);
	interface->registerMethod("ItemType", "weaponType", luaItemTypeWeaponType);

	interface->registerMethod("ItemType", "elementType", luaItemTypeElementType);
	interface->registerMethod("ItemType", "elementDamage", luaItemTypeElementDamage);

	interface->registerMethod("ItemType", "transformEquipId", luaItemTypeTransformEquipId);
	interface->registerMethod("ItemType", "transformDeEquipId", luaItemTypeTransformDeEquipId);
	interface->registerMethod("ItemType", "destroyId", luaItemTypeDestroyId);
	interface->registerMethod("ItemType", "decayId", luaItemTypeDecayId);
	interface->registerMethod("ItemType", "requiredLevel", luaItemTypeRequiredLevel);
	interface->registerMethod("ItemType", "ammoType", luaItemTypeAmmoType);

	interface->registerMethod("ItemType", "abilities", luaItemTypeAbilities);

	interface->registerMethod("ItemType", "showAttributes", luaItemTypeShowAttributes);
	interface->registerMethod("ItemType", "showCount", luaItemTypeShowCount);
	interface->registerMethod("ItemType", "showCharges", luaItemTypeShowCharges);
	interface->registerMethod("ItemType", "stopDuration", luaItemTypeStopDuration);
	interface->registerMethod("ItemType", "showDuration", luaItemTypeShowDuration);
	interface->registerMethod("ItemType", "wieldInfo", luaItemTypeWieldInfo);
	interface->registerMethod("ItemType", "replaceable", luaItemTypeReplaceable);
	interface->registerMethod("ItemType", "duration", luaItemTypeDuration);
	interface->registerMethod("ItemType", "floorChange", luaItemTypeFloorChange);
	interface->registerMethod("ItemType", "levelDoor", luaItemTypeLevelDoor);
	interface->registerMethod("ItemType", "vocationString", luaItemTypeVocationString);
	interface->registerMethod("ItemType", "minReqLevel", luaItemTypeMinReqLevel);
	interface->registerMethod("ItemType", "minReqMagicLevel", luaItemTypeMinReqMagicLevel);
	interface->registerMethod("ItemType", "getMarketBuyStatistics", luaItemTypeGetMarketBuyStatistics);
	interface->registerMethod("ItemType", "getMarketSellStatistics", luaItemTypeGetMarketSellStatistics);

	interface->registerMethod("ItemType", "subType", luaItemTypeSubType);

	interface->registerMethod("ItemType", "storeItem", luaItemTypeStoreItem);
	interface->registerMethod("ItemType", "register", luaItemTypeRegister);

	// properties
	interface->registerMethod("ItemType", "id", luaItemTypeId);
	interface->registerMethod("ItemType", "clientId", luaItemTypeClientId);
	interface->registerMethod("ItemType", "speed", luaItemTypeSpeed);
	interface->registerMethod("ItemType", "wareId", luaItemTypeWareId);
	interface->registerMethod("ItemType", "light", luaItemTypeLight);
	interface->registerMethod("ItemType", "alwaysOnTopOrder", luaItemTypeAlwaysOnTopOrder);
	interface->registerMethod("ItemType", "classification", luaItemTypeClassification);
	interface->registerMethod("ItemType", "group", luaItemTypeGroup);
	interface->registerMethod("ItemType", "type", luaItemTypeType);
	interface->registerMethod("ItemType", "blockSolid", luaItemTypeBlockSolid);
	interface->registerMethod("ItemType", "blockProjectile", luaItemTypeBlockProjectile);
	interface->registerMethod("ItemType", "blockPathFind", luaItemTypeBlockPathFind);
	interface->registerMethod("ItemType", "hasHeight", luaItemTypeHasHeight);
	interface->registerMethod("ItemType", "useable", luaItemTypeUseable);
	interface->registerMethod("ItemType", "pickupable", luaItemTypePickupable);
	interface->registerMethod("ItemType", "moveable", luaItemTypeMoveable);
	interface->registerMethod("ItemType", "stackable", luaItemTypeStackable);
	interface->registerMethod("ItemType", "alwaysOnTop", luaItemTypeAlwaysOnTop);
	interface->registerMethod("ItemType", "isVertical", luaItemTypeIsVertical);
	interface->registerMethod("ItemType", "isHorizontal", luaItemTypeIsHorizontal);
	interface->registerMethod("ItemType", "isHangable", luaItemTypeIsHangable);
	interface->registerMethod("ItemType", "allowDistRead", luaItemTypeAllowDistRead);
	interface->registerMethod("ItemType", "rotateable", luaItemTypeRotateable);
	interface->registerMethod("ItemType", "readable", luaItemTypeReadable);
	interface->registerMethod("ItemType", "lookThrough", luaItemTypeLookThrough);
	interface->registerMethod("ItemType", "isAnimation", luaItemTypeIsAnimation);
	interface->registerMethod("ItemType", "walkStack", luaItemTypeWalkStack);
	interface->registerMethod("ItemType", "forceUse", luaItemTypeForceUse);
}
} // namespace LuaItemType

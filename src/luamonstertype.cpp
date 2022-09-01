// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "luascript.h"
#include "monster.h"
#include "monsters.h"
#include "script.h"
#include "spells.h"

extern Monsters g_monsters;
extern Scripts* g_scripts;

using namespace Lua;

static int luaMonsterTypeCreate(lua_State* L)
{
	// MonsterType(name)
	MonsterType* monsterType = g_monsters.getMonsterType(getString(L, 2));
	if (monsterType) {
		pushUserdata<MonsterType>(L, monsterType);
		setMetatable(L, -1, "MonsterType");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeIsAttackable(lua_State* L)
{
	// get: monsterType:isAttackable() set: monsterType:isAttackable(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.isAttackable);
		} else {
			monsterType->info.isAttackable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeIsChallengeable(lua_State* L)
{
	// get: monsterType:isChallengeable() set: monsterType:isChallengeable(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.isChallengeable);
		} else {
			monsterType->info.isChallengeable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeIsConvinceable(lua_State* L)
{
	// get: monsterType:isConvinceable() set: monsterType:isConvinceable(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.isConvinceable);
		} else {
			monsterType->info.isConvinceable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeIsSummonable(lua_State* L)
{
	// get: monsterType:isSummonable() set: monsterType:isSummonable(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.isSummonable);
		} else {
			monsterType->info.isSummonable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeIsIgnoringSpawnBlock(lua_State* L)
{
	// get: monsterType:isIgnoringSpawnBlock() set: monsterType:isIgnoringSpawnBlock(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.isIgnoringSpawnBlock);
		} else {
			monsterType->info.isIgnoringSpawnBlock = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeIsIllusionable(lua_State* L)
{
	// get: monsterType:isIllusionable() set: monsterType:isIllusionable(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.isIllusionable);
		} else {
			monsterType->info.isIllusionable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeIsHostile(lua_State* L)
{
	// get: monsterType:isHostile() set: monsterType:isHostile(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.isHostile);
		} else {
			monsterType->info.isHostile = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeIsPushable(lua_State* L)
{
	// get: monsterType:isPushable() set: monsterType:isPushable(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.pushable);
		} else {
			monsterType->info.pushable = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeIsHealthHidden(lua_State* L)
{
	// get: monsterType:isHealthHidden() set: monsterType:isHealthHidden(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.hiddenHealth);
		} else {
			monsterType->info.hiddenHealth = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeIsBoss(lua_State* L)
{
	// get: monsterType:isBoss() set: monsterType:isBoss(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.isBoss);
		} else {
			monsterType->info.isBoss = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeCanPushItems(lua_State* L)
{
	// get: monsterType:canPushItems() set: monsterType:canPushItems(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.canPushItems);
		} else {
			monsterType->info.canPushItems = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeCanPushCreatures(lua_State* L)
{
	// get: monsterType:canPushCreatures() set: monsterType:canPushCreatures(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.canPushCreatures);
		} else {
			monsterType->info.canPushCreatures = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeCanWalkOnEnergy(lua_State* L)
{
	// get: monsterType:canWalkOnEnergy() set: monsterType:canWalkOnEnergy(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.canWalkOnEnergy);
		} else {
			monsterType->info.canWalkOnEnergy = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeCanWalkOnFire(lua_State* L)
{
	// get: monsterType:canWalkOnFire() set: monsterType:canWalkOnFire(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.canWalkOnFire);
		} else {
			monsterType->info.canWalkOnFire = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeCanWalkOnPoison(lua_State* L)
{
	// get: monsterType:canWalkOnPoison() set: monsterType:canWalkOnPoison(bool)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, monsterType->info.canWalkOnPoison);
		} else {
			monsterType->info.canWalkOnPoison = getBoolean(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeName(lua_State* L)
{
	// get: monsterType:name() set: monsterType:name(name)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushString(L, monsterType->name);
		} else {
			monsterType->name = getString(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeNameDescription(lua_State* L)
{
	// get: monsterType:nameDescription() set: monsterType:nameDescription(desc)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushString(L, monsterType->nameDescription);
		} else {
			monsterType->nameDescription = getString(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeHealth(lua_State* L)
{
	// get: monsterType:health() set: monsterType:health(health)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.health);
		} else {
			monsterType->info.health = getNumber<int32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeMaxHealth(lua_State* L)
{
	// get: monsterType:maxHealth() set: monsterType:maxHealth(health)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.healthMax);
		} else {
			monsterType->info.healthMax = getNumber<int32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeRunHealth(lua_State* L)
{
	// get: monsterType:runHealth() set: monsterType:runHealth(health)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.runAwayHealth);
		} else {
			monsterType->info.runAwayHealth = getNumber<int32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeExperience(lua_State* L)
{
	// get: monsterType:experience() set: monsterType:experience(exp)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.experience);
		} else {
			monsterType->info.experience = getNumber<uint64_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeSkull(lua_State* L)
{
	// get: monsterType:skull() set: monsterType:skull(str/constant)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.skull);
		} else {
			if (isNumber(L, 2)) {
				monsterType->info.skull = getNumber<Skulls_t>(L, 2);
			} else {
				monsterType->info.skull = getSkullType(getString(L, 2));
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeCombatImmunities(lua_State* L)
{
	// get: monsterType:combatImmunities() set: monsterType:combatImmunities(immunity)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.damageImmunities);
		} else {
			std::string immunity = getString(L, 2);
			if (immunity == "physical") {
				monsterType->info.damageImmunities |= COMBAT_PHYSICALDAMAGE;
				pushBoolean(L, true);
			} else if (immunity == "energy") {
				monsterType->info.damageImmunities |= COMBAT_ENERGYDAMAGE;
				pushBoolean(L, true);
			} else if (immunity == "fire") {
				monsterType->info.damageImmunities |= COMBAT_FIREDAMAGE;
				pushBoolean(L, true);
			} else if (immunity == "poison" || immunity == "earth") {
				monsterType->info.damageImmunities |= COMBAT_EARTHDAMAGE;
				pushBoolean(L, true);
			} else if (immunity == "drown") {
				monsterType->info.damageImmunities |= COMBAT_DROWNDAMAGE;
				pushBoolean(L, true);
			} else if (immunity == "ice") {
				monsterType->info.damageImmunities |= COMBAT_ICEDAMAGE;
				pushBoolean(L, true);
			} else if (immunity == "holy") {
				monsterType->info.damageImmunities |= COMBAT_HOLYDAMAGE;
				pushBoolean(L, true);
			} else if (immunity == "death") {
				monsterType->info.damageImmunities |= COMBAT_DEATHDAMAGE;
				pushBoolean(L, true);
			} else if (immunity == "lifedrain") {
				monsterType->info.damageImmunities |= COMBAT_LIFEDRAIN;
				pushBoolean(L, true);
			} else if (immunity == "manadrain") {
				monsterType->info.damageImmunities |= COMBAT_MANADRAIN;
				pushBoolean(L, true);
			} else {
				std::cout << "[Warning - Monsters::loadMonster] Unknown immunity name " << immunity
				          << " for monster: " << monsterType->name << std::endl;
				lua_pushnil(L);
			}
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeConditionImmunities(lua_State* L)
{
	// get: monsterType:conditionImmunities() set: monsterType:conditionImmunities(immunity)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.conditionImmunities);
		} else {
			std::string immunity = getString(L, 2);
			if (immunity == "physical") {
				monsterType->info.conditionImmunities |= CONDITION_BLEEDING;
				pushBoolean(L, true);
			} else if (immunity == "energy") {
				monsterType->info.conditionImmunities |= CONDITION_ENERGY;
				pushBoolean(L, true);
			} else if (immunity == "fire") {
				monsterType->info.conditionImmunities |= CONDITION_FIRE;
				pushBoolean(L, true);
			} else if (immunity == "poison" || immunity == "earth") {
				monsterType->info.conditionImmunities |= CONDITION_POISON;
				pushBoolean(L, true);
			} else if (immunity == "drown") {
				monsterType->info.conditionImmunities |= CONDITION_DROWN;
				pushBoolean(L, true);
			} else if (immunity == "ice") {
				monsterType->info.conditionImmunities |= CONDITION_FREEZING;
				pushBoolean(L, true);
			} else if (immunity == "holy") {
				monsterType->info.conditionImmunities |= CONDITION_DAZZLED;
				pushBoolean(L, true);
			} else if (immunity == "death") {
				monsterType->info.conditionImmunities |= CONDITION_CURSED;
				pushBoolean(L, true);
			} else if (immunity == "paralyze") {
				monsterType->info.conditionImmunities |= CONDITION_PARALYZE;
				pushBoolean(L, true);
			} else if (immunity == "outfit") {
				monsterType->info.conditionImmunities |= CONDITION_OUTFIT;
				pushBoolean(L, true);
			} else if (immunity == "drunk") {
				monsterType->info.conditionImmunities |= CONDITION_DRUNK;
				pushBoolean(L, true);
			} else if (immunity == "invisible" || immunity == "invisibility") {
				monsterType->info.conditionImmunities |= CONDITION_INVISIBLE;
				pushBoolean(L, true);
			} else if (immunity == "bleed") {
				monsterType->info.conditionImmunities |= CONDITION_BLEEDING;
				pushBoolean(L, true);
			} else {
				std::cout << "[Warning - Monsters::loadMonster] Unknown immunity name " << immunity
				          << " for monster: " << monsterType->name << std::endl;
				lua_pushnil(L);
			}
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeGetAttackList(lua_State* L)
{
	// monsterType:getAttackList()
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (!monsterType) {
		lua_pushnil(L);
		return 1;
	}

	lua_createtable(L, monsterType->info.attackSpells.size(), 0);

	int index = 0;
	for (const auto& spellBlock : monsterType->info.attackSpells) {
		lua_createtable(L, 0, 8);

		setField(L, "chance", spellBlock.chance);
		setField(L, "isCombatSpell", spellBlock.combatSpell ? 1 : 0);
		setField(L, "isMelee", spellBlock.isMelee ? 1 : 0);
		setField(L, "minCombatValue", spellBlock.minCombatValue);
		setField(L, "maxCombatValue", spellBlock.maxCombatValue);
		setField(L, "range", spellBlock.range);
		setField(L, "speed", spellBlock.speed);
		pushUserdata<CombatSpell>(L, static_cast<CombatSpell*>(spellBlock.spell));
		lua_setfield(L, -2, "spell");

		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaMonsterTypeAddAttack(lua_State* L)
{
	// monsterType:addAttack(monsterspell)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		MonsterSpell* spell = getUserdata<MonsterSpell>(L, 2);
		if (spell) {
			spellBlock_t sb;
			if (g_monsters.deserializeSpell(spell, sb, monsterType->name)) {
				monsterType->info.attackSpells.push_back(std::move(sb));
			} else {
				std::cout << monsterType->name << std::endl;
				std::cout << "[Warning - Monsters::loadMonster] Cant load spell. " << spell->name << std::endl;
			}
		} else {
			lua_pushnil(L);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeGetDefenseList(lua_State* L)
{
	// monsterType:getDefenseList()
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (!monsterType) {
		lua_pushnil(L);
		return 1;
	}

	lua_createtable(L, monsterType->info.defenseSpells.size(), 0);

	int index = 0;
	for (const auto& spellBlock : monsterType->info.defenseSpells) {
		lua_createtable(L, 0, 8);

		setField(L, "chance", spellBlock.chance);
		setField(L, "isCombatSpell", spellBlock.combatSpell ? 1 : 0);
		setField(L, "isMelee", spellBlock.isMelee ? 1 : 0);
		setField(L, "minCombatValue", spellBlock.minCombatValue);
		setField(L, "maxCombatValue", spellBlock.maxCombatValue);
		setField(L, "range", spellBlock.range);
		setField(L, "speed", spellBlock.speed);
		pushUserdata<CombatSpell>(L, static_cast<CombatSpell*>(spellBlock.spell));
		lua_setfield(L, -2, "spell");

		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaMonsterTypeAddDefense(lua_State* L)
{
	// monsterType:addDefense(monsterspell)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		MonsterSpell* spell = getUserdata<MonsterSpell>(L, 2);
		if (spell) {
			spellBlock_t sb;
			if (g_monsters.deserializeSpell(spell, sb, monsterType->name)) {
				monsterType->info.defenseSpells.push_back(std::move(sb));
			} else {
				std::cout << monsterType->name << std::endl;
				std::cout << "[Warning - Monsters::loadMonster] Cant load spell. " << spell->name << std::endl;
			}
		} else {
			lua_pushnil(L);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeGetElementList(lua_State* L)
{
	// monsterType:getElementList()
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (!monsterType) {
		lua_pushnil(L);
		return 1;
	}

	lua_createtable(L, monsterType->info.elementMap.size(), 0);
	for (const auto& elementEntry : monsterType->info.elementMap) {
		lua_pushnumber(L, elementEntry.second);
		lua_rawseti(L, -2, elementEntry.first);
	}
	return 1;
}

static int luaMonsterTypeAddElement(lua_State* L)
{
	// monsterType:addElement(type, percent)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		CombatType_t element = getNumber<CombatType_t>(L, 2);
		monsterType->info.elementMap[element] = getNumber<int32_t>(L, 3);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeGetVoices(lua_State* L)
{
	// monsterType:getVoices()
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (!monsterType) {
		lua_pushnil(L);
		return 1;
	}

	int index = 0;
	lua_createtable(L, monsterType->info.voiceVector.size(), 0);
	for (const auto& voiceBlock : monsterType->info.voiceVector) {
		lua_createtable(L, 0, 2);
		setField(L, "text", voiceBlock.text);
		setField(L, "yellText", voiceBlock.yellText);
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaMonsterTypeAddVoice(lua_State* L)
{
	// monsterType:addVoice(sentence, interval, chance, yell)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		voiceBlock_t voice;
		voice.text = getString(L, 2);
		monsterType->info.yellSpeedTicks = getNumber<uint32_t>(L, 3);
		monsterType->info.yellChance = getNumber<uint32_t>(L, 4);
		voice.yellText = getBoolean(L, 5);
		monsterType->info.voiceVector.push_back(voice);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeGetLoot(lua_State* L)
{
	// monsterType:getLoot()
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (!monsterType) {
		lua_pushnil(L);
		return 1;
	}

	pushLoot(L, monsterType->info.lootItems);
	return 1;
}

static int luaMonsterTypeAddLoot(lua_State* L)
{
	// monsterType:addLoot(loot)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		Loot* loot = getUserdata<Loot>(L, 2);
		if (loot) {
			monsterType->loadLoot(monsterType, loot->lootBlock);
			pushBoolean(L, true);
		} else {
			lua_pushnil(L);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeGetCreatureEvents(lua_State* L)
{
	// monsterType:getCreatureEvents()
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (!monsterType) {
		lua_pushnil(L);
		return 1;
	}

	int index = 0;
	lua_createtable(L, monsterType->info.scripts.size(), 0);
	for (const std::string& creatureEvent : monsterType->info.scripts) {
		pushString(L, creatureEvent);
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaMonsterTypeRegisterEvent(lua_State* L)
{
	// monsterType:registerEvent(name)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		monsterType->info.scripts.push_back(getString(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeEventOnCallback(lua_State* L)
{
	// monsterType:onThink(callback)
	// monsterType:onAppear(callback)
	// monsterType:onDisappear(callback)
	// monsterType:onMove(callback)
	// monsterType:onSay(callback)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (monsterType->loadCallback(&g_scripts->getScriptInterface())) {
			pushBoolean(L, true);
			return 1;
		}
		pushBoolean(L, false);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeEventType(lua_State* L)
{
	// monstertype:eventType(event)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		monsterType->info.eventType = getNumber<MonstersEvent_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeGetSummonList(lua_State* L)
{
	// monsterType:getSummonList()
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (!monsterType) {
		lua_pushnil(L);
		return 1;
	}

	int index = 0;
	lua_createtable(L, monsterType->info.summons.size(), 0);
	for (const auto& summonBlock : monsterType->info.summons) {
		lua_createtable(L, 0, 3);
		setField(L, "name", summonBlock.name);
		setField(L, "speed", summonBlock.speed);
		setField(L, "chance", summonBlock.chance);
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaMonsterTypeAddSummon(lua_State* L)
{
	// monsterType:addSummon(name, interval, chance[, max = -1])
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		summonBlock_t summon;
		summon.name = getString(L, 2);
		summon.speed = getNumber<int32_t>(L, 3);
		summon.chance = getNumber<int32_t>(L, 4);
		summon.max = getNumber<int32_t>(L, 5, -1);
		monsterType->info.summons.push_back(summon);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeMaxSummons(lua_State* L)
{
	// get: monsterType:maxSummons() set: monsterType:maxSummons(ammount)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.maxSummons);
		} else {
			monsterType->info.maxSummons = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeArmor(lua_State* L)
{
	// get: monsterType:armor() set: monsterType:armor(armor)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.armor);
		} else {
			monsterType->info.armor = getNumber<int32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeDefense(lua_State* L)
{
	// get: monsterType:defense() set: monsterType:defense(defense)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.defense);
		} else {
			monsterType->info.defense = getNumber<int32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeOutfit(lua_State* L)
{
	// get: monsterType:outfit() set: monsterType:outfit(outfit)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			pushOutfit(L, monsterType->info.outfit);
		} else {
			monsterType->info.outfit = getOutfit(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeRace(lua_State* L)
{
	// get: monsterType:race() set: monsterType:race(race)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	std::string race = getString(L, 2);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.race);
		} else {
			if (race == "venom") {
				monsterType->info.race = RACE_VENOM;
			} else if (race == "blood") {
				monsterType->info.race = RACE_BLOOD;
			} else if (race == "undead") {
				monsterType->info.race = RACE_UNDEAD;
			} else if (race == "fire") {
				monsterType->info.race = RACE_FIRE;
			} else if (race == "energy") {
				monsterType->info.race = RACE_ENERGY;
			} else {
				std::cout << "[Warning - Monsters::loadMonster] Unknown race type " << race << "." << std::endl;
				lua_pushnil(L);
				return 1;
			}
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeCorpseId(lua_State* L)
{
	// get: monsterType:corpseId() set: monsterType:corpseId(id)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.lookcorpse);
		} else {
			monsterType->info.lookcorpse = getNumber<uint16_t>(L, 2);
			lua_pushboolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeManaCost(lua_State* L)
{
	// get: monsterType:manaCost() set: monsterType:manaCost(mana)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.manaCost);
		} else {
			monsterType->info.manaCost = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeBaseSpeed(lua_State* L)
{
	// get: monsterType:baseSpeed() set: monsterType:baseSpeed(speed)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.baseSpeed);
		} else {
			monsterType->info.baseSpeed = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeLight(lua_State* L)
{
	// get: monsterType:light() set: monsterType:light(color, level)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (!monsterType) {
		lua_pushnil(L);
		return 1;
	}
	if (lua_gettop(L) == 1) {
		lua_pushnumber(L, monsterType->info.light.level);
		lua_pushnumber(L, monsterType->info.light.color);
		return 2;
	} else {
		monsterType->info.light.color = getNumber<uint8_t>(L, 2);
		monsterType->info.light.level = getNumber<uint8_t>(L, 3);
		pushBoolean(L, true);
	}
	return 1;
}

static int luaMonsterTypeStaticAttackChance(lua_State* L)
{
	// get: monsterType:staticAttackChance() set: monsterType:staticAttackChance(chance)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.staticAttackChance);
		} else {
			monsterType->info.staticAttackChance = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeTargetDistance(lua_State* L)
{
	// get: monsterType:targetDistance() set: monsterType:targetDistance(distance)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.targetDistance);
		} else {
			monsterType->info.targetDistance = getNumber<int32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeYellChance(lua_State* L)
{
	// get: monsterType:yellChance() set: monsterType:yellChance(chance)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.yellChance);
		} else {
			monsterType->info.yellChance = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeYellSpeedTicks(lua_State* L)
{
	// get: monsterType:yellSpeedTicks() set: monsterType:yellSpeedTicks(rate)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.yellSpeedTicks);
		} else {
			monsterType->info.yellSpeedTicks = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeChangeTargetChance(lua_State* L)
{
	// get: monsterType:changeTargetChance() set: monsterType:changeTargetChance(chance)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.changeTargetChance);
		} else {
			monsterType->info.changeTargetChance = getNumber<int32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMonsterTypeChangeTargetSpeed(lua_State* L)
{
	// get: monsterType:changeTargetSpeed() set: monsterType:changeTargetSpeed(speed)
	MonsterType* monsterType = getUserdata<MonsterType>(L, 1);
	if (monsterType) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, monsterType->info.changeTargetSpeed);
		} else {
			monsterType->info.changeTargetSpeed = getNumber<uint32_t>(L, 2);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

namespace LuaMonsterType {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("MonsterType", "", luaMonsterTypeCreate);
	interface->registerMetaMethod("MonsterType", "__eq", interface->luaUserdataCompare);

	interface->registerMethod("MonsterType", "isAttackable", luaMonsterTypeIsAttackable);
	interface->registerMethod("MonsterType", "isChallengeable", luaMonsterTypeIsChallengeable);
	interface->registerMethod("MonsterType", "isConvinceable", luaMonsterTypeIsConvinceable);
	interface->registerMethod("MonsterType", "isSummonable", luaMonsterTypeIsSummonable);
	interface->registerMethod("MonsterType", "isIgnoringSpawnBlock", luaMonsterTypeIsIgnoringSpawnBlock);
	interface->registerMethod("MonsterType", "isIllusionable", luaMonsterTypeIsIllusionable);
	interface->registerMethod("MonsterType", "isHostile", luaMonsterTypeIsHostile);
	interface->registerMethod("MonsterType", "isPushable", luaMonsterTypeIsPushable);
	interface->registerMethod("MonsterType", "isHealthHidden", luaMonsterTypeIsHealthHidden);
	interface->registerMethod("MonsterType", "isBoss", luaMonsterTypeIsBoss);

	interface->registerMethod("MonsterType", "canPushItems", luaMonsterTypeCanPushItems);
	interface->registerMethod("MonsterType", "canPushCreatures", luaMonsterTypeCanPushCreatures);

	interface->registerMethod("MonsterType", "canWalkOnEnergy", luaMonsterTypeCanWalkOnEnergy);
	interface->registerMethod("MonsterType", "canWalkOnFire", luaMonsterTypeCanWalkOnFire);
	interface->registerMethod("MonsterType", "canWalkOnPoison", luaMonsterTypeCanWalkOnPoison);

	interface->registerMethod("MonsterType", "name", luaMonsterTypeName);
	interface->registerMethod("MonsterType", "nameDescription", luaMonsterTypeNameDescription);

	interface->registerMethod("MonsterType", "health", luaMonsterTypeHealth);
	interface->registerMethod("MonsterType", "maxHealth", luaMonsterTypeMaxHealth);
	interface->registerMethod("MonsterType", "runHealth", luaMonsterTypeRunHealth);
	interface->registerMethod("MonsterType", "experience", luaMonsterTypeExperience);
	interface->registerMethod("MonsterType", "skull", luaMonsterTypeSkull);

	interface->registerMethod("MonsterType", "combatImmunities", luaMonsterTypeCombatImmunities);
	interface->registerMethod("MonsterType", "conditionImmunities", luaMonsterTypeConditionImmunities);

	interface->registerMethod("MonsterType", "getAttackList", luaMonsterTypeGetAttackList);
	interface->registerMethod("MonsterType", "addAttack", luaMonsterTypeAddAttack);

	interface->registerMethod("MonsterType", "getDefenseList", luaMonsterTypeGetDefenseList);
	interface->registerMethod("MonsterType", "addDefense", luaMonsterTypeAddDefense);

	interface->registerMethod("MonsterType", "getElementList", luaMonsterTypeGetElementList);
	interface->registerMethod("MonsterType", "addElement", luaMonsterTypeAddElement);

	interface->registerMethod("MonsterType", "getVoices", luaMonsterTypeGetVoices);
	interface->registerMethod("MonsterType", "addVoice", luaMonsterTypeAddVoice);

	interface->registerMethod("MonsterType", "getLoot", luaMonsterTypeGetLoot);
	interface->registerMethod("MonsterType", "addLoot", luaMonsterTypeAddLoot);

	interface->registerMethod("MonsterType", "getCreatureEvents", luaMonsterTypeGetCreatureEvents);
	interface->registerMethod("MonsterType", "registerEvent", luaMonsterTypeRegisterEvent);

	interface->registerMethod("MonsterType", "eventType", luaMonsterTypeEventType);
	interface->registerMethod("MonsterType", "onThink", luaMonsterTypeEventOnCallback);
	interface->registerMethod("MonsterType", "onAppear", luaMonsterTypeEventOnCallback);
	interface->registerMethod("MonsterType", "onDisappear", luaMonsterTypeEventOnCallback);
	interface->registerMethod("MonsterType", "onMove", luaMonsterTypeEventOnCallback);
	interface->registerMethod("MonsterType", "onSay", luaMonsterTypeEventOnCallback);

	interface->registerMethod("MonsterType", "getSummonList", luaMonsterTypeGetSummonList);
	interface->registerMethod("MonsterType", "addSummon", luaMonsterTypeAddSummon);

	interface->registerMethod("MonsterType", "maxSummons", luaMonsterTypeMaxSummons);

	interface->registerMethod("MonsterType", "armor", luaMonsterTypeArmor);
	interface->registerMethod("MonsterType", "defense", luaMonsterTypeDefense);
	interface->registerMethod("MonsterType", "outfit", luaMonsterTypeOutfit);
	interface->registerMethod("MonsterType", "race", luaMonsterTypeRace);
	interface->registerMethod("MonsterType", "corpseId", luaMonsterTypeCorpseId);
	interface->registerMethod("MonsterType", "manaCost", luaMonsterTypeManaCost);
	interface->registerMethod("MonsterType", "baseSpeed", luaMonsterTypeBaseSpeed);
	interface->registerMethod("MonsterType", "light", luaMonsterTypeLight);

	interface->registerMethod("MonsterType", "staticAttackChance", luaMonsterTypeStaticAttackChance);
	interface->registerMethod("MonsterType", "targetDistance", luaMonsterTypeTargetDistance);
	interface->registerMethod("MonsterType", "yellChance", luaMonsterTypeYellChance);
	interface->registerMethod("MonsterType", "yellSpeedTicks", luaMonsterTypeYellSpeedTicks);
	interface->registerMethod("MonsterType", "changeTargetChance", luaMonsterTypeChangeTargetChance);
	interface->registerMethod("MonsterType", "changeTargetSpeed", luaMonsterTypeChangeTargetSpeed);
}
} // namespace LuaMonsterType

// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "luascript.h"
#include "spells.h"
#include "vocation.h"

extern Spells* g_spells;
extern Vocations g_vocations;

using namespace Lua;

static int luaSpellCreate(lua_State* L)
{
	// Spell(words or name) to get an existing spell
	// Spell(type) ex: Spell(SPELL_INSTANT) or Spell(SPELL_RUNE) to create a new spell
	if (lua_gettop(L) == 1) {
		std::cout << "[Error - luaSpellCreate] There is no parameter set!" << std::endl;
		lua_pushnil(L);
		return 1;
	}

	if (isString(L, 2)) {
		const std::string& arg = getString(L, 2);
		InstantSpell* raw = g_spells->getInstantSpellByName(arg);
		Spell_shared_ptr instant(raw);
		if (instant) {
			pushSharedPtr<Spell_shared_ptr>(L, instant);
			setMetatable(L, -1, "Spell");
			return 1;
		}
		raw = g_spells->getInstantSpell(arg);
		Spell_shared_ptr instant2(raw);
		if (instant2) {
			pushSharedPtr<Spell_shared_ptr>(L, instant2);
			setMetatable(L, -1, "Spell");
			return 1;
		}
		RuneSpell* rawRune = g_spells->getRuneSpellByName(arg);
		Spell_shared_ptr rune(rawRune);
		if (rune) {
			pushSharedPtr<Spell_shared_ptr>(L, rune);
			setMetatable(L, -1, "Spell");
			return 1;
		}
	}

	SpellType_t spellType = getNumber<SpellType_t>(L, 2);

	if (spellType == SPELL_INSTANT) {
		InstantSpell* raw = new InstantSpell(LuaScriptInterface::getScriptEnv()->getScriptInterface());
		Spell_shared_ptr spell(raw);
		raw->fromLua = true;
		spell->spellType = SPELL_INSTANT;
		pushSharedPtr<Spell_shared_ptr>(L, spell);
		setMetatable(L, -1, "Spell");
		return 1;
	} else if (spellType == SPELL_RUNE) {
		RuneSpell* raw = new RuneSpell(LuaScriptInterface::getScriptEnv()->getScriptInterface());
		Spell_shared_ptr rune(raw);
		raw->fromLua = true;
		rune->spellType = SPELL_RUNE;
		pushSharedPtr<Spell_shared_ptr>(L, rune);
		setMetatable(L, -1, "Spell");
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

static int luaSpellOnCastSpell(lua_State* L)
{
	// spell:onCastSpell(callback)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		const std::string& functionName = getString(L, 2);
		bool fileName = functionName == "onCastSpell" ? true : false;
		if (spell->spellType == SPELL_INSTANT) {
			InstantSpell* instant = static_cast<InstantSpell*>(spell.get());
			if (!instant->loadCallback(functionName, fileName)) {
				pushBoolean(L, false);
				return 1;
			}
			instant->scripted = true;
			pushBoolean(L, true);
		} else if (spell->spellType == SPELL_RUNE) {
			RuneSpell* rune = static_cast<RuneSpell*>(spell.get());
			if (!rune->loadCallback(functionName, fileName)) {
				pushBoolean(L, false);
				return 1;
			}
			rune->scripted = true;
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellRegister(lua_State* L)
{
	// spell:register()
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (spell->spellType == SPELL_INSTANT) {
			InstantSpell* instant = static_cast<InstantSpell*>(spell.get());
			if (!instant->isScripted()) {
				pushBoolean(L, false);
				return 1;
			}
			pushBoolean(L, g_spells->registerInstantLuaEvent(spell));
		} else if (spell->spellType == SPELL_RUNE) {
			RuneSpell* rune = static_cast<RuneSpell*>(spell.get());
			if (rune->getMagicLevel() != 0 || rune->getLevel() != 0) {
				// Change information in the ItemType to get accurate description
				ItemType& iType = Item::items.getItemType(rune->getRuneItemId());
				iType.name = rune->getName();
				iType.runeMagLevel = rune->getMagicLevel();
				iType.runeLevel = rune->getLevel();
				iType.charges = rune->getCharges();
			}
			if (!rune->isScripted()) {
				pushBoolean(L, false);
				return 1;
			}
			pushBoolean(L, g_spells->registerRuneLuaEvent(spell));
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellName(lua_State* L)
{
	// spell:name(name)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			pushString(L, spell->getName());
		} else {
			spell->setName(getString(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellId(lua_State* L)
{
	// spell:id(id)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, spell->getId());
		} else {
			spell->setId(getNumber<uint8_t>(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellGroup(lua_State* L)
{
	// spell:group(primaryGroup[, secondaryGroup])
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, spell->getGroup());
			lua_pushnumber(L, spell->getSecondaryGroup());
			return 2;
		} else if (lua_gettop(L) == 2) {
			SpellGroup_t group = getNumber<SpellGroup_t>(L, 2);
			if (group) {
				spell->setGroup(group);
				pushBoolean(L, true);
			} else if (isString(L, 2)) {
				group = stringToSpellGroup(getString(L, 2));
				if (group != SPELLGROUP_NONE) {
					spell->setGroup(group);
				} else {
					std::cout << "[Warning - Spell::group] Unknown group: " << getString(L, 2) << std::endl;
					pushBoolean(L, false);
					return 1;
				}
				pushBoolean(L, true);
			} else {
				std::cout << "[Warning - Spell::group] Unknown group: " << getString(L, 2) << std::endl;
				pushBoolean(L, false);
				return 1;
			}
		} else {
			SpellGroup_t primaryGroup = getNumber<SpellGroup_t>(L, 2);
			SpellGroup_t secondaryGroup = getNumber<SpellGroup_t>(L, 2);
			if (primaryGroup && secondaryGroup) {
				spell->setGroup(primaryGroup);
				spell->setSecondaryGroup(secondaryGroup);
				pushBoolean(L, true);
			} else if (isString(L, 2) && isString(L, 3)) {
				primaryGroup = stringToSpellGroup(getString(L, 2));
				if (primaryGroup != SPELLGROUP_NONE) {
					spell->setGroup(primaryGroup);
				} else {
					std::cout << "[Warning - Spell::group] Unknown primaryGroup: " << getString(L, 2) << std::endl;
					pushBoolean(L, false);
					return 1;
				}
				secondaryGroup = stringToSpellGroup(getString(L, 3));
				if (secondaryGroup != SPELLGROUP_NONE) {
					spell->setSecondaryGroup(secondaryGroup);
				} else {
					std::cout << "[Warning - Spell::group] Unknown secondaryGroup: " << getString(L, 3) << std::endl;
					pushBoolean(L, false);
					return 1;
				}
				pushBoolean(L, true);
			} else {
				std::cout << "[Warning - Spell::group] Unknown primaryGroup: " << getString(L, 2)
				          << " or secondaryGroup: " << getString(L, 3) << std::endl;
				pushBoolean(L, false);
				return 1;
			}
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellCooldown(lua_State* L)
{
	// spell:cooldown(cooldown)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, spell->getCooldown());
		} else {
			spell->setCooldown(getNumber<uint32_t>(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellGroupCooldown(lua_State* L)
{
	// spell:groupCooldown(primaryGroupCd[, secondaryGroupCd])
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, spell->getGroupCooldown());
			lua_pushnumber(L, spell->getSecondaryCooldown());
			return 2;
		} else if (lua_gettop(L) == 2) {
			spell->setGroupCooldown(getNumber<uint32_t>(L, 2));
			pushBoolean(L, true);
		} else {
			spell->setGroupCooldown(getNumber<uint32_t>(L, 2));
			spell->setSecondaryCooldown(getNumber<uint32_t>(L, 3));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellLevel(lua_State* L)
{
	// spell:level(lvl)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, spell->getLevel());
		} else {
			spell->setLevel(getNumber<uint32_t>(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellMagicLevel(lua_State* L)
{
	// spell:magicLevel(lvl)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, spell->getMagicLevel());
		} else {
			spell->setMagicLevel(getNumber<uint32_t>(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellMana(lua_State* L)
{
	// spell:mana(mana)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, spell->getMana());
		} else {
			spell->setMana(getNumber<uint32_t>(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellManaPercent(lua_State* L)
{
	// spell:manaPercent(percent)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, spell->getManaPercent());
		} else {
			spell->setManaPercent(getNumber<uint32_t>(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellSoul(lua_State* L)
{
	// spell:soul(soul)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, spell->getSoulCost());
		} else {
			spell->setSoulCost(getNumber<uint32_t>(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellRange(lua_State* L)
{
	// spell:range(range)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, spell->getRange());
		} else {
			spell->setRange(getNumber<int32_t>(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellPremium(lua_State* L)
{
	// spell:isPremium(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, spell->isPremium());
		} else {
			spell->setPremium(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellEnabled(lua_State* L)
{
	// spell:isEnabled(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, spell->isEnabled());
		} else {
			spell->setEnabled(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellNeedTarget(lua_State* L)
{
	// spell:needTarget(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, spell->getNeedTarget());
		} else {
			spell->setNeedTarget(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellNeedWeapon(lua_State* L)
{
	// spell:needWeapon(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, spell->getNeedWeapon());
		} else {
			spell->setNeedWeapon(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellNeedLearn(lua_State* L)
{
	// spell:needLearn(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, spell->getNeedLearn());
		} else {
			spell->setNeedLearn(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellSelfTarget(lua_State* L)
{
	// spell:isSelfTarget(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, spell->getSelfTarget());
		} else {
			spell->setSelfTarget(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellBlocking(lua_State* L)
{
	// spell:isBlocking(blockingSolid, blockingCreature)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, spell->getBlockingSolid());
			pushBoolean(L, spell->getBlockingCreature());
			return 2;
		} else {
			spell->setBlockingSolid(getBoolean(L, 2));
			spell->setBlockingCreature(getBoolean(L, 3));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellAggressive(lua_State* L)
{
	// spell:isAggressive(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, spell->getAggressive());
		} else {
			spell->setAggressive(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellPzLock(lua_State* L)
{
	// spell:isPzLock(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		if (lua_gettop(L) == 1) {
			pushBoolean(L, spell->getPzLock());
		} else {
			spell->setPzLock(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaSpellVocation(lua_State* L)
{
	// spell:vocation(vocation)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (!spell) {
		lua_pushnil(L);
		return 1;
	}

	if (lua_gettop(L) == 1) {
		lua_createtable(L, 0, 0);
		int i = 0;
		for (auto& voc : spell->getVocMap()) {
			const std::string& name = g_vocations.getVocation(voc.first)->getVocName();
			pushString(L, name);
			lua_rawseti(L, -2, ++i);
		}
	} else {
		int parameters = lua_gettop(L) - 1; // - 1 because self is a parameter aswell, which we want to skip ofc
		for (int i = 0; i < parameters; ++i) {
			std::vector<std::string> vocList = explodeString(getString(L, 2 + i), ";");
			spell->addVocMap(g_vocations.getVocationId(vocList[0]),
			                 vocList.size() > 1 ? booleanString(vocList[1]) : false);
		}
		pushBoolean(L, true);
	}
	return 1;
}

// only for InstantSpells
static int luaSpellWords(lua_State* L)
{
	// spell:words(words[, separator = ""])
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		// if spell != SPELL_INSTANT, it means that this actually is no InstantSpell, so we return nil
		if (spell->spellType != SPELL_INSTANT) {
			lua_pushnil(L);
			return 1;
		}

		InstantSpell* instant = static_cast<InstantSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			pushString(L, instant->getWords());
			pushString(L, instant->getSeparator());
			return 2;
		} else {
			std::string sep = "";
			if (lua_gettop(L) == 3) {
				sep = getString(L, 3);
			}
			instant->setWords(getString(L, 2));
			instant->setSeparator(sep);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// only for InstantSpells
static int luaSpellNeedDirection(lua_State* L)
{
	// spell:needDirection(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		// if spell != SPELL_INSTANT, it means that this actually is no InstantSpell, so we return nil
		if (spell->spellType != SPELL_INSTANT) {
			lua_pushnil(L);
			return 1;
		}

		InstantSpell* instant = static_cast<InstantSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			pushBoolean(L, instant->getNeedDirection());
		} else {
			instant->setNeedDirection(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// only for InstantSpells
static int luaSpellHasParams(lua_State* L)
{
	// spell:hasParams(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		// if spell != SPELL_INSTANT, it means that this actually is no InstantSpell, so we return nil
		if (spell->spellType != SPELL_INSTANT) {
			lua_pushnil(L);
			return 1;
		}

		InstantSpell* instant = static_cast<InstantSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			pushBoolean(L, instant->getHasParam());
		} else {
			instant->setHasParam(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// only for InstantSpells
static int luaSpellHasPlayerNameParam(lua_State* L)
{
	// spell:hasPlayerNameParam(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		// if spell != SPELL_INSTANT, it means that this actually is no InstantSpell, so we return nil
		if (spell->spellType != SPELL_INSTANT) {
			lua_pushnil(L);
			return 1;
		}

		InstantSpell* instant = static_cast<InstantSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			pushBoolean(L, instant->getHasPlayerNameParam());
		} else {
			instant->setHasPlayerNameParam(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// only for InstantSpells
static int luaSpellNeedCasterTargetOrDirection(lua_State* L)
{
	// spell:needCasterTargetOrDirection(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		// if spell != SPELL_INSTANT, it means that this actually is no InstantSpell, so we return nil
		if (spell->spellType != SPELL_INSTANT) {
			lua_pushnil(L);
			return 1;
		}

		InstantSpell* instant = static_cast<InstantSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			pushBoolean(L, instant->getNeedCasterTargetOrDirection());
		} else {
			instant->setNeedCasterTargetOrDirection(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// only for InstantSpells
static int luaSpellIsBlockingWalls(lua_State* L)
{
	// spell:blockWalls(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		// if spell != SPELL_INSTANT, it means that this actually is no InstantSpell, so we return nil
		if (spell->spellType != SPELL_INSTANT) {
			lua_pushnil(L);
			return 1;
		}

		InstantSpell* instant = static_cast<InstantSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			pushBoolean(L, instant->getBlockWalls());
		} else {
			instant->setBlockWalls(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// only for RuneSpells
static int luaSpellRuneLevel(lua_State* L)
{
	// spell:runeLevel(level)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	int32_t level = getNumber<int32_t>(L, 2);
	if (spell) {
		// if spell != SPELL_RUNE, it means that this actually is no RuneSpell, so we return nil
		if (spell->spellType != SPELL_RUNE) {
			lua_pushnil(L);
			return 1;
		}

		RuneSpell* rune = static_cast<RuneSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, rune->getLevel());
		} else {
			rune->setLevel(level);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// only for RuneSpells
static int luaSpellRuneMagicLevel(lua_State* L)
{
	// spell:runeMagicLevel(magLevel)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	int32_t magLevel = getNumber<int32_t>(L, 2);
	if (spell) {
		// if spell != SPELL_RUNE, it means that this actually is no RuneSpell, so we return nil
		if (spell->spellType != SPELL_RUNE) {
			lua_pushnil(L);
			return 1;
		}

		RuneSpell* rune = static_cast<RuneSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, rune->getMagicLevel());
		} else {
			rune->setMagicLevel(magLevel);
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// only for RuneSpells
static int luaSpellRuneId(lua_State* L)
{
	// spell:runeId(id)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		// if spell != SPELL_RUNE, it means that this actually is no RuneSpell, so we return nil
		if (spell->spellType != SPELL_RUNE) {
			lua_pushnil(L);
			return 1;
		}

		RuneSpell* rune = static_cast<RuneSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, rune->getRuneItemId());
		} else {
			rune->setRuneItemId(getNumber<uint16_t>(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// only for RuneSpells
static int luaSpellCharges(lua_State* L)
{
	// spell:charges(charges)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		// if spell != SPELL_RUNE, it means that this actually is no RuneSpell, so we return nil
		if (spell->spellType != SPELL_RUNE) {
			lua_pushnil(L);
			return 1;
		}

		RuneSpell* rune = static_cast<RuneSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			lua_pushnumber(L, rune->getCharges());
		} else {
			rune->setCharges(getNumber<uint32_t>(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// only for RuneSpells
static int luaSpellAllowFarUse(lua_State* L)
{
	// spell:allowFarUse(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		// if spell != SPELL_RUNE, it means that this actually is no RuneSpell, so we return nil
		if (spell->spellType != SPELL_RUNE) {
			lua_pushnil(L);
			return 1;
		}

		RuneSpell* rune = static_cast<RuneSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			pushBoolean(L, rune->getAllowFarUse());
		} else {
			rune->setAllowFarUse(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// only for RuneSpells
static int luaSpellBlockWalls(lua_State* L)
{
	// spell:blockWalls(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		// if spell != SPELL_RUNE, it means that this actually is no RuneSpell, so we return nil
		if (spell->spellType != SPELL_RUNE) {
			lua_pushnil(L);
			return 1;
		}

		RuneSpell* rune = static_cast<RuneSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			pushBoolean(L, rune->getCheckLineOfSight());
		} else {
			rune->setCheckLineOfSight(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// only for RuneSpells
static int luaSpellCheckFloor(lua_State* L)
{
	// spell:checkFloor(bool)
	Spell_shared_ptr spell = getSharedPtr<Spell>(L, 1);
	if (spell) {
		// if spell != SPELL_RUNE, it means that this actually is no RuneSpell, so we return nil
		if (spell->spellType != SPELL_RUNE) {
			lua_pushnil(L);
			return 1;
		}

		RuneSpell* rune = static_cast<RuneSpell*>(spell.get());

		if (lua_gettop(L) == 1) {
			pushBoolean(L, rune->getCheckFloor());
		} else {
			rune->setCheckFloor(getBoolean(L, 2));
			pushBoolean(L, true);
		}
	} else {
		lua_pushnil(L);
	}
	return 1;
}

namespace LuaSpell {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("Spell", "", luaSpellCreate);
	interface->registerMetaMethod("Spell", "__eq", interface->luaUserdataCompare);

	interface->registerMethod("Spell", "onCastSpell", luaSpellOnCastSpell);
	interface->registerMethod("Spell", "register", luaSpellRegister);
	interface->registerMethod("Spell", "name", luaSpellName);
	interface->registerMethod("Spell", "id", luaSpellId);
	interface->registerMethod("Spell", "group", luaSpellGroup);
	interface->registerMethod("Spell", "cooldown", luaSpellCooldown);
	interface->registerMethod("Spell", "groupCooldown", luaSpellGroupCooldown);
	interface->registerMethod("Spell", "level", luaSpellLevel);
	interface->registerMethod("Spell", "magicLevel", luaSpellMagicLevel);
	interface->registerMethod("Spell", "mana", luaSpellMana);
	interface->registerMethod("Spell", "manaPercent", luaSpellManaPercent);
	interface->registerMethod("Spell", "soul", luaSpellSoul);
	interface->registerMethod("Spell", "range", luaSpellRange);
	interface->registerMethod("Spell", "isPremium", luaSpellPremium);
	interface->registerMethod("Spell", "isEnabled", luaSpellEnabled);
	interface->registerMethod("Spell", "needTarget", luaSpellNeedTarget);
	interface->registerMethod("Spell", "needWeapon", luaSpellNeedWeapon);
	interface->registerMethod("Spell", "needLearn", luaSpellNeedLearn);
	interface->registerMethod("Spell", "isSelfTarget", luaSpellSelfTarget);
	interface->registerMethod("Spell", "isBlocking", luaSpellBlocking);
	interface->registerMethod("Spell", "isAggressive", luaSpellAggressive);
	interface->registerMethod("Spell", "isPzLock", luaSpellPzLock);
	interface->registerMethod("Spell", "vocation", luaSpellVocation);

	// only for InstantSpell
	interface->registerMethod("Spell", "words", luaSpellWords);
	interface->registerMethod("Spell", "needDirection", luaSpellNeedDirection);
	interface->registerMethod("Spell", "hasParams", luaSpellHasParams);
	interface->registerMethod("Spell", "hasPlayerNameParam", luaSpellHasPlayerNameParam);
	interface->registerMethod("Spell", "needCasterTargetOrDirection", luaSpellNeedCasterTargetOrDirection);
	interface->registerMethod("Spell", "isBlockingWalls", luaSpellIsBlockingWalls);

	// only for RuneSpells
	interface->registerMethod("Spell", "runeLevel", luaSpellRuneLevel);
	interface->registerMethod("Spell", "runeMagicLevel", luaSpellRuneMagicLevel);
	interface->registerMethod("Spell", "runeId", luaSpellRuneId);
	interface->registerMethod("Spell", "charges", luaSpellCharges);
	interface->registerMethod("Spell", "allowFarUse", luaSpellAllowFarUse);
	interface->registerMethod("Spell", "blockWalls", luaSpellBlockWalls);
	interface->registerMethod("Spell", "checkFloor", luaSpellCheckFloor);
}
} // namespace LuaSpell

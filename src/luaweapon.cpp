// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "luascript.h"
#include "script.h"
#include "weapons.h"

extern Scripts* g_scripts;
extern Weapons* g_weapons;

using namespace Lua;

static int luaCreateWeapon(lua_State* L)
{
	// Weapon(type)
	if (LuaScriptInterface::getScriptEnv()->getScriptInterface() != &g_scripts->getScriptInterface()) {
		reportErrorFunc(L, "Weapons can only be registered in the Scripts interface.");
		lua_pushnil(L);
		return 1;
	}

	WeaponType_t type = getNumber<WeaponType_t>(L, 2);
	switch (type) {
		case WEAPON_SWORD:
		case WEAPON_AXE:
		case WEAPON_CLUB: {
			auto weapon = std::make_shared<WeaponMelee>(LuaScriptInterface::getScriptEnv()->getScriptInterface());
			if (weapon) {
				weapon->weaponType = type;
				pushSharedPtr<Weapon_shared_ptr>(L, weapon);
				setMetatable(L, -1, "Weapon");
			} else {
				lua_pushnil(L);
			}
			break;
		}
		case WEAPON_DISTANCE:
		case WEAPON_AMMO: {
			auto weapon = std::make_shared<WeaponDistance>(LuaScriptInterface::getScriptEnv()->getScriptInterface());
			if (weapon) {
				weapon->weaponType = type;
				pushSharedPtr<Weapon_shared_ptr>(L, weapon);
				setMetatable(L, -1, "Weapon");
			} else {
				lua_pushnil(L);
			}
			break;
		}
		case WEAPON_WAND: {
			auto weapon = std::make_shared<WeaponWand>(LuaScriptInterface::getScriptEnv()->getScriptInterface());
			if (weapon) {
				weapon->weaponType = type;
				pushSharedPtr<Weapon_shared_ptr>(L, weapon);
				setMetatable(L, -1, "Weapon");
			} else {
				lua_pushnil(L);
			}
			break;
		}
		default: {
			lua_pushnil(L);
			break;
		}
	}
	return 1;
}

static int luaWeaponType(lua_State* L)
{
	// weapon:weaponType(weaponType)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->weaponType = getNumber<WeaponType_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponAction(lua_State* L)
{
	// weapon:action(callback)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		const std::string& tmpStr = boost::algorithm::to_lower_copy(getString(L, 2));
		if (tmpStr == "removecount") {
			weapon->action = WEAPONACTION_REMOVECOUNT;
		} else if (tmpStr == "removecharge") {
			weapon->action = WEAPONACTION_REMOVECHARGE;
		} else if (tmpStr == "move") {
			weapon->action = WEAPONACTION_MOVE;
		} else {
			std::cout << "Error: [luaWeaponAction] No valid action " << tmpStr << std::endl;
			pushBoolean(L, false);
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponRegister(lua_State* L)
{
	// weapon:register()
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (!weapon) {
		lua_pushnil(L);
		return 1;
	}

	uint16_t id = weapon->getID();
	ItemType& it = Item::items.getItemType(id);
	it.weaponType = weapon->weaponType;

	if (weapon->getWieldInfo() != 0) {
		it.wieldInfo = weapon->getWieldInfo();
		it.vocationString = weapon->getVocationString();
		it.minReqLevel = weapon->getReqLevel();
		it.minReqMagicLevel = weapon->getReqMagLv();
	}

	weapon->configureWeapon(it);
	pushBoolean(L, g_weapons->registerLuaEvent(weapon));

	return 1;
}

static int luaWeaponOnUseWeapon(lua_State* L)
{
	// weapon:onUseWeapon(callback)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		const std::string& functionName = getString(L, 2);
		bool fileName = functionName == "onUseWeapon" ? true : false;
		if (!weapon->loadCallback(functionName, fileName)) {
			pushBoolean(L, false);
			return 1;
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponUnproperly(lua_State* L)
{
	// weapon:wieldUnproperly(bool)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->setWieldUnproperly(getBoolean(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponLevel(lua_State* L)
{
	// weapon:level(lvl)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->setRequiredLevel(getNumber<uint32_t>(L, 2));
		weapon->setWieldInfo(WIELDINFO_LEVEL);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponMagicLevel(lua_State* L)
{
	// weapon:magicLevel(lvl)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->setRequiredMagLevel(getNumber<uint32_t>(L, 2));
		weapon->setWieldInfo(WIELDINFO_MAGLV);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponMana(lua_State* L)
{
	// weapon:mana(mana)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->setMana(getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponManaPercent(lua_State* L)
{
	// weapon:manaPercent(percent)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->setManaPercent(getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponHealth(lua_State* L)
{
	// weapon:health(health)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->setHealth(getNumber<int32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponHealthPercent(lua_State* L)
{
	// weapon:healthPercent(percent)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->setHealthPercent(getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponSoul(lua_State* L)
{
	// weapon:soul(soul)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->setSoul(getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponBreakChance(lua_State* L)
{
	// weapon:breakChance(percent)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->setBreakChance(getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponWandDamage(lua_State* L)
{
	// weapon:damage(damage[min, max]) only use this if the weapon is a wand!
	auto weapon = std::static_pointer_cast<WeaponWand>(getSharedPtr<Weapon>(L, 1));
	if (weapon) {
		weapon->setMinChange(getNumber<uint32_t>(L, 2));
		if (lua_gettop(L) > 2) {
			weapon->setMaxChange(getNumber<uint32_t>(L, 3));
		} else {
			weapon->setMaxChange(getNumber<uint32_t>(L, 2));
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponElement(lua_State* L)
{
	// weapon:element(combatType)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		if (!getNumber<CombatType_t>(L, 2)) {
			const std::string& tmpStrValue = boost::algorithm::to_lower_copy(getString(L, 2));
			if (tmpStrValue == "earth") {
				weapon->params.combatType = COMBAT_EARTHDAMAGE;
			} else if (tmpStrValue == "ice") {
				weapon->params.combatType = COMBAT_ICEDAMAGE;
			} else if (tmpStrValue == "energy") {
				weapon->params.combatType = COMBAT_ENERGYDAMAGE;
			} else if (tmpStrValue == "fire") {
				weapon->params.combatType = COMBAT_FIREDAMAGE;
			} else if (tmpStrValue == "death") {
				weapon->params.combatType = COMBAT_DEATHDAMAGE;
			} else if (tmpStrValue == "holy") {
				weapon->params.combatType = COMBAT_HOLYDAMAGE;
			} else {
				std::cout << "[Warning - luaWeaponElement] Type \"" << tmpStrValue << "\" does not exist." << std::endl;
			}
		} else {
			weapon->params.combatType = getNumber<CombatType_t>(L, 2);
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponPremium(lua_State* L)
{
	// weapon:premium(bool)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->setNeedPremium(getBoolean(L, 2));
		weapon->setWieldInfo(WIELDINFO_PREMIUM);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponVocation(lua_State* L)
{
	// weapon:vocation(vocName[, showInDescription = false, lastVoc = false])
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->addVocWeaponMap(getString(L, 2));
		weapon->setWieldInfo(WIELDINFO_VOCREQ);
		std::string tmp;
		bool showInDescription = getBoolean(L, 3, false);
		bool lastVoc = getBoolean(L, 4, false);

		if (showInDescription) {
			if (weapon->getVocationString().empty()) {
				tmp = boost::algorithm::to_lower_copy(getString(L, 2));
				tmp += "s";
				weapon->setVocationString(tmp);
			} else {
				tmp = weapon->getVocationString();
				if (lastVoc) {
					tmp += " and ";
				} else {
					tmp += ", ";
				}
				tmp += boost::algorithm::to_lower_copy(getString(L, 2));
				tmp += "s";
				weapon->setVocationString(tmp);
			}
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponId(lua_State* L)
{
	// weapon:id(id)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		weapon->setID(getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponAttack(lua_State* L)
{
	// weapon:attack(atk)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);
		it.attack = getNumber<int32_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponDefense(lua_State* L)
{
	// weapon:defense(defense[, extraDefense])
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);
		it.defense = getNumber<int32_t>(L, 2);
		if (lua_gettop(L) > 2) {
			it.extraDefense = getNumber<int32_t>(L, 3);
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponRange(lua_State* L)
{
	// weapon:range(range)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);
		it.shootRange = getNumber<uint8_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponCharges(lua_State* L)
{
	// weapon:charges(charges[, showCharges = true])
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		bool showCharges = getBoolean(L, 3, true);
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);

		it.charges = getNumber<uint8_t>(L, 2);
		it.showCharges = showCharges;
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponDuration(lua_State* L)
{
	// weapon:duration(duration[, showDuration = true])
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		bool showDuration = getBoolean(L, 3, true);
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);

		it.decayTime = getNumber<uint32_t>(L, 2);
		it.showDuration = showDuration;
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponDecayTo(lua_State* L)
{
	// weapon:decayTo([itemid = 0])
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		uint16_t itemid = getNumber<uint16_t>(L, 2, 0);
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);

		it.decayTo = itemid;
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponTransformEquipTo(lua_State* L)
{
	// weapon:transformEquipTo(itemid)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);
		it.transformEquipTo = getNumber<uint16_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponTransformDeEquipTo(lua_State* L)
{
	// weapon:transformDeEquipTo(itemid)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);
		it.transformDeEquipTo = getNumber<uint16_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponShootType(lua_State* L)
{
	// weapon:shootType(type)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);
		it.shootType = getNumber<ShootType_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponSlotType(lua_State* L)
{
	// weapon:slotType(slot)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);
		const std::string& slot = getString(L, 2);

		if (slot == "two-handed") {
			it.slotPosition |= SLOTP_TWO_HAND;
		} else {
			it.slotPosition |= SLOTP_HAND;
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponAmmoType(lua_State* L)
{
	// weapon:ammoType(type)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);
		const std::string& type = getString(L, 2);

		if (type == "arrow") {
			it.ammoType = AMMO_ARROW;
		} else if (type == "bolt") {
			it.ammoType = AMMO_BOLT;
		} else {
			std::cout << "[Warning - luaWeaponAmmoType] Type \"" << type << "\" does not exist." << std::endl;
			lua_pushnil(L);
			return 1;
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponHitChance(lua_State* L)
{
	// weapon:hitChance(chance)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);
		it.hitChance = getNumber<int8_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponMaxHitChance(lua_State* L)
{
	// weapon:maxHitChance(max)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);
		it.maxHitChance = getNumber<int32_t>(L, 2);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaWeaponExtraElement(lua_State* L)
{
	// weapon:extraElement(atk, combatType)
	Weapon_shared_ptr weapon = getSharedPtr<Weapon>(L, 1);
	if (weapon) {
		uint16_t id = weapon->getID();
		ItemType& it = Item::items.getItemType(id);
		it.abilities.get()->elementDamage = getNumber<uint16_t>(L, 2);

		if (!getNumber<CombatType_t>(L, 3)) {
			const std::string& element = getString(L, 3);
			const std::string& tmpStrValue = boost::algorithm::to_lower_copy(element);
			if (tmpStrValue == "earth") {
				it.abilities.get()->elementType = COMBAT_EARTHDAMAGE;
			} else if (tmpStrValue == "ice") {
				it.abilities.get()->elementType = COMBAT_ICEDAMAGE;
			} else if (tmpStrValue == "energy") {
				it.abilities.get()->elementType = COMBAT_ENERGYDAMAGE;
			} else if (tmpStrValue == "fire") {
				it.abilities.get()->elementType = COMBAT_FIREDAMAGE;
			} else if (tmpStrValue == "death") {
				it.abilities.get()->elementType = COMBAT_DEATHDAMAGE;
			} else if (tmpStrValue == "holy") {
				it.abilities.get()->elementType = COMBAT_HOLYDAMAGE;
			} else {
				std::cout << "[Warning - weapon:extraElement] Type \"" << element << "\" does not exist." << std::endl;
			}
		} else {
			it.abilities.get()->elementType = getNumber<CombatType_t>(L, 3);
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

namespace LuaWeapon {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("Weapon", "", luaCreateWeapon);
	interface->registerMethod("Weapon", "weaponType", luaWeaponType);
	interface->registerMethod("Weapon", "action", luaWeaponAction);
	interface->registerMethod("Weapon", "register", luaWeaponRegister);
	interface->registerMethod("Weapon", "id", luaWeaponId);
	interface->registerMethod("Weapon", "level", luaWeaponLevel);
	interface->registerMethod("Weapon", "magicLevel", luaWeaponMagicLevel);
	interface->registerMethod("Weapon", "mana", luaWeaponMana);
	interface->registerMethod("Weapon", "manaPercent", luaWeaponManaPercent);
	interface->registerMethod("Weapon", "health", luaWeaponHealth);
	interface->registerMethod("Weapon", "healthPercent", luaWeaponHealthPercent);
	interface->registerMethod("Weapon", "soul", luaWeaponSoul);
	interface->registerMethod("Weapon", "breakChance", luaWeaponBreakChance);
	interface->registerMethod("Weapon", "premium", luaWeaponPremium);
	interface->registerMethod("Weapon", "wieldUnproperly", luaWeaponUnproperly);
	interface->registerMethod("Weapon", "vocation", luaWeaponVocation);
	interface->registerMethod("Weapon", "onUseWeapon", luaWeaponOnUseWeapon);
	interface->registerMethod("Weapon", "element", luaWeaponElement);
	interface->registerMethod("Weapon", "attack", luaWeaponAttack);
	interface->registerMethod("Weapon", "defense", luaWeaponDefense);
	interface->registerMethod("Weapon", "range", luaWeaponRange);
	interface->registerMethod("Weapon", "charges", luaWeaponCharges);
	interface->registerMethod("Weapon", "duration", luaWeaponDuration);
	interface->registerMethod("Weapon", "decayTo", luaWeaponDecayTo);
	interface->registerMethod("Weapon", "transformEquipTo", luaWeaponTransformEquipTo);
	interface->registerMethod("Weapon", "transformDeEquipTo", luaWeaponTransformDeEquipTo);
	interface->registerMethod("Weapon", "slotType", luaWeaponSlotType);
	interface->registerMethod("Weapon", "hitChance", luaWeaponHitChance);
	interface->registerMethod("Weapon", "extraElement", luaWeaponExtraElement);

	// exclusively for distance weapons
	interface->registerMethod("Weapon", "ammoType", luaWeaponAmmoType);
	interface->registerMethod("Weapon", "maxHitChance", luaWeaponMaxHitChance);

	// exclusively for wands
	interface->registerMethod("Weapon", "damage", luaWeaponWandDamage);

	// exclusively for wands & distance weapons
	interface->registerMethod("Weapon", "shootType", luaWeaponShootType);
}
} // namespace LuaWeapon

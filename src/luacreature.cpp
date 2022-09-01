// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "condition.h"
#include "creature.h"
#include "creatureevent.h"
#include "events.h"
#include "game.h"
#include "luascript.h"
#include "spectators.h"

extern Game g_game;
extern Events* g_events;

using namespace Lua;

// Creature
static int luaCreatureCreate(lua_State* L)
{
	// Creature(id or name or userdata)
	Creature* creature;
	if (isNumber(L, 2)) {
		creature = g_game.getCreatureByID(getNumber<uint32_t>(L, 2));
	} else if (isString(L, 2)) {
		creature = g_game.getCreatureByName(getString(L, 2));
	} else if (isUserdata(L, 2)) {
		LuaDataType type = getUserdataType(L, 2);
		if (type != LuaData_Player && type != LuaData_Monster && type != LuaData_Npc) {
			lua_pushnil(L);
			return 1;
		}
		creature = getUserdata<Creature>(L, 2);
	} else {
		creature = nullptr;
	}

	if (creature) {
		pushUserdata<Creature>(L, creature);
		setCreatureMetatable(L, -1, creature);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetEvents(lua_State* L)
{
	// creature:getEvents(type)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	CreatureEventType_t eventType = getNumber<CreatureEventType_t>(L, 2);
	const auto& eventList = creature->getCreatureEvents(eventType);
	lua_createtable(L, eventList.size(), 0);

	int index = 0;
	for (CreatureEvent* event : eventList) {
		pushString(L, event->getName());
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaCreatureRegisterEvent(lua_State* L)
{
	// creature:registerEvent(name)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		const std::string& name = getString(L, 2);
		pushBoolean(L, creature->registerCreatureEvent(name));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureUnregisterEvent(lua_State* L)
{
	// creature:unregisterEvent(name)
	const std::string& name = getString(L, 2);
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		pushBoolean(L, creature->unregisterCreatureEvent(name));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureIsRemoved(lua_State* L)
{
	// creature:isRemoved()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		pushBoolean(L, creature->isRemoved());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureIsCreature(lua_State* L)
{
	// creature:isCreature()
	pushBoolean(L, getUserdata<const Creature>(L, 1) != nullptr);
	return 1;
}

static int luaCreatureIsInGhostMode(lua_State* L)
{
	// creature:isInGhostMode()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		pushBoolean(L, creature->isInGhostMode());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureIsHealthHidden(lua_State* L)
{
	// creature:isHealthHidden()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		pushBoolean(L, creature->isHealthHidden());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureIsMovementBlocked(lua_State* L)
{
	// creature:isMovementBlocked()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		pushBoolean(L, creature->isMovementBlocked());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureCanSee(lua_State* L)
{
	// creature:canSee(position)
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		const Position& position = getPosition(L, 2);
		pushBoolean(L, creature->canSee(position));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureCanSeeCreature(lua_State* L)
{
	// creature:canSeeCreature(creature)
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		const Creature* otherCreature = getCreature(L, 2);
		if (!otherCreature) {
			reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			pushBoolean(L, false);
			return 1;
		}

		pushBoolean(L, creature->canSeeCreature(otherCreature));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureCanSeeGhostMode(lua_State* L)
{
	// creature:canSeeGhostMode(creature)
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		const Creature* otherCreature = getCreature(L, 2);
		if (!otherCreature) {
			reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
			pushBoolean(L, false);
			return 1;
		}

		pushBoolean(L, creature->canSeeGhostMode(otherCreature));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureCanSeeInvisibility(lua_State* L)
{
	// creature:canSeeInvisibility()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		pushBoolean(L, creature->canSeeInvisibility());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetParent(lua_State* L)
{
	// creature:getParent()
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	Cylinder* parent = creature->getParent();
	if (!parent) {
		lua_pushnil(L);
		return 1;
	}

	LuaScriptInterface::pushCylinder(L, parent);
	return 1;
}

static int luaCreatureGetId(lua_State* L)
{
	// creature:getId()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		lua_pushnumber(L, creature->getID());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetName(lua_State* L)
{
	// creature:getName()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		pushString(L, creature->getName());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetTarget(lua_State* L)
{
	// creature:getTarget()
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	Creature* target = creature->getAttackedCreature();
	if (target) {
		pushUserdata<Creature>(L, target);
		setCreatureMetatable(L, -1, target);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureSetTarget(lua_State* L)
{
	// creature:setTarget(target)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		pushBoolean(L, creature->setAttackedCreature(getCreature(L, 2)));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetFollowCreature(lua_State* L)
{
	// creature:getFollowCreature()
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	Creature* followCreature = creature->getFollowCreature();
	if (followCreature) {
		pushUserdata<Creature>(L, followCreature);
		setCreatureMetatable(L, -1, followCreature);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureSetFollowCreature(lua_State* L)
{
	// creature:setFollowCreature(followedCreature)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		pushBoolean(L, creature->setFollowCreature(getCreature(L, 2)));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetMaster(lua_State* L)
{
	// creature:getMaster()
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	Creature* master = creature->getMaster();
	if (!master) {
		lua_pushnil(L);
		return 1;
	}

	pushUserdata<Creature>(L, master);
	setCreatureMetatable(L, -1, master);
	return 1;
}

static int luaCreatureSetMaster(lua_State* L)
{
	// creature:setMaster(master)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	pushBoolean(L, creature->setMaster(getCreature(L, 2)));

	// update summon icon
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, creature->getPosition(), true, true);

	for (Creature* spectator : spectators) {
		spectator->getPlayer()->sendUpdateTileCreature(creature);
	}
	return 1;
}

static int luaCreatureGetLight(lua_State* L)
{
	// creature:getLight()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	LightInfo lightInfo = creature->getCreatureLight();
	lua_pushnumber(L, lightInfo.level);
	lua_pushnumber(L, lightInfo.color);
	return 2;
}

static int luaCreatureSetLight(lua_State* L)
{
	// creature:setLight(color, level)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	LightInfo light;
	light.color = getNumber<uint8_t>(L, 2);
	light.level = getNumber<uint8_t>(L, 3);
	creature->setCreatureLight(light);
	g_game.changeLight(creature);
	pushBoolean(L, true);
	return 1;
}

static int luaCreatureGetSpeed(lua_State* L)
{
	// creature:getSpeed()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		lua_pushnumber(L, creature->getSpeed());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetBaseSpeed(lua_State* L)
{
	// creature:getBaseSpeed()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		lua_pushnumber(L, creature->getBaseSpeed());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureChangeSpeed(lua_State* L)
{
	// creature:changeSpeed(delta)
	Creature* creature = getCreature(L, 1);
	if (!creature) {
		reportErrorFunc(L, LuaScriptInterface::getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	int32_t delta = getNumber<int32_t>(L, 2);
	g_game.changeSpeed(creature, delta);
	pushBoolean(L, true);
	return 1;
}

static int luaCreatureSetDropLoot(lua_State* L)
{
	// creature:setDropLoot(doDrop)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		creature->setDropLoot(getBoolean(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureSetSkillLoss(lua_State* L)
{
	// creature:setSkillLoss(skillLoss)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		creature->setSkillLoss(getBoolean(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetPosition(lua_State* L)
{
	// creature:getPosition()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		pushPosition(L, creature->getPosition());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetTile(lua_State* L)
{
	// creature:getTile()
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	Tile* tile = creature->getTile();
	if (tile) {
		pushUserdata<Tile>(L, tile);
		setMetatable(L, -1, "Tile");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetDirection(lua_State* L)
{
	// creature:getDirection()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		lua_pushnumber(L, creature->getDirection());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureSetDirection(lua_State* L)
{
	// creature:setDirection(direction)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		pushBoolean(L, g_game.internalCreatureTurn(creature, getNumber<Direction>(L, 2)));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetHealth(lua_State* L)
{
	// creature:getHealth()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		lua_pushnumber(L, creature->getHealth());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureSetHealth(lua_State* L)
{
	// creature:setHealth(health)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	creature->health = std::min<int32_t>(getNumber<uint32_t>(L, 2), creature->healthMax);
	g_game.addCreatureHealth(creature);

	Player* player = creature->getPlayer();
	if (player) {
		player->sendStats();
	}
	pushBoolean(L, true);
	return 1;
}

static int luaCreatureAddHealth(lua_State* L)
{
	// creature:addHealth(healthChange)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	CombatDamage damage;
	damage.primary.value = getNumber<int32_t>(L, 2);
	if (damage.primary.value >= 0) {
		damage.primary.type = COMBAT_HEALING;
	} else {
		damage.primary.type = COMBAT_UNDEFINEDDAMAGE;
	}
	pushBoolean(L, g_game.combatChangeHealth(nullptr, creature, damage));
	return 1;
}

static int luaCreatureGetMaxHealth(lua_State* L)
{
	// creature:getMaxHealth()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		lua_pushnumber(L, creature->getMaxHealth());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureSetMaxHealth(lua_State* L)
{
	// creature:setMaxHealth(maxHealth)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	creature->healthMax = getNumber<uint32_t>(L, 2);
	creature->health = std::min<int32_t>(creature->health, creature->healthMax);
	g_game.addCreatureHealth(creature);

	Player* player = creature->getPlayer();
	if (player) {
		player->sendStats();
	}
	pushBoolean(L, true);
	return 1;
}

static int luaCreatureSetHiddenHealth(lua_State* L)
{
	// creature:setHiddenHealth(hide)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		creature->setHiddenHealth(getBoolean(L, 2));
		g_game.addCreatureHealth(creature);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureSetMovementBlocked(lua_State* L)
{
	// creature:setMovementBlocked(state)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		creature->setMovementBlocked(getBoolean(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetSkull(lua_State* L)
{
	// creature:getSkull()
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		lua_pushnumber(L, creature->getSkull());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureSetSkull(lua_State* L)
{
	// creature:setSkull(skull)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		creature->setSkull(getNumber<Skulls_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetOutfit(lua_State* L)
{
	// creature:getOutfit()
	const Creature* creature = getUserdata<const Creature>(L, 1);
	if (creature) {
		pushOutfit(L, creature->getCurrentOutfit());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureSetOutfit(lua_State* L)
{
	// creature:setOutfit(outfit)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		creature->defaultOutfit = getOutfit(L, 2);
		g_game.internalCreatureChangeOutfit(creature, creature->defaultOutfit);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetCondition(lua_State* L)
{
	// creature:getCondition(conditionType[, conditionId = CONDITIONID_COMBAT[, subId = 0]])
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	ConditionType_t conditionType = getNumber<ConditionType_t>(L, 2);
	ConditionId_t conditionId = getNumber<ConditionId_t>(L, 3, CONDITIONID_COMBAT);
	uint32_t subId = getNumber<uint32_t>(L, 4, 0);

	Condition* condition = creature->getCondition(conditionType, conditionId, subId);
	if (condition) {
		pushUserdata<Condition>(L, condition);
		setWeakMetatable(L, -1, "Condition");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureAddCondition(lua_State* L)
{
	// creature:addCondition(condition[, force = false])
	Creature* creature = getUserdata<Creature>(L, 1);
	Condition* condition = getUserdata<Condition>(L, 2);
	if (creature && condition) {
		bool force = getBoolean(L, 3, false);
		pushBoolean(L, creature->addCondition(condition->clone(), force));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureRemoveCondition(lua_State* L)
{
	// creature:removeCondition(conditionType[, conditionId = CONDITIONID_COMBAT[, subId = 0[, force = false]]])
	// creature:removeCondition(condition[, force = false])
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	Condition* condition = nullptr;

	bool force = false;

	if (isUserdata(L, 2)) {
		condition = getUserdata<Condition>(L, 2);
		force = getBoolean(L, 3, false);
	} else {
		ConditionType_t conditionType = getNumber<ConditionType_t>(L, 2);
		ConditionId_t conditionId = getNumber<ConditionId_t>(L, 3, CONDITIONID_COMBAT);
		uint32_t subId = getNumber<uint32_t>(L, 4, 0);
		condition = creature->getCondition(conditionType, conditionId, subId);
		force = getBoolean(L, 5, false);
	}

	if (condition) {
		creature->removeCondition(condition, force);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureHasCondition(lua_State* L)
{
	// creature:hasCondition(conditionType[, subId = 0])
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	ConditionType_t conditionType = getNumber<ConditionType_t>(L, 2);
	uint32_t subId = getNumber<uint32_t>(L, 3, 0);
	pushBoolean(L, creature->hasCondition(conditionType, subId));
	return 1;
}

static int luaCreatureIsImmune(lua_State* L)
{
	// creature:isImmune(condition or conditionType)
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	if (isNumber(L, 2)) {
		pushBoolean(L, creature->isImmune(getNumber<ConditionType_t>(L, 2)));
	} else if (Condition* condition = getUserdata<Condition>(L, 2)) {
		pushBoolean(L, creature->isImmune(condition->getType()));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureRemove(lua_State* L)
{
	// creature:remove()
	Creature** creaturePtr = getRawUserdata<Creature>(L, 1);
	if (!creaturePtr) {
		lua_pushnil(L);
		return 1;
	}

	Creature* creature = *creaturePtr;
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	Player* player = creature->getPlayer();
	if (player) {
		player->kickPlayer(true);
	} else {
		g_game.removeCreature(creature);
	}

	*creaturePtr = nullptr;
	pushBoolean(L, true);
	return 1;
}

static int luaCreatureTeleportTo(lua_State* L)
{
	// creature:teleportTo(position[, pushMovement = false])
	bool pushMovement = getBoolean(L, 3, false);

	const Position& position = getPosition(L, 2);
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	const Position oldPosition = creature->getPosition();
	if (g_game.internalTeleport(creature, position, pushMovement) != RETURNVALUE_NOERROR) {
		pushBoolean(L, false);
		return 1;
	}

	if (pushMovement) {
		if (oldPosition.x == position.x) {
			if (oldPosition.y < position.y) {
				g_game.internalCreatureTurn(creature, DIRECTION_SOUTH);
			} else {
				g_game.internalCreatureTurn(creature, DIRECTION_NORTH);
			}
		} else if (oldPosition.x > position.x) {
			g_game.internalCreatureTurn(creature, DIRECTION_WEST);
		} else if (oldPosition.x < position.x) {
			g_game.internalCreatureTurn(creature, DIRECTION_EAST);
		}
	}
	pushBoolean(L, true);
	return 1;
}

static int luaCreatureSay(lua_State* L)
{
	// creature:say(text[, type = TALKTYPE_MONSTER_SAY[, ghost = false[, target = nullptr[, position]]]])
	int parameters = lua_gettop(L);

	Position position;
	if (parameters >= 6) {
		position = getPosition(L, 6);
		if (!position.x || !position.y) {
			reportErrorFunc(L, "Invalid position specified.");
			pushBoolean(L, false);
			return 1;
		}
	}

	Creature* target = nullptr;
	if (parameters >= 5) {
		target = getCreature(L, 5);
	}

	bool ghost = getBoolean(L, 4, false);

	SpeakClasses type = getNumber<SpeakClasses>(L, 3, TALKTYPE_MONSTER_SAY);
	const std::string& text = getString(L, 2);
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	SpectatorVec spectators;
	if (target) {
		spectators.emplace_back(target);
	}

	// Prevent infinity echo on event onHear
	bool echo =
	    LuaScriptInterface::getScriptEnv()->getScriptId() == g_events->getScriptId(EventInfoId::CREATURE_ONHEAR);

	if (position.x != 0) {
		pushBoolean(L, g_game.internalCreatureSay(creature, type, text, ghost, &spectators, &position, echo));
	} else {
		pushBoolean(L, g_game.internalCreatureSay(creature, type, text, ghost, &spectators, nullptr, echo));
	}
	return 1;
}

static int luaCreatureGetDamageMap(lua_State* L)
{
	// creature:getDamageMap()
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	lua_createtable(L, creature->damageMap.size(), 0);
	for (const auto& damageEntry : creature->damageMap) {
		lua_createtable(L, 0, 2);
		setField(L, "total", damageEntry.second.total);
		setField(L, "ticks", damageEntry.second.ticks);
		lua_rawseti(L, -2, damageEntry.first);
	}
	return 1;
}

static int luaCreatureGetSummons(lua_State* L)
{
	// creature:getSummons()
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	lua_createtable(L, creature->getSummonCount(), 0);

	int index = 0;
	for (Creature* summon : creature->getSummons()) {
		pushUserdata<Creature>(L, summon);
		setCreatureMetatable(L, -1, summon);
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaCreatureGetDescription(lua_State* L)
{
	// creature:getDescription(distance)
	int32_t distance = getNumber<int32_t>(L, 2);
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		pushString(L, creature->getDescription(distance));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureGetPathTo(lua_State* L)
{
	// creature:getPathTo(pos[, minTargetDist = 0[, maxTargetDist = 1[, fullPathSearch = true[, clearSight = true[,
	// maxSearchDist = 0]]]]])
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	const Position& position = getPosition(L, 2);

	FindPathParams fpp;
	fpp.minTargetDist = getNumber<int32_t>(L, 3, 0);
	fpp.maxTargetDist = getNumber<int32_t>(L, 4, 1);
	fpp.fullPathSearch = getBoolean(L, 5, fpp.fullPathSearch);
	fpp.clearSight = getBoolean(L, 6, fpp.clearSight);
	fpp.maxSearchDist = getNumber<int32_t>(L, 7, fpp.maxSearchDist);

	std::vector<Direction> dirList;
	if (creature->getPathTo(position, dirList, fpp)) {
		lua_newtable(L);

		int index = 0;
		for (auto it = dirList.rbegin(); it != dirList.rend(); ++it) {
			lua_pushnumber(L, *it);
			lua_rawseti(L, -2, ++index);
		}
	} else {
		pushBoolean(L, false);
	}
	return 1;
}

static int luaCreatureMove(lua_State* L)
{
	// creature:move(direction)
	// creature:move(tile[, flags = 0])
	Creature* creature = getUserdata<Creature>(L, 1);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	if (isNumber(L, 2)) {
		Direction direction = getNumber<Direction>(L, 2);
		if (direction > DIRECTION_LAST) {
			lua_pushnil(L);
			return 1;
		}
		lua_pushnumber(L, g_game.internalMoveCreature(creature, direction, FLAG_NOLIMIT));
	} else {
		Tile* tile = getUserdata<Tile>(L, 2);
		if (!tile) {
			lua_pushnil(L);
			return 1;
		}
		lua_pushnumber(L, g_game.internalMoveCreature(*creature, *tile, getNumber<uint32_t>(L, 3)));
	}
	return 1;
}

static int luaCreatureGetZone(lua_State* L)
{
	// creature:getZone()
	Creature* creature = getUserdata<Creature>(L, 1);
	if (creature) {
		lua_pushnumber(L, creature->getZone());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

namespace LuaCreature {
static void registerFunctions(LuaScriptInterface* interface)
{
	// Creature
	interface->registerClass("Creature", "", luaCreatureCreate);
	interface->registerMetaMethod("Creature", "__eq", interface->luaUserdataCompare);

	interface->registerMethod("Creature", "getEvents", luaCreatureGetEvents);
	interface->registerMethod("Creature", "registerEvent", luaCreatureRegisterEvent);
	interface->registerMethod("Creature", "unregisterEvent", luaCreatureUnregisterEvent);

	interface->registerMethod("Creature", "isRemoved", luaCreatureIsRemoved);
	interface->registerMethod("Creature", "isCreature", luaCreatureIsCreature);
	interface->registerMethod("Creature", "isInGhostMode", luaCreatureIsInGhostMode);
	interface->registerMethod("Creature", "isHealthHidden", luaCreatureIsHealthHidden);
	interface->registerMethod("Creature", "isMovementBlocked", luaCreatureIsMovementBlocked);
	interface->registerMethod("Creature", "isImmune", luaCreatureIsImmune);

	interface->registerMethod("Creature", "canSee", luaCreatureCanSee);
	interface->registerMethod("Creature", "canSeeCreature", luaCreatureCanSeeCreature);
	interface->registerMethod("Creature", "canSeeGhostMode", luaCreatureCanSeeGhostMode);
	interface->registerMethod("Creature", "canSeeInvisibility", luaCreatureCanSeeInvisibility);

	interface->registerMethod("Creature", "getParent", luaCreatureGetParent);

	interface->registerMethod("Creature", "getId", luaCreatureGetId);
	interface->registerMethod("Creature", "getName", luaCreatureGetName);

	interface->registerMethod("Creature", "getTarget", luaCreatureGetTarget);
	interface->registerMethod("Creature", "setTarget", luaCreatureSetTarget);

	interface->registerMethod("Creature", "getFollowCreature", luaCreatureGetFollowCreature);
	interface->registerMethod("Creature", "setFollowCreature", luaCreatureSetFollowCreature);

	interface->registerMethod("Creature", "getMaster", luaCreatureGetMaster);
	interface->registerMethod("Creature", "setMaster", luaCreatureSetMaster);

	interface->registerMethod("Creature", "getLight", luaCreatureGetLight);
	interface->registerMethod("Creature", "setLight", luaCreatureSetLight);

	interface->registerMethod("Creature", "getSpeed", luaCreatureGetSpeed);
	interface->registerMethod("Creature", "getBaseSpeed", luaCreatureGetBaseSpeed);
	interface->registerMethod("Creature", "changeSpeed", luaCreatureChangeSpeed);

	interface->registerMethod("Creature", "setDropLoot", luaCreatureSetDropLoot);
	interface->registerMethod("Creature", "setSkillLoss", luaCreatureSetSkillLoss);

	interface->registerMethod("Creature", "getPosition", luaCreatureGetPosition);
	interface->registerMethod("Creature", "getTile", luaCreatureGetTile);
	interface->registerMethod("Creature", "getDirection", luaCreatureGetDirection);
	interface->registerMethod("Creature", "setDirection", luaCreatureSetDirection);

	interface->registerMethod("Creature", "getHealth", luaCreatureGetHealth);
	interface->registerMethod("Creature", "setHealth", luaCreatureSetHealth);
	interface->registerMethod("Creature", "addHealth", luaCreatureAddHealth);
	interface->registerMethod("Creature", "getMaxHealth", luaCreatureGetMaxHealth);
	interface->registerMethod("Creature", "setMaxHealth", luaCreatureSetMaxHealth);
	interface->registerMethod("Creature", "setHiddenHealth", luaCreatureSetHiddenHealth);
	interface->registerMethod("Creature", "setMovementBlocked", luaCreatureSetMovementBlocked);

	interface->registerMethod("Creature", "getSkull", luaCreatureGetSkull);
	interface->registerMethod("Creature", "setSkull", luaCreatureSetSkull);

	interface->registerMethod("Creature", "getOutfit", luaCreatureGetOutfit);
	interface->registerMethod("Creature", "setOutfit", luaCreatureSetOutfit);

	interface->registerMethod("Creature", "getCondition", luaCreatureGetCondition);
	interface->registerMethod("Creature", "addCondition", luaCreatureAddCondition);
	interface->registerMethod("Creature", "removeCondition", luaCreatureRemoveCondition);
	interface->registerMethod("Creature", "hasCondition", luaCreatureHasCondition);

	interface->registerMethod("Creature", "remove", luaCreatureRemove);
	interface->registerMethod("Creature", "teleportTo", luaCreatureTeleportTo);
	interface->registerMethod("Creature", "say", luaCreatureSay);

	interface->registerMethod("Creature", "getDamageMap", luaCreatureGetDamageMap);

	interface->registerMethod("Creature", "getSummons", luaCreatureGetSummons);

	interface->registerMethod("Creature", "getDescription", luaCreatureGetDescription);

	interface->registerMethod("Creature", "getPathTo", luaCreatureGetPathTo);
	interface->registerMethod("Creature", "move", luaCreatureMove);

	interface->registerMethod("Creature", "getZone", luaCreatureGetZone);
}
} // namespace LuaCreature

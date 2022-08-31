// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "game.h"
#include "luascript.h"
#include "spectators.h"

extern Game g_game;

using namespace Lua;

// Position
static int luaPositionCreate(lua_State* L)
{
	// Position([x = 0[, y = 0[, z = 0[, stackpos = 0]]]])
	// Position([position])
	if (lua_gettop(L) <= 1) {
		pushPosition(L, Position());
		return 1;
	}

	int32_t stackpos;
	if (isTable(L, 2)) {
		const Position& position = getPosition(L, 2, stackpos);
		pushPosition(L, position, stackpos);
	} else {
		uint16_t x = getNumber<uint16_t>(L, 2, 0);
		uint16_t y = getNumber<uint16_t>(L, 3, 0);
		uint8_t z = getNumber<uint8_t>(L, 4, 0);
		stackpos = getNumber<int32_t>(L, 5, 0);

		pushPosition(L, Position(x, y, z), stackpos);
	}
	return 1;
}

static int luaPositionAdd(lua_State* L)
{
	// positionValue = position + positionEx
	int32_t stackpos;
	const Position& position = getPosition(L, 1, stackpos);

	Position positionEx;
	if (stackpos == 0) {
		positionEx = getPosition(L, 2, stackpos);
	} else {
		positionEx = getPosition(L, 2);
	}

	pushPosition(L, position + positionEx, stackpos);
	return 1;
}

static int luaPositionSub(lua_State* L)
{
	// positionValue = position - positionEx
	int32_t stackpos;
	const Position& position = getPosition(L, 1, stackpos);

	Position positionEx;
	if (stackpos == 0) {
		positionEx = getPosition(L, 2, stackpos);
	} else {
		positionEx = getPosition(L, 2);
	}

	pushPosition(L, position - positionEx, stackpos);
	return 1;
}

static int luaPositionCompare(lua_State* L)
{
	// position == positionEx
	const Position& positionEx = getPosition(L, 2);
	const Position& position = getPosition(L, 1);
	pushBoolean(L, position == positionEx);
	return 1;
}

static int luaPositionGetDistance(lua_State* L)
{
	// position:getDistance(positionEx)
	const Position& positionEx = getPosition(L, 2);
	const Position& position = getPosition(L, 1);
	lua_pushnumber(L, std::max<int32_t>(std::max<int32_t>(std::abs(Position::getDistanceX(position, positionEx)),
	                                                      std::abs(Position::getDistanceY(position, positionEx))),
	                                    std::abs(Position::getDistanceZ(position, positionEx))));
	return 1;
}

static int luaPositionIsSightClear(lua_State* L)
{
	// position:isSightClear(positionEx[, sameFloor = true])
	bool sameFloor = getBoolean(L, 3, true);
	const Position& positionEx = getPosition(L, 2);
	const Position& position = getPosition(L, 1);
	pushBoolean(L, g_game.isSightClear(position, positionEx, sameFloor));
	return 1;
}

static int luaPositionSendMagicEffect(lua_State* L)
{
	// position:sendMagicEffect(magicEffect[, player = nullptr])
	SpectatorVec spectators;
	if (lua_gettop(L) >= 3) {
		Player* player = getPlayer(L, 3);
		if (player) {
			spectators.emplace_back(player);
		}
	}

	MagicEffectClasses magicEffect = getNumber<MagicEffectClasses>(L, 2);
	if (magicEffect == CONST_ME_NONE) {
		pushBoolean(L, false);
		return 1;
	}

	const Position& position = getPosition(L, 1);
	if (!spectators.empty()) {
		Game::addMagicEffect(spectators, position, magicEffect);
	} else {
		g_game.addMagicEffect(position, magicEffect);
	}

	pushBoolean(L, true);
	return 1;
}

static int luaPositionSendDistanceEffect(lua_State* L)
{
	// position:sendDistanceEffect(positionEx, distanceEffect[, player = nullptr])
	SpectatorVec spectators;
	if (lua_gettop(L) >= 4) {
		Player* player = getPlayer(L, 4);
		if (player) {
			spectators.emplace_back(player);
		}
	}

	ShootType_t distanceEffect = getNumber<ShootType_t>(L, 3);
	const Position& positionEx = getPosition(L, 2);
	const Position& position = getPosition(L, 1);
	if (!spectators.empty()) {
		Game::addDistanceEffect(spectators, position, positionEx, distanceEffect);
	} else {
		g_game.addDistanceEffect(position, positionEx, distanceEffect);
	}

	pushBoolean(L, true);
	return 1;
}

namespace LuaPosition {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("Position", "", luaPositionCreate);
	interface->registerMetaMethod("Position", "__add", luaPositionAdd);
	interface->registerMetaMethod("Position", "__sub", luaPositionSub);
	interface->registerMetaMethod("Position", "__eq", luaPositionCompare);

	interface->registerMethod("Position", "getDistance", luaPositionGetDistance);
	interface->registerMethod("Position", "isSightClear", luaPositionIsSightClear);

	interface->registerMethod("Position", "sendMagicEffect", luaPositionSendMagicEffect);
	interface->registerMethod("Position", "sendDistanceEffect", luaPositionSendDistanceEffect);
}
} // namespace LuaPosition

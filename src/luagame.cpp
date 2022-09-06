// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "configmanager.h"
#include "creature.h"
#include "events.h"
#include "game.h"
#include "luascript.h"
#include "monster.h"
#include "monsters.h"
#include "npc.h"
#include "outfit.h"
#include "script.h"
#include "spectators.h"

extern ConfigManager g_config;
extern Monsters g_monsters;
extern Events* g_events;
extern Scripts* g_scripts;
extern LuaEnvironment g_luaEnvironment;

using namespace Lua;

// Game
static int luaGameGetSpectators(lua_State* L)
{
	// Game.getSpectators(position[, multifloor = false[, onlyPlayer = false[, minRangeX = 0[, maxRangeX = 0[, minRangeY
	// = 0[, maxRangeY = 0]]]]]])
	const Position& position = getPosition(L, 1);
	bool multifloor = getBoolean(L, 2, false);
	bool onlyPlayers = getBoolean(L, 3, false);
	int32_t minRangeX = getNumber<int32_t>(L, 4, 0);
	int32_t maxRangeX = getNumber<int32_t>(L, 5, 0);
	int32_t minRangeY = getNumber<int32_t>(L, 6, 0);
	int32_t maxRangeY = getNumber<int32_t>(L, 7, 0);

	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, position, multifloor, onlyPlayers, minRangeX, maxRangeX, minRangeY, maxRangeY);

	lua_createtable(L, spectators.size(), 0);

	int index = 0;
	for (Creature* creature : spectators) {
		pushUserdata<Creature>(L, creature);
		setCreatureMetatable(L, -1, creature);
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaGameGetPlayers(lua_State* L)
{
	// Game.getPlayers()
	lua_createtable(L, g_game.getPlayersOnline(), 0);

	int index = 0;
	for (const auto& playerEntry : g_game.getPlayers()) {
		pushUserdata<Player>(L, playerEntry.second);
		setMetatable(L, -1, "Player");
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaGameGetNpcs(lua_State* L)
{
	// Game.getNpcs()
	lua_createtable(L, g_game.getNpcsOnline(), 0);

	int index = 0;
	for (const auto& npcEntry : g_game.getNpcs()) {
		pushUserdata<Npc>(L, npcEntry.second);
		setMetatable(L, -1, "Npc");
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaGameGetMonsters(lua_State* L)
{
	// Game.getMonsters()
	lua_createtable(L, g_game.getMonstersOnline(), 0);

	int index = 0;
	for (const auto& monsterEntry : g_game.getMonsters()) {
		pushUserdata<Monster>(L, monsterEntry.second);
		setMetatable(L, -1, "Monster");
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaGameLoadMap(lua_State* L)
{
	// Game.loadMap(path)
	const std::string& path = getString(L, 1);
	g_dispatcher.addTask(createTask([path]() {
		try {
			g_game.loadMap(path);
		} catch (const std::exception& e) {
			// FIXME: Should only catch some exceptions
			std::cout << "[Error - luaGameLoadMap] Failed to load map: " << e.what() << std::endl;
		}
	}));
	return 0;
}

static int luaGameGetExperienceStage(lua_State* L)
{
	// Game.getExperienceStage(level)
	uint32_t level = getNumber<uint32_t>(L, 1);
	lua_pushnumber(L, g_config.getExperienceStage(level));
	return 1;
}

static int luaGameGetExperienceForLevel(lua_State* L)
{
	// Game.getExperienceForLevel(level)
	const uint32_t level = getNumber<uint32_t>(L, 1);
	if (level == 0) {
		lua_pushnumber(L, 0);
	} else {
		lua_pushnumber(L, Player::getExpForLevel(level));
	}
	return 1;
}

static int luaGameGetMonsterCount(lua_State* L)
{
	// Game.getMonsterCount()
	lua_pushnumber(L, g_game.getMonstersOnline());
	return 1;
}

static int luaGameGetPlayerCount(lua_State* L)
{
	// Game.getPlayerCount()
	lua_pushnumber(L, g_game.getPlayersOnline());
	return 1;
}

static int luaGameGetNpcCount(lua_State* L)
{
	// Game.getNpcCount()
	lua_pushnumber(L, g_game.getNpcsOnline());
	return 1;
}

static int luaGameGetMonsterTypes(lua_State* L)
{
	// Game.getMonsterTypes()
	auto& type = g_monsters.monsters;
	lua_createtable(L, type.size(), 0);

	for (auto& mType : type) {
		pushUserdata<MonsterType>(L, &mType.second);
		setMetatable(L, -1, "MonsterType");
		lua_setfield(L, -2, mType.first.c_str());
	}
	return 1;
}

static int luaGameGetCurrencyItems(lua_State* L)
{
	// Game.getCurrencyItems()
	const auto& currencyItems = Item::items.currencyItems;
	size_t size = currencyItems.size();
	lua_createtable(L, size, 0);

	for (const auto& it : currencyItems) {
		const ItemType& itemType = Item::items[it.second];
		pushUserdata<const ItemType>(L, &itemType);
		setMetatable(L, -1, "ItemType");
		lua_rawseti(L, -2, size--);
	}
	return 1;
}

static int luaGameGetItemTypeByClientId(lua_State* L)
{
	// Game.getItemTypeByClientId(clientId)
	uint16_t spriteId = getNumber<uint16_t>(L, 1);
	const ItemType& itemType = Item::items.getItemIdByClientId(spriteId);
	if (itemType.id != 0) {
		pushUserdata<const ItemType>(L, &itemType);
		setMetatable(L, -1, "ItemType");
	} else {
		lua_pushnil(L);
	}

	return 1;
}

static int luaGameGetMountIdByLookType(lua_State* L)
{
	// Game.getMountIdByLookType(lookType)
	Mount* mount = nullptr;
	if (isNumber(L, 1)) {
		mount = g_game.mounts.getMountByClientID(getNumber<uint16_t>(L, 1));
	}

	if (mount) {
		lua_pushnumber(L, mount->id);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaGameGetTowns(lua_State* L)
{
	// Game.getTowns()
	const auto& towns = g_game.map.towns.getTowns();
	lua_createtable(L, towns.size(), 0);

	int index = 0;
	for (auto townEntry : towns) {
		pushUserdata<Town>(L, townEntry.second);
		setMetatable(L, -1, "Town");
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaGameGetHouses(lua_State* L)
{
	// Game.getHouses()
	const auto& houses = g_game.map.houses.getHouses();
	lua_createtable(L, houses.size(), 0);

	int index = 0;
	for (auto houseEntry : houses) {
		pushUserdata<House>(L, houseEntry.second);
		setMetatable(L, -1, "House");
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaGameGetOutfits(lua_State* L)
{
	// Game.getOutfits(playerSex)
	if (!isNumber(L, 1)) {
		lua_pushnil(L);
		return 1;
	}

	PlayerSex_t playerSex = getNumber<PlayerSex_t>(L, 1);
	if (playerSex > PLAYERSEX_LAST) {
		lua_pushnil(L);
		return 1;
	}

	const auto& outfits = Outfits::getInstance().getOutfits(playerSex);
	lua_createtable(L, outfits.size(), 0);

	int index = 0;
	for (const auto& outfit : outfits) {
		pushOutfit(L, &outfit);
		lua_rawseti(L, -2, ++index);
	}

	return 1;
}

static int luaGameGetMounts(lua_State* L)
{
	// Game.getMounts()
	const auto& mounts = g_game.mounts.getMounts();
	lua_createtable(L, mounts.size(), 0);

	int index = 0;
	for (const auto& mount : mounts) {
		pushMount(L, &mount);
		lua_rawseti(L, -2, ++index);
	}

	return 1;
}

static int luaGameGetGameState(lua_State* L)
{
	// Game.getGameState()
	lua_pushnumber(L, g_game.getGameState());
	return 1;
}

static int luaGameSetGameState(lua_State* L)
{
	// Game.setGameState(state)
	GameState_t state = getNumber<GameState_t>(L, 1);
	g_game.setGameState(state);
	pushBoolean(L, true);
	return 1;
}

static int luaGameGetWorldType(lua_State* L)
{
	// Game.getWorldType()
	lua_pushnumber(L, g_game.getWorldType());
	return 1;
}

static int luaGameSetWorldType(lua_State* L)
{
	// Game.setWorldType(type)
	WorldType_t type = getNumber<WorldType_t>(L, 1);
	g_game.setWorldType(type);
	pushBoolean(L, true);
	return 1;
}

static int luaGameGetReturnMessage(lua_State* L)
{
	// Game.getReturnMessage(value)
	ReturnValue value = getNumber<ReturnValue>(L, 1);
	pushString(L, getReturnMessage(value));
	return 1;
}

static int luaGameGetItemAttributeByName(lua_State* L)
{
	// Game.getItemAttributeByName(name)
	lua_pushnumber(L, stringToItemAttribute(getString(L, 1)));
	return 1;
}

static int luaGameCreateItem(lua_State* L)
{
	// Game.createItem(itemId[, count[, position]])
	uint16_t count = getNumber<uint16_t>(L, 2, 1);
	uint16_t id;
	if (isNumber(L, 1)) {
		id = getNumber<uint16_t>(L, 1);
	} else {
		id = Item::items.getItemIdByName(getString(L, 1));
		if (id == 0) {
			lua_pushnil(L);
			return 1;
		}
	}

	const ItemType& it = Item::items[id];
	if (it.stackable) {
		count = std::min<uint16_t>(count, 100);
	}

	Item* item = Item::CreateItem(id, count);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	if (lua_gettop(L) >= 3) {
		const Position& position = getPosition(L, 3);
		Tile* tile = g_game.map.getTile(position);
		if (!tile) {
			delete item;
			lua_pushnil(L);
			return 1;
		}

		g_game.internalAddItem(tile, item, INDEX_WHEREEVER, FLAG_NOLIMIT);
	} else {
		LuaScriptInterface::getScriptEnv()->addTempItem(item);
		item->setParent(VirtualCylinder::virtualCylinder);
	}

	pushUserdata<Item>(L, item);
	setItemMetatable(L, -1, item);
	return 1;
}

static int luaGameCreateContainer(lua_State* L)
{
	// Game.createContainer(itemId, size[, position])
	uint16_t size = getNumber<uint16_t>(L, 2);
	uint16_t id;
	if (isNumber(L, 1)) {
		id = getNumber<uint16_t>(L, 1);
	} else {
		id = Item::items.getItemIdByName(getString(L, 1));
		if (id == 0) {
			lua_pushnil(L);
			return 1;
		}
	}

	Container* container = Item::CreateItemAsContainer(id, size);
	if (!container) {
		lua_pushnil(L);
		return 1;
	}

	if (lua_gettop(L) >= 3) {
		const Position& position = getPosition(L, 3);
		Tile* tile = g_game.map.getTile(position);
		if (!tile) {
			delete container;
			lua_pushnil(L);
			return 1;
		}

		g_game.internalAddItem(tile, container, INDEX_WHEREEVER, FLAG_NOLIMIT);
	} else {
		LuaScriptInterface::getScriptEnv()->addTempItem(container);
		container->setParent(VirtualCylinder::virtualCylinder);
	}

	pushUserdata<Container>(L, container);
	setMetatable(L, -1, "Container");
	return 1;
}

static int luaGameCreateMonster(lua_State* L)
{
	// Game.createMonster(monsterName, position[, extended = false[, force = false[, magicEffect = CONST_ME_TELEPORT]]])
	Monster* monster = Monster::createMonster(getString(L, 1));
	if (!monster) {
		lua_pushnil(L);
		return 1;
	}

	const Position& position = getPosition(L, 2);
	bool extended = getBoolean(L, 3, false);
	bool force = getBoolean(L, 4, false);
	MagicEffectClasses magicEffect = getNumber<MagicEffectClasses>(L, 5, CONST_ME_TELEPORT);
	if (g_events->eventMonsterOnSpawn(monster, position, false, true) || force) {
		if (g_game.placeCreature(monster, position, extended, force, magicEffect)) {
			pushUserdata<Monster>(L, monster);
			setMetatable(L, -1, "Monster");
		} else {
			delete monster;
			lua_pushnil(L);
		}
	} else {
		delete monster;
		lua_pushnil(L);
	}
	return 1;
}

static int luaGameCreateNpc(lua_State* L)
{
	// Game.createNpc(npcName, position[, extended = false[, force = false[, magicEffect = CONST_ME_TELEPORT]]])
	Npc* npc = Npc::createNpc(getString(L, 1));
	if (!npc) {
		lua_pushnil(L);
		return 1;
	}

	const Position& position = getPosition(L, 2);
	bool extended = getBoolean(L, 3, false);
	bool force = getBoolean(L, 4, false);
	MagicEffectClasses magicEffect = getNumber<MagicEffectClasses>(L, 5, CONST_ME_TELEPORT);
	if (g_game.placeCreature(npc, position, extended, force, magicEffect)) {
		pushUserdata<Npc>(L, npc);
		setMetatable(L, -1, "Npc");
	} else {
		delete npc;
		lua_pushnil(L);
	}
	return 1;
}

static int luaGameCreateTile(lua_State* L)
{
	// Game.createTile(x, y, z[, isDynamic = false])
	// Game.createTile(position[, isDynamic = false])
	Position position;
	bool isDynamic;
	if (isTable(L, 1)) {
		position = getPosition(L, 1);
		isDynamic = getBoolean(L, 2, false);
	} else {
		position.x = getNumber<uint16_t>(L, 1);
		position.y = getNumber<uint16_t>(L, 2);
		position.z = getNumber<uint16_t>(L, 3);
		isDynamic = getBoolean(L, 4, false);
	}

	Tile* tile = g_game.map.getTile(position);
	if (!tile) {
		if (isDynamic) {
			tile = new DynamicTile(position.x, position.y, position.z);
		} else {
			tile = new StaticTile(position.x, position.y, position.z);
		}

		g_game.map.setTile(position, tile);
	}

	pushUserdata(L, tile);
	setMetatable(L, -1, "Tile");
	return 1;
}

static int luaGameCreateMonsterType(lua_State* L)
{
	// Game.createMonsterType(name)
	if (LuaScriptInterface::getScriptEnv()->getScriptInterface() != &g_scripts->getScriptInterface()) {
		reportErrorFunc(L, "MonsterTypes can only be registered in the Scripts interface.");
		lua_pushnil(L);
		return 1;
	}

	const std::string& name = getString(L, 1);
	if (name.length() == 0) {
		lua_pushnil(L);
		return 1;
	}

	MonsterType* monsterType = g_monsters.getMonsterType(name, false);
	if (!monsterType) {
		monsterType = &g_monsters.monsters[boost::algorithm::to_lower_copy(name)];
		monsterType->name = name;
		monsterType->nameDescription = "a " + name;
	} else {
		monsterType->info.lootItems.clear();
		monsterType->info.attackSpells.clear();
		monsterType->info.defenseSpells.clear();
		monsterType->info.scripts.clear();
		monsterType->info.thinkEvent = -1;
		monsterType->info.creatureAppearEvent = -1;
		monsterType->info.creatureDisappearEvent = -1;
		monsterType->info.creatureMoveEvent = -1;
		monsterType->info.creatureSayEvent = -1;
	}

	pushUserdata<MonsterType>(L, monsterType);
	setMetatable(L, -1, "MonsterType");
	return 1;
}

static int luaGameCreateItemType(lua_State* L)
{
	// Game.createItemType(id, clientid)
	if (LuaScriptInterface::getScriptEnv()->getScriptInterface() != &g_scripts->getScriptInterface()) {
		reportErrorFunc(L, "ItemTypes can only be registered in the Scripts interface.");
		lua_pushnil(L);
		return 1;
	}

	uint16_t id = getNumber<uint16_t>(L, 1);

	if (Item::items.itemTypeExists(id)) {
		ItemType& itemType = Item::items.getItemType(id);
		pushUserdata<ItemType>(L, &itemType);
		setMetatable(L, -1, "ItemType");
		return 1;
	}

	uint16_t clientid = getNumber<uint16_t>(L, 2);
	ItemType& iType = Item::items.parseItemLua(id, clientid);
	pushUserdata<ItemType>(L, &iType);
	setMetatable(L, -1, "ItemType");
	return 1;
}

static int luaGameStartRaid(lua_State* L)
{
	// Game.startRaid(raidName)
	const std::string& raidName = getString(L, 1);

	Raid* raid = g_game.raids.getRaidByName(raidName);
	if (!raid || !raid->isLoaded()) {
		lua_pushnumber(L, RETURNVALUE_NOSUCHRAIDEXISTS);
		return 1;
	}

	if (g_game.raids.getRunning()) {
		lua_pushnumber(L, RETURNVALUE_ANOTHERRAIDISALREADYEXECUTING);
		return 1;
	}

	g_game.raids.setRunning(raid);
	raid->startRaid();
	lua_pushnumber(L, RETURNVALUE_NOERROR);
	return 1;
}

static int luaGameGetClientVersion(lua_State* L)
{
	// Game.getClientVersion()
	lua_createtable(L, 0, 3);
	setField(L, "min", CLIENT_VERSION_MIN);
	setField(L, "max", CLIENT_VERSION_MAX);
	setField(L, "string", CLIENT_VERSION_STR);
	return 1;
}

static int luaGameReload(lua_State* L)
{
	// Game.reload(reloadType)
	ReloadTypes_t reloadType = getNumber<ReloadTypes_t>(L, 1);
	if (reloadType == RELOAD_TYPE_LIBRARY) {
		pushBoolean(L, g_scripts->loadLibs());
	} else {
		pushBoolean(L, g_game.reload(reloadType));
	}
	lua_gc(g_luaEnvironment.getLuaState(), LUA_GCCOLLECT, 0);
	return 1;
}

static int luaGameGetAccountStorageValue(lua_State* L)
{
	// Game.getAccountStorageValue(accountId, key)
	uint32_t accountId = getNumber<uint32_t>(L, 1);
	uint32_t key = getNumber<uint32_t>(L, 2);

	lua_pushnumber(L, g_game.getAccountStorageValue(accountId, key));

	return 1;
}

static int luaGameSetAccountStorageValue(lua_State* L)
{
	// Game.setAccountStorageValue(accountId, key, value)
	uint32_t accountId = getNumber<uint32_t>(L, 1);
	uint32_t key = getNumber<uint32_t>(L, 2);
	int32_t value = getNumber<int32_t>(L, 3);

	g_game.setAccountStorageValue(accountId, key, value);
	lua_pushboolean(L, true);

	return 1;
}

static int luaGameSaveAccountStorageValues(lua_State* L)
{
	// Game.saveAccountStorageValues()
	lua_pushboolean(L, g_game.saveAccountStorageValues());

	return 1;
}

namespace LuaGame {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerTable("Game");

	interface->registerMethod("Game", "getSpectators", luaGameGetSpectators);
	interface->registerMethod("Game", "getPlayers", luaGameGetPlayers);
	interface->registerMethod("Game", "getNpcs", luaGameGetNpcs);
	interface->registerMethod("Game", "getMonsters", luaGameGetMonsters);
	interface->registerMethod("Game", "loadMap", luaGameLoadMap);

	interface->registerMethod("Game", "getExperienceStage", luaGameGetExperienceStage);
	interface->registerMethod("Game", "getExperienceForLevel", luaGameGetExperienceForLevel);
	interface->registerMethod("Game", "getMonsterCount", luaGameGetMonsterCount);
	interface->registerMethod("Game", "getPlayerCount", luaGameGetPlayerCount);
	interface->registerMethod("Game", "getNpcCount", luaGameGetNpcCount);
	interface->registerMethod("Game", "getMonsterTypes", luaGameGetMonsterTypes);
	interface->registerMethod("Game", "getCurrencyItems", luaGameGetCurrencyItems);
	interface->registerMethod("Game", "getItemTypeByClientId", luaGameGetItemTypeByClientId);
	interface->registerMethod("Game", "getMountIdByLookType", luaGameGetMountIdByLookType);

	interface->registerMethod("Game", "getTowns", luaGameGetTowns);
	interface->registerMethod("Game", "getHouses", luaGameGetHouses);
	interface->registerMethod("Game", "getOutfits", luaGameGetOutfits);
	interface->registerMethod("Game", "getMounts", luaGameGetMounts);

	interface->registerMethod("Game", "getGameState", luaGameGetGameState);
	interface->registerMethod("Game", "setGameState", luaGameSetGameState);

	interface->registerMethod("Game", "getWorldType", luaGameGetWorldType);
	interface->registerMethod("Game", "setWorldType", luaGameSetWorldType);

	interface->registerMethod("Game", "getItemAttributeByName", luaGameGetItemAttributeByName);
	interface->registerMethod("Game", "getReturnMessage", luaGameGetReturnMessage);

	interface->registerMethod("Game", "createItem", luaGameCreateItem);
	interface->registerMethod("Game", "createContainer", luaGameCreateContainer);
	interface->registerMethod("Game", "createMonster", luaGameCreateMonster);
	interface->registerMethod("Game", "createNpc", luaGameCreateNpc);
	interface->registerMethod("Game", "createTile", luaGameCreateTile);
	interface->registerMethod("Game", "createMonsterType", luaGameCreateMonsterType);
	interface->registerMethod("Game", "createItemType", luaGameCreateItemType);

	interface->registerMethod("Game", "startRaid", luaGameStartRaid);

	interface->registerMethod("Game", "getClientVersion", luaGameGetClientVersion);

	interface->registerMethod("Game", "reload", luaGameReload);

	interface->registerMethod("Game", "getAccountStorageValue", luaGameGetAccountStorageValue);
	interface->registerMethod("Game", "setAccountStorageValue", luaGameSetAccountStorageValue);
	interface->registerMethod("Game", "saveAccountStorageValues", luaGameSaveAccountStorageValues);
}
} // namespace LuaGame

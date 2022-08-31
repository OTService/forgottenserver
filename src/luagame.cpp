#include "otpch.h"

#include "luagame.h"

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

void LuaScriptInterface::registerGameFunctions()
{
	registerTable("Game");

	registerMethod("Game", "getSpectators", LuaGame::luaGameGetSpectators);
	registerMethod("Game", "getPlayers", LuaGame::luaGameGetPlayers);
	registerMethod("Game", "getNpcs", LuaGame::luaGameGetNpcs);
	registerMethod("Game", "getMonsters", LuaGame::luaGameGetMonsters);
	registerMethod("Game", "loadMap", LuaGame::luaGameLoadMap);

	registerMethod("Game", "getExperienceStage", LuaGame::luaGameGetExperienceStage);
	registerMethod("Game", "getExperienceForLevel", LuaGame::luaGameGetExperienceForLevel);
	registerMethod("Game", "getMonsterCount", LuaGame::luaGameGetMonsterCount);
	registerMethod("Game", "getPlayerCount", LuaGame::luaGameGetPlayerCount);
	registerMethod("Game", "getNpcCount", LuaGame::luaGameGetNpcCount);
	registerMethod("Game", "getMonsterTypes", LuaGame::luaGameGetMonsterTypes);
	registerMethod("Game", "getCurrencyItems", LuaGame::luaGameGetCurrencyItems);
	registerMethod("Game", "getItemTypeByClientId", LuaGame::luaGameGetItemTypeByClientId);
	registerMethod("Game", "getMountIdByLookType", LuaGame::luaGameGetMountIdByLookType);

	registerMethod("Game", "getTowns", LuaGame::luaGameGetTowns);
	registerMethod("Game", "getHouses", LuaGame::luaGameGetHouses);
	registerMethod("Game", "getOutfits", LuaGame::luaGameGetOutfits);
	registerMethod("Game", "getMounts", LuaGame::luaGameGetMounts);

	registerMethod("Game", "getGameState", LuaGame::luaGameGetGameState);
	registerMethod("Game", "setGameState", LuaGame::luaGameSetGameState);

	registerMethod("Game", "getWorldType", LuaGame::luaGameGetWorldType);
	registerMethod("Game", "setWorldType", LuaGame::luaGameSetWorldType);

	registerMethod("Game", "getItemAttributeByName", LuaGame::luaGameGetItemAttributeByName);
	registerMethod("Game", "getReturnMessage", LuaGame::luaGameGetReturnMessage);

	registerMethod("Game", "createItem", LuaGame::luaGameCreateItem);
	registerMethod("Game", "createContainer", LuaGame::luaGameCreateContainer);
	registerMethod("Game", "createMonster", LuaGame::luaGameCreateMonster);
	registerMethod("Game", "createNpc", LuaGame::luaGameCreateNpc);
	registerMethod("Game", "createTile", LuaGame::luaGameCreateTile);
	registerMethod("Game", "createMonsterType", LuaGame::luaGameCreateMonsterType);
	registerMethod("Game", "createItemType", LuaGame::luaGameCreateItemType);

	registerMethod("Game", "startRaid", LuaGame::luaGameStartRaid);

	registerMethod("Game", "getClientVersion", LuaGame::luaGameGetClientVersion);

	registerMethod("Game", "reload", LuaGame::luaGameReload);

	registerMethod("Game", "getAccountStorageValue", LuaGame::luaGameGetAccountStorageValue);
	registerMethod("Game", "setAccountStorageValue", LuaGame::luaGameSetAccountStorageValue);
	registerMethod("Game", "saveAccountStorageValues", LuaGame::luaGameSaveAccountStorageValues);
}

// Game
int LuaGame::luaGameGetSpectators(lua_State* L)
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

int LuaGame::luaGameGetPlayers(lua_State* L)
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

int LuaGame::luaGameGetNpcs(lua_State* L)
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

int LuaGame::luaGameGetMonsters(lua_State* L)
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

int LuaGame::luaGameLoadMap(lua_State* L)
{
	// Game.loadMap(path)
	const std::string& path = getString(L, 1);
	g_dispatcher.addTask(createTask([path]() {
		try {
			g_game.loadMap(path);
		} catch (const std::exception& e) {
			// FIXME: Should only catch some exceptions
			std::cout << "[Error - LuaGame::luaGameLoadMap] Failed to load map: " << e.what() << std::endl;
		}
	}));
	return 0;
}

int LuaGame::luaGameGetExperienceStage(lua_State* L)
{
	// Game.getExperienceStage(level)
	uint32_t level = getNumber<uint32_t>(L, 1);
	lua_pushnumber(L, g_config.getExperienceStage(level));
	return 1;
}

int LuaGame::luaGameGetExperienceForLevel(lua_State* L)
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

int LuaGame::luaGameGetMonsterCount(lua_State* L)
{
	// Game.getMonsterCount()
	lua_pushnumber(L, g_game.getMonstersOnline());
	return 1;
}

int LuaGame::luaGameGetPlayerCount(lua_State* L)
{
	// Game.getPlayerCount()
	lua_pushnumber(L, g_game.getPlayersOnline());
	return 1;
}

int LuaGame::luaGameGetNpcCount(lua_State* L)
{
	// Game.getNpcCount()
	lua_pushnumber(L, g_game.getNpcsOnline());
	return 1;
}

int LuaGame::luaGameGetMonsterTypes(lua_State* L)
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

int LuaGame::luaGameGetCurrencyItems(lua_State* L)
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

int LuaGame::luaGameGetItemTypeByClientId(lua_State* L)
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

int LuaGame::luaGameGetMountIdByLookType(lua_State* L)
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

int LuaGame::luaGameGetTowns(lua_State* L)
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

int LuaGame::luaGameGetHouses(lua_State* L)
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

int LuaGame::luaGameGetOutfits(lua_State* L)
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

int LuaGame::luaGameGetMounts(lua_State* L)
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

int LuaGame::luaGameGetGameState(lua_State* L)
{
	// Game.getGameState()
	lua_pushnumber(L, g_game.getGameState());
	return 1;
}

int LuaGame::luaGameSetGameState(lua_State* L)
{
	// Game.setGameState(state)
	GameState_t state = getNumber<GameState_t>(L, 1);
	g_game.setGameState(state);
	pushBoolean(L, true);
	return 1;
}

int LuaGame::luaGameGetWorldType(lua_State* L)
{
	// Game.getWorldType()
	lua_pushnumber(L, g_game.getWorldType());
	return 1;
}

int LuaGame::luaGameSetWorldType(lua_State* L)
{
	// Game.setWorldType(type)
	WorldType_t type = getNumber<WorldType_t>(L, 1);
	g_game.setWorldType(type);
	pushBoolean(L, true);
	return 1;
}

int LuaGame::luaGameGetReturnMessage(lua_State* L)
{
	// Game.getReturnMessage(value)
	ReturnValue value = getNumber<ReturnValue>(L, 1);
	pushString(L, getReturnMessage(value));
	return 1;
}

int LuaGame::luaGameGetItemAttributeByName(lua_State* L)
{
	// Game.getItemAttributeByName(name)
	lua_pushnumber(L, stringToItemAttribute(getString(L, 1)));
	return 1;
}

int LuaGame::luaGameCreateItem(lua_State* L)
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
		getScriptEnv()->addTempItem(item);
		item->setParent(VirtualCylinder::virtualCylinder);
	}

	pushUserdata<Item>(L, item);
	setItemMetatable(L, -1, item);
	return 1;
}

int LuaGame::luaGameCreateContainer(lua_State* L)
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
		getScriptEnv()->addTempItem(container);
		container->setParent(VirtualCylinder::virtualCylinder);
	}

	pushUserdata<Container>(L, container);
	setMetatable(L, -1, "Container");
	return 1;
}

int LuaGame::luaGameCreateMonster(lua_State* L)
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

int LuaGame::luaGameCreateNpc(lua_State* L)
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

int LuaGame::luaGameCreateTile(lua_State* L)
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

int LuaGame::luaGameCreateMonsterType(lua_State* L)
{
	// Game.createMonsterType(name)
	if (getScriptEnv()->getScriptInterface() != &g_scripts->getScriptInterface()) {
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

int LuaGame::luaGameCreateItemType(lua_State* L)
{
	// Game.createItemType(id)
	if (getScriptEnv()->getScriptInterface() != &g_scripts->getScriptInterface()) {
		reportErrorFunc(L, "ItemTypes can only be registered in the Scripts interface.");
		lua_pushnil(L);
		return 1;
	}

	uint32_t id = getNumber<uint32_t>(L, 1);
	ItemType& itemType = Item::items.parseItemLua(id);
	if (itemType.id == 0) {
		pushBoolean(L, false);
		return 1;
	}

	if (!itemType.name.empty()) {
		std::cout << "[Warning - Items::parseItemNode] Duplicate item with id: " << id << std::endl;
		pushBoolean(L, false);
		return 1;
	}

	pushUserdata<ItemType>(L, &itemType);
	setMetatable(L, -1, "ItemType");
	return 1;
}

int LuaGame::luaGameStartRaid(lua_State* L)
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

int LuaGame::luaGameGetClientVersion(lua_State* L)
{
	// Game.getClientVersion()
	lua_createtable(L, 0, 3);
	setField(L, "min", CLIENT_VERSION_MIN);
	setField(L, "max", CLIENT_VERSION_MAX);
	setField(L, "string", CLIENT_VERSION_STR);
	return 1;
}

int LuaGame::luaGameReload(lua_State* L)
{
	// Game.reload(reloadType)
	ReloadTypes_t reloadType = getNumber<ReloadTypes_t>(L, 1);
	if (reloadType == RELOAD_TYPE_GLOBAL) {
		pushBoolean(L, g_luaEnvironment.loadFile("data/global.lua") == 0);
		pushBoolean(L, g_scripts->loadScripts("scripts/lib", true, true));
	} else {
		pushBoolean(L, g_game.reload(reloadType));
	}
	lua_gc(g_luaEnvironment.getLuaState(), LUA_GCCOLLECT, 0);
	return 1;
}

int LuaGame::luaGameGetAccountStorageValue(lua_State* L)
{
	// Game.getAccountStorageValue(accountId, key)
	uint32_t accountId = getNumber<uint32_t>(L, 1);
	uint32_t key = getNumber<uint32_t>(L, 2);

	lua_pushnumber(L, g_game.getAccountStorageValue(accountId, key));

	return 1;
}

int LuaGame::luaGameSetAccountStorageValue(lua_State* L)
{
	// Game.setAccountStorageValue(accountId, key, value)
	uint32_t accountId = getNumber<uint32_t>(L, 1);
	uint32_t key = getNumber<uint32_t>(L, 2);
	int32_t value = getNumber<int32_t>(L, 3);

	g_game.setAccountStorageValue(accountId, key, value);
	lua_pushboolean(L, true);

	return 1;
}

int LuaGame::luaGameSaveAccountStorageValues(lua_State* L)
{
	// Game.saveAccountStorageValues()
	lua_pushboolean(L, g_game.saveAccountStorageValues());

	return 1;
}

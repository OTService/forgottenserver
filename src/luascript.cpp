// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "luascript.h"

#include "bed.h"
#include "chat.h"
#include "configmanager.h"
#include "databasemanager.h"
#include "databasetasks.h"
#include "luaaction.cpp"
#include "luacombat.cpp"
#include "luacondition.cpp"
#include "luacontainer.cpp"
#include "luacreature.cpp"
#include "luacreatureevent.cpp"
#include "luaenums.cpp"
#include "luagame.cpp"
#include "luaglobalevent.cpp"
#include "luagroup.cpp"
#include "luaguild.cpp"
#include "luahouse.cpp"
#include "luaitem.cpp"
#include "luaitemtype.cpp"
#include "lualoot.cpp"
#include "luamodalwindow.cpp"
#include "luamonster.cpp"
#include "luamonsterspell.cpp"
#include "luamonstertype.cpp"
#include "luamoveevent.cpp"
#include "luanetworkmessage.cpp"
#include "luanpc.cpp"
#include "luaparty.cpp"
#include "luaplayer.cpp"
#include "luaposition.cpp"
#include "luaspell.cpp"
#include "luatalkaction.cpp"
#include "luatile.cpp"
#include "luatown.cpp"
#include "luavariant.h"
#include "luavocation.cpp"
#include "luaweapon.cpp"
#include "luaxml.cpp"
#include "monster.h"
#include "movement.h"
#include "npc.h"
#include "outfit.h"
#include "party.h"
#include "podium.h"
#include "protocolstatus.h"
#include "scheduler.h"
#include "teleport.h"
#include "http/responder.h"

#include <boost/range/adaptor/reversed.hpp>

ScriptEnvironment::DBResultMap ScriptEnvironment::tempResults;
uint32_t ScriptEnvironment::lastResultId = 0;

std::multimap<ScriptEnvironment*, Item*> ScriptEnvironment::tempItems;

LuaEnvironment g_luaEnvironment;

ScriptEnvironment::ScriptEnvironment() { resetEnv(); }

ScriptEnvironment::~ScriptEnvironment() { resetEnv(); }

void ScriptEnvironment::resetEnv()
{
	scriptId = 0;
	callbackId = 0;
	timerEvent = false;
	interface = nullptr;
	localMap.clear();
	tempResults.clear();

	auto pair = tempItems.equal_range(this);
	auto it = pair.first;
	while (it != pair.second) {
		Item* item = it->second;
		if (item && item->getParent() == VirtualCylinder::virtualCylinder) {
			g_game.ReleaseItem(item);
		}
		it = tempItems.erase(it);
	}
}

bool ScriptEnvironment::setCallbackId(int32_t callbackId, LuaScriptInterface* scriptInterface)
{
	if (this->callbackId != 0) {
		// nested callbacks are not allowed
		if (interface) {
			reportErrorFunc(interface->getLuaState(), "Nested callbacks!");
		}
		return false;
	}

	this->callbackId = callbackId;
	interface = scriptInterface;
	return true;
}

void ScriptEnvironment::getEventInfo(int32_t& scriptId, LuaScriptInterface*& scriptInterface, int32_t& callbackId,
                                     bool& timerEvent) const
{
	scriptId = this->scriptId;
	scriptInterface = interface;
	callbackId = this->callbackId;
	timerEvent = this->timerEvent;
}

uint32_t ScriptEnvironment::addThing(Thing* thing)
{
	if (!thing || thing->isRemoved()) {
		return 0;
	}

	Creature* creature = thing->getCreature();
	if (creature) {
		return creature->getID();
	}

	Item* item = thing->getItem();
	if (item && item->hasAttribute(ITEM_ATTRIBUTE_UNIQUEID)) {
		return item->getUniqueId();
	}

	for (const auto& it : localMap) {
		if (it.second == item) {
			return it.first;
		}
	}

	localMap[++lastUID] = item;
	return lastUID;
}

void ScriptEnvironment::insertItem(uint32_t uid, Item* item)
{
	auto result = localMap.emplace(uid, item);
	if (!result.second) {
		std::cout << std::endl << "Lua Script Error: Thing uid already taken.";
	}
}

Thing* ScriptEnvironment::getThingByUID(uint32_t uid)
{
	if (uid >= CREATURE_ID_MIN) {
		return g_game.getCreatureByID(uid);
	}

	if (uid <= std::numeric_limits<uint16_t>::max()) {
		Item* item = g_game.getUniqueItem(uid);
		if (item && !item->isRemoved()) {
			return item;
		}
		return nullptr;
	}

	auto it = localMap.find(uid);
	if (it != localMap.end()) {
		Item* item = it->second;
		if (!item->isRemoved()) {
			return item;
		}
	}
	return nullptr;
}

Item* ScriptEnvironment::getItemByUID(uint32_t uid)
{
	Thing* thing = getThingByUID(uid);
	if (!thing) {
		return nullptr;
	}
	return thing->getItem();
}

Container* ScriptEnvironment::getContainerByUID(uint32_t uid)
{
	Item* item = getItemByUID(uid);
	if (!item) {
		return nullptr;
	}
	return item->getContainer();
}

void ScriptEnvironment::removeItemByUID(uint32_t uid)
{
	if (uid <= std::numeric_limits<uint16_t>::max()) {
		g_game.removeUniqueItem(uid);
		return;
	}

	auto it = localMap.find(uid);
	if (it != localMap.end()) {
		localMap.erase(it);
	}
}

void ScriptEnvironment::addTempItem(Item* item) { tempItems.emplace(this, item); }

void ScriptEnvironment::removeTempItem(Item* item)
{
	for (auto it = tempItems.begin(), end = tempItems.end(); it != end; ++it) {
		if (it->second == item) {
			tempItems.erase(it);
			break;
		}
	}
}

uint32_t ScriptEnvironment::addResult(DBResult_ptr res)
{
	tempResults[++lastResultId] = res;
	return lastResultId;
}

bool ScriptEnvironment::removeResult(uint32_t id)
{
	auto it = tempResults.find(id);
	if (it == tempResults.end()) {
		return false;
	}

	tempResults.erase(it);
	return true;
}

DBResult_ptr ScriptEnvironment::getResultByID(uint32_t id)
{
	auto it = tempResults.find(id);
	if (it == tempResults.end()) {
		return nullptr;
	}
	return it->second;
}

std::string LuaScriptInterface::getErrorDesc(ErrorCode_t code)
{
	switch (code) {
		case LUA_ERROR_PLAYER_NOT_FOUND:
			return "Player not found";
		case LUA_ERROR_CREATURE_NOT_FOUND:
			return "Creature not found";
		case LUA_ERROR_ITEM_NOT_FOUND:
			return "Item not found";
		case LUA_ERROR_THING_NOT_FOUND:
			return "Thing not found";
		case LUA_ERROR_TILE_NOT_FOUND:
			return "Tile not found";
		case LUA_ERROR_HOUSE_NOT_FOUND:
			return "House not found";
		case LUA_ERROR_COMBAT_NOT_FOUND:
			return "Combat not found";
		case LUA_ERROR_CONDITION_NOT_FOUND:
			return "Condition not found";
		case LUA_ERROR_AREA_NOT_FOUND:
			return "Area not found";
		case LUA_ERROR_CONTAINER_NOT_FOUND:
			return "Container not found";
		case LUA_ERROR_VARIANT_NOT_FOUND:
			return "Variant not found";
		case LUA_ERROR_VARIANT_UNKNOWN:
			return "Unknown variant type";
		case LUA_ERROR_SPELL_NOT_FOUND:
			return "Spell not found";
		default:
			return "Bad error code";
	}
}

ScriptEnvironment LuaScriptInterface::scriptEnv[16];
int32_t LuaScriptInterface::scriptEnvIndex = -1;

LuaScriptInterface::LuaScriptInterface(std::string interfaceName) : interfaceName(std::move(interfaceName))
{
	if (!g_luaEnvironment.getLuaState()) {
		g_luaEnvironment.initState();
	}
}

LuaScriptInterface::~LuaScriptInterface() { closeState(); }

bool LuaScriptInterface::reInitState()
{
	g_luaEnvironment.clearCombatObjects(this);
	g_luaEnvironment.clearAreaObjects(this);

	closeState();
	return initState();
}

/// Same as lua_pcall, but adds stack trace to error strings in called function.
int LuaScriptInterface::protectedCall(lua_State* L, int nargs, int nresults)
{
	int error_index = lua_gettop(L) - nargs;
	lua_pushcfunction(L, luaErrorHandler);
	lua_insert(L, error_index);

	int ret = lua_pcall(L, nargs, nresults, error_index);
	lua_remove(L, error_index);
	return ret;
}

int32_t LuaScriptInterface::loadFile(const std::string& file, Npc* npc /* = nullptr*/)
{
	// loads file as a chunk at stack top
	int ret = luaL_loadfile(luaState, file.c_str());
	if (ret != 0) {
		lastLuaError = popString(luaState);
		return -1;
	}

	// check that it is loaded as a function
	if (!isFunction(luaState, -1)) {
		lua_pop(luaState, 1);
		return -1;
	}

	loadingFile = file;

	if (!reserveScriptEnv()) {
		lua_pop(luaState, 1);
		return -1;
	}

	ScriptEnvironment* env = getScriptEnv();
	env->setScriptId(EVENT_ID_LOADING, this);
	env->setNpc(npc);

	// execute it
	ret = protectedCall(luaState, 0, 0);
	if (ret != 0) {
		reportError(nullptr, popString(luaState));
		resetScriptEnv();
		return -1;
	}

	resetScriptEnv();
	return 0;
}

int32_t LuaScriptInterface::getEvent(const std::string& eventName)
{
	// get our events table
	lua_rawgeti(luaState, LUA_REGISTRYINDEX, eventTableRef);
	if (!isTable(luaState, -1)) {
		lua_pop(luaState, 1);
		return -1;
	}

	// get current event function pointer
	lua_getglobal(luaState, eventName.c_str());
	if (!isFunction(luaState, -1)) {
		lua_pop(luaState, 2);
		return -1;
	}

	// save in our events table
	lua_pushvalue(luaState, -1);
	lua_rawseti(luaState, -3, runningEventId);
	lua_pop(luaState, 2);

	// reset global value of this event
	lua_pushnil(luaState);
	lua_setglobal(luaState, eventName.c_str());

	cacheFiles[runningEventId] = loadingFile + ":" + eventName;
	return runningEventId++;
}

int32_t LuaScriptInterface::getEvent()
{
	// check if function is on the stack
	if (!isFunction(luaState, -1)) {
		return -1;
	}

	// get our events table
	lua_rawgeti(luaState, LUA_REGISTRYINDEX, eventTableRef);
	if (!isTable(luaState, -1)) {
		lua_pop(luaState, 1);
		return -1;
	}

	// save in our events table
	lua_pushvalue(luaState, -2);
	lua_rawseti(luaState, -2, runningEventId);
	lua_pop(luaState, 2);

	cacheFiles[runningEventId] = loadingFile + ":callback";
	return runningEventId++;
}

int32_t LuaScriptInterface::getMetaEvent(const std::string& globalName, const std::string& eventName)
{
	// get our events table
	lua_rawgeti(luaState, LUA_REGISTRYINDEX, eventTableRef);
	if (!isTable(luaState, -1)) {
		lua_pop(luaState, 1);
		return -1;
	}

	// get current event function pointer
	lua_getglobal(luaState, globalName.c_str());
	lua_getfield(luaState, -1, eventName.c_str());
	if (!isFunction(luaState, -1)) {
		lua_pop(luaState, 3);
		return -1;
	}

	// save in our events table
	lua_pushvalue(luaState, -1);
	lua_rawseti(luaState, -4, runningEventId);
	lua_pop(luaState, 1);

	// reset global value of this event
	lua_pushnil(luaState);
	lua_setfield(luaState, -2, eventName.c_str());
	lua_pop(luaState, 2);

	cacheFiles[runningEventId] = loadingFile + ":" + globalName + "@" + eventName;
	return runningEventId++;
}

const std::string& LuaScriptInterface::getFileById(int32_t scriptId)
{
	if (scriptId == EVENT_ID_LOADING) {
		return loadingFile;
	}

	auto it = cacheFiles.find(scriptId);
	if (it == cacheFiles.end()) {
		static const std::string& unk = "(Unknown scriptfile)";
		return unk;
	}
	return it->second;
}

std::string Lua::getStackTrace(lua_State* L, const std::string& error_desc)
{
	luaL_traceback(L, L, error_desc.c_str(), 1);
	return LuaScriptInterface::popString(L);
}

void LuaScriptInterface::reportError(const char* function, const std::string& error_desc, lua_State* L /*= nullptr*/,
                                     bool stack_trace /*= false*/)
{
	int32_t scriptId;
	int32_t callbackId;
	bool timerEvent;
	LuaScriptInterface* scriptInterface;
	getScriptEnv()->getEventInfo(scriptId, scriptInterface, callbackId, timerEvent);

	std::cout << std::endl << "Lua Script Error: ";

	if (scriptInterface) {
		std::cout << '[' << scriptInterface->getInterfaceName() << "] " << std::endl;

		if (timerEvent) {
			std::cout << "in a timer event called from: " << std::endl;
		}

		if (callbackId) {
			std::cout << "in callback: " << scriptInterface->getFileById(callbackId) << std::endl;
		}

		std::cout << scriptInterface->getFileById(scriptId) << std::endl;
	}

	if (function) {
		std::cout << function << "(). ";
	}

	if (L && stack_trace) {
		std::cout << getStackTrace(L, error_desc) << std::endl;
	} else {
		std::cout << error_desc << std::endl;
	}
}

bool LuaScriptInterface::pushFunction(int32_t functionId)
{
	lua_rawgeti(luaState, LUA_REGISTRYINDEX, eventTableRef);
	if (!isTable(luaState, -1)) {
		return false;
	}

	lua_rawgeti(luaState, -1, functionId);
	lua_replace(luaState, -2);
	return isFunction(luaState, -1);
}

bool LuaScriptInterface::initState()
{
	luaState = g_luaEnvironment.getLuaState();
	if (!luaState) {
		return false;
	}

	lua_newtable(luaState);
	eventTableRef = luaL_ref(luaState, LUA_REGISTRYINDEX);
	runningEventId = EVENT_ID_USER;
	return true;
}

bool LuaScriptInterface::closeState()
{
	if (!g_luaEnvironment.getLuaState() || !luaState) {
		return false;
	}

	cacheFiles.clear();
	if (eventTableRef != -1) {
		luaL_unref(luaState, LUA_REGISTRYINDEX, eventTableRef);
		eventTableRef = -1;
	}

	luaState = nullptr;
	return true;
}

int LuaScriptInterface::luaErrorHandler(lua_State* L)
{
	const std::string& errorMessage = popString(L);
	pushString(L, Lua::getStackTrace(L, errorMessage));
	return 1;
}

bool LuaScriptInterface::callFunction(int params)
{
	bool result = false;
	int size = lua_gettop(luaState);
	if (protectedCall(luaState, params, 1) != 0) {
		LuaScriptInterface::reportError(nullptr, getString(luaState, -1));
	} else {
		result = getBoolean(luaState, -1);
	}

	lua_pop(luaState, 1);
	if ((lua_gettop(luaState) + params + 1) != size) {
		LuaScriptInterface::reportError(nullptr, "Stack size changed!");
	}

	resetScriptEnv();
	return result;
}

void LuaScriptInterface::callVoidFunction(int params)
{
	int size = lua_gettop(luaState);
	if (protectedCall(luaState, params, 0) != 0) {
		LuaScriptInterface::reportError(nullptr, LuaScriptInterface::popString(luaState));
	}

	if ((lua_gettop(luaState) + params + 1) != size) {
		LuaScriptInterface::reportError(nullptr, "Stack size changed!");
	}

	resetScriptEnv();
}

void LuaScriptInterface::pushVariant(lua_State* L, const LuaVariant& var)
{
	lua_createtable(L, 0, 2);
	setField(L, "type", var.type());
	switch (var.type()) {
		case VARIANT_NUMBER:
			setField(L, "number", var.getNumber());
			break;
		case VARIANT_STRING:
			setField(L, "string", var.getString());
			break;
		case VARIANT_TARGETPOSITION:
			pushPosition(L, var.getTargetPosition());
			lua_setfield(L, -2, "pos");
			break;
		case VARIANT_POSITION: {
			pushPosition(L, var.getPosition());
			lua_setfield(L, -2, "pos");
			break;
		}
		default:
			break;
	}
	setMetatable(L, -1, "Variant");
}

void LuaScriptInterface::pushThing(lua_State* L, Thing* thing)
{
	if (!thing) {
		lua_createtable(L, 0, 4);
		setField(L, "uid", 0);
		setField(L, "itemid", 0);
		setField(L, "actionid", 0);
		setField(L, "type", 0);
		return;
	}

	if (Item* item = thing->getItem()) {
		pushUserdata<Item>(L, item);
		setItemMetatable(L, -1, item);
	} else if (Creature* creature = thing->getCreature()) {
		pushUserdata<Creature>(L, creature);
		setCreatureMetatable(L, -1, creature);
	} else {
		lua_pushnil(L);
	}
}

void LuaScriptInterface::pushCylinder(lua_State* L, Cylinder* cylinder)
{
	if (Creature* creature = cylinder->getCreature()) {
		pushUserdata<Creature>(L, creature);
		setCreatureMetatable(L, -1, creature);
	} else if (Item* parentItem = cylinder->getItem()) {
		pushUserdata<Item>(L, parentItem);
		setItemMetatable(L, -1, parentItem);
	} else if (Tile* tile = cylinder->getTile()) {
		pushUserdata<Tile>(L, tile);
		setMetatable(L, -1, "Tile");
	} else if (cylinder == VirtualCylinder::virtualCylinder) {
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
}

void Lua::pushString(lua_State* L, const std::string& value) { lua_pushlstring(L, value.c_str(), value.length()); }

void LuaScriptInterface::pushCallback(lua_State* L, int32_t callback) { lua_rawgeti(L, LUA_REGISTRYINDEX, callback); }

std::string LuaScriptInterface::popString(lua_State* L)
{
	if (lua_gettop(L) == 0) {
		return std::string();
	}

	std::string str(getString(L, -1));
	lua_pop(L, 1);
	return str;
}

int32_t LuaScriptInterface::popCallback(lua_State* L) { return luaL_ref(L, LUA_REGISTRYINDEX); }

// Metatables
void Lua::setMetatable(lua_State* L, int32_t index, const std::string& name)
{
	luaL_getmetatable(L, name.c_str());
	lua_setmetatable(L, index - 1);
}

void Lua::setWeakMetatable(lua_State* L, int32_t index, const std::string& name)
{
	static std::set<std::string> weakObjectTypes;
	const std::string& weakName = name + "_weak";

	auto result = weakObjectTypes.emplace(name);
	if (result.second) {
		luaL_getmetatable(L, name.c_str());
		int childMetatable = lua_gettop(L);

		luaL_newmetatable(L, weakName.c_str());
		int metatable = lua_gettop(L);

		static const std::vector<std::string> methodKeys = {"__index", "__metatable", "__eq"};
		for (const std::string& metaKey : methodKeys) {
			lua_getfield(L, childMetatable, metaKey.c_str());
			lua_setfield(L, metatable, metaKey.c_str());
		}

		static const std::vector<int> methodIndexes = {'h', 'p', 't'};
		for (int metaIndex : methodIndexes) {
			lua_rawgeti(L, childMetatable, metaIndex);
			lua_rawseti(L, metatable, metaIndex);
		}

		lua_pushnil(L);
		lua_setfield(L, metatable, "__gc");

		lua_remove(L, childMetatable);
	} else {
		luaL_getmetatable(L, weakName.c_str());
	}
	lua_setmetatable(L, index - 1);
}

void Lua::setItemMetatable(lua_State* L, int32_t index, const Item* item)
{
	if (item->getContainer()) {
		luaL_getmetatable(L, "Container");
	} else if (item->getTeleport()) {
		luaL_getmetatable(L, "Teleport");
	} else if (item->getPodium()) {
		luaL_getmetatable(L, "Podium");
	} else {
		luaL_getmetatable(L, "Item");
	}
	lua_setmetatable(L, index - 1);
}

void Lua::setCreatureMetatable(lua_State* L, int32_t index, const Creature* creature)
{
	if (creature->getPlayer()) {
		luaL_getmetatable(L, "Player");
	} else if (creature->getMonster()) {
		luaL_getmetatable(L, "Monster");
	} else {
		luaL_getmetatable(L, "Npc");
	}
	lua_setmetatable(L, index - 1);
}

// Get
std::string Lua::getString(lua_State* L, int32_t arg)
{
	size_t len;
	const char* c_str = lua_tolstring(L, arg, &len);
	if (!c_str || len == 0) {
		return std::string();
	}
	return std::string(c_str, len);
}

Position Lua::getPosition(lua_State* L, int32_t arg, int32_t& stackpos)
{
	Position position;
	position.x = getField<uint16_t>(L, arg, "x");
	position.y = getField<uint16_t>(L, arg, "y");
	position.z = getField<uint8_t>(L, arg, "z");

	lua_getfield(L, arg, "stackpos");
	if (lua_isnil(L, -1) == 1) {
		stackpos = 0;
	} else {
		stackpos = getNumber<int32_t>(L, -1);
	}

	lua_pop(L, 4);
	return position;
}

Position Lua::getPosition(lua_State* L, int32_t arg)
{
	Position position;
	position.x = getField<uint16_t>(L, arg, "x");
	position.y = getField<uint16_t>(L, arg, "y");
	position.z = getField<uint8_t>(L, arg, "z");

	lua_pop(L, 3);
	return position;
}

Outfit_t Lua::getOutfit(lua_State* L, int32_t arg)
{
	Outfit_t outfit;

	outfit.lookType = getField<uint16_t>(L, arg, "lookType");
	outfit.lookTypeEx = getField<uint16_t>(L, arg, "lookTypeEx");

	outfit.lookHead = getField<uint8_t>(L, arg, "lookHead");
	outfit.lookBody = getField<uint8_t>(L, arg, "lookBody");
	outfit.lookLegs = getField<uint8_t>(L, arg, "lookLegs");
	outfit.lookFeet = getField<uint8_t>(L, arg, "lookFeet");
	outfit.lookAddons = getField<uint8_t>(L, arg, "lookAddons");

	outfit.lookMount = getField<uint16_t>(L, arg, "lookMount");
	outfit.lookMountHead = getField<uint8_t>(L, arg, "lookMountHead");
	outfit.lookMountBody = getField<uint8_t>(L, arg, "lookMountBody");
	outfit.lookMountLegs = getField<uint8_t>(L, arg, "lookMountLegs");
	outfit.lookMountFeet = getField<uint8_t>(L, arg, "lookMountFeet");

	lua_pop(L, 12);
	return outfit;
}

Outfit Lua::getOutfitClass(lua_State* L, int32_t arg)
{
	uint16_t lookType = getField<uint16_t>(L, arg, "lookType");
	const std::string& name = getFieldString(L, arg, "name");
	bool premium = getField<uint8_t>(L, arg, "premium") == 1;
	bool unlocked = getField<uint8_t>(L, arg, "unlocked") == 1;
	lua_pop(L, 4);
	return Outfit(name, lookType, premium, unlocked);
}

FieldBlock Lua::getFieldBlock(lua_State* L, int32_t arg)
{
	const std::string& name = getFieldString(L, arg, "name");
	uint32_t ticks = getField<uint32_t>(L, arg, "ticks");
	int32_t initDamage = getField<int32_t>(L, arg, "initDamage");
	int32_t count = std::max(1, getField<int32_t>(L, arg, "count"));
	int32_t start = std::max(0, getField<int32_t>(L, arg, "start"));
	int32_t damage = -getField<int32_t>(L, arg, "damage");
	lua_pop(L, 6);
	return FieldBlock(name, ticks, initDamage, count, start, damage);
}

LuaVariant Lua::getVariant(lua_State* L, int32_t arg)
{
	LuaVariant var;
	switch (getField<LuaVariantType_t>(L, arg, "type")) {
		case VARIANT_NUMBER: {
			var.setNumber(getField<uint32_t>(L, arg, "number"));
			lua_pop(L, 2);
			break;
		}

		case VARIANT_STRING: {
			var.setString(getFieldString(L, arg, "string"));
			lua_pop(L, 2);
			break;
		}

		case VARIANT_POSITION:
			lua_getfield(L, arg, "pos");
			var.setPosition(getPosition(L, lua_gettop(L)));
			lua_pop(L, 2);
			break;

		case VARIANT_TARGETPOSITION: {
			lua_getfield(L, arg, "pos");
			var.setTargetPosition(getPosition(L, lua_gettop(L)));
			lua_pop(L, 2);
			break;
		}

		default: {
			var = {};
			lua_pop(L, 1);
			break;
		}
	}
	return var;
}

InstantSpell* Lua::getInstantSpell(lua_State* L, int32_t arg)
{
	InstantSpell* spell = g_spells->getInstantSpellByName(getFieldString(L, arg, "name"));
	lua_pop(L, 1);
	return spell;
}

Reflect Lua::getReflect(lua_State* L, int32_t arg)
{
	uint16_t percent = getField<uint16_t>(L, arg, "percent");
	uint16_t chance = getField<uint16_t>(L, arg, "chance");
	lua_pop(L, 2);
	return Reflect(percent, chance);
}

Thing* Lua::getThing(lua_State* L, int32_t arg)
{
	Thing* thing;
	if (lua_getmetatable(L, arg) != 0) {
		lua_rawgeti(L, -1, 't');
		switch (getNumber<uint32_t>(L, -1)) {
			case LuaData_Item:
				thing = getUserdata<Item>(L, arg);
				break;
			case LuaData_Container:
				thing = getUserdata<Container>(L, arg);
				break;
			case LuaData_Teleport:
				thing = getUserdata<Teleport>(L, arg);
				break;
			case LuaData_Podium:
				thing = getUserdata<Podium>(L, arg);
				break;
			case LuaData_Player:
				thing = getUserdata<Player>(L, arg);
				break;
			case LuaData_Monster:
				thing = getUserdata<Monster>(L, arg);
				break;
			case LuaData_Npc:
				thing = getUserdata<Npc>(L, arg);
				break;
			default:
				thing = nullptr;
				break;
		}
		lua_pop(L, 2);
	} else {
		thing = LuaScriptInterface::getScriptEnv()->getThingByUID(getNumber<uint32_t>(L, arg));
	}
	return thing;
}

Creature* Lua::getCreature(lua_State* L, int32_t arg)
{
	if (isUserdata(L, arg)) {
		return getUserdata<Creature>(L, arg);
	}
	return g_game.getCreatureByID(getNumber<uint32_t>(L, arg));
}

Player* Lua::getPlayer(lua_State* L, int32_t arg)
{
	if (isUserdata(L, arg)) {
		return getUserdata<Player>(L, arg);
	}
	return g_game.getPlayerByID(getNumber<uint32_t>(L, arg));
}

std::string Lua::getFieldString(lua_State* L, int32_t arg, const std::string& key)
{
	lua_getfield(L, arg, key.c_str());
	return getString(L, -1);
}

LuaDataType Lua::getUserdataType(lua_State* L, int32_t arg)
{
	if (lua_getmetatable(L, arg) == 0) {
		return LuaData_Unknown;
	}
	lua_rawgeti(L, -1, 't');

	LuaDataType type = getNumber<LuaDataType>(L, -1);
	lua_pop(L, 2);

	return type;
}

// Push
void Lua::pushBoolean(lua_State* L, bool value) { lua_pushboolean(L, value ? 1 : 0); }

void Lua::pushCombatDamage(lua_State* L, const CombatDamage& damage)
{
	lua_pushnumber(L, damage.primary.value);
	lua_pushnumber(L, damage.primary.type);
	lua_pushnumber(L, damage.secondary.value);
	lua_pushnumber(L, damage.secondary.type);
	lua_pushnumber(L, damage.origin);
}

void Lua::pushInstantSpell(lua_State* L, const InstantSpell& spell)
{
	lua_createtable(L, 0, 7);

	setField(L, "name", spell.getName());
	setField(L, "words", spell.getWords());
	setField(L, "level", spell.getLevel());
	setField(L, "mlevel", spell.getMagicLevel());
	setField(L, "mana", spell.getMana());
	setField(L, "manapercent", spell.getManaPercent());
	setField(L, "params", spell.getHasParam());

	setMetatable(L, -1, "Spell");
}

void Lua::pushPosition(lua_State* L, const Position& position, int32_t stackpos /* = 0*/)
{
	lua_createtable(L, 0, 4);

	setField(L, "x", position.x);
	setField(L, "y", position.y);
	setField(L, "z", position.z);
	setField(L, "stackpos", stackpos);

	setMetatable(L, -1, "Position");
}

void Lua::pushOutfit(lua_State* L, const Outfit_t& outfit)
{
	lua_createtable(L, 0, 12);
	setField(L, "lookType", outfit.lookType);
	setField(L, "lookTypeEx", outfit.lookTypeEx);
	setField(L, "lookHead", outfit.lookHead);
	setField(L, "lookBody", outfit.lookBody);
	setField(L, "lookLegs", outfit.lookLegs);
	setField(L, "lookFeet", outfit.lookFeet);
	setField(L, "lookAddons", outfit.lookAddons);
	setField(L, "lookMount", outfit.lookMount);
	setField(L, "lookMountHead", outfit.lookMountHead);
	setField(L, "lookMountBody", outfit.lookMountBody);
	setField(L, "lookMountLegs", outfit.lookMountLegs);
	setField(L, "lookMountFeet", outfit.lookMountFeet);
}

void Lua::pushOutfit(lua_State* L, const Outfit* outfit)
{
	lua_createtable(L, 0, 4);
	setField(L, "lookType", outfit->lookType);
	setField(L, "name", outfit->name);
	setField(L, "premium", outfit->premium);
	setField(L, "unlocked", outfit->unlocked);
	setMetatable(L, -1, "Outfit");
}

void Lua::pushMount(lua_State* L, const Mount* mount)
{
	lua_createtable(L, 0, 5);
	setField(L, "name", mount->name);
	setField(L, "speed", mount->speed);
	setField(L, "clientId", mount->clientId);
	setField(L, "id", mount->id);
	setField(L, "premium", mount->premium);
}

void Lua::pushLoot(lua_State* L, const std::vector<LootBlock>& lootList)
{
	lua_createtable(L, lootList.size(), 0);

	int index = 0;
	for (const auto& lootBlock : lootList) {
		lua_createtable(L, 0, 7);

		setField(L, "itemId", lootBlock.id);
		setField(L, "chance", lootBlock.chance);
		setField(L, "subType", lootBlock.subType);
		setField(L, "maxCount", lootBlock.countmax);
		setField(L, "actionId", lootBlock.actionId);
		setField(L, "text", lootBlock.text);

		pushLoot(L, lootBlock.childLoot);
		lua_setfield(L, -2, "childLoot");

		lua_rawseti(L, -2, ++index);
	}
}

void Lua::pushFieldBlock(lua_State* L, const FieldBlock& fieldBlock)
{
	lua_createtable(L, 0, 6);

	setField(L, "name", fieldBlock.name);
	setField(L, "initDamage", fieldBlock.initDamage);
	setField(L, "ticks", fieldBlock.ticks);
	setField(L, "count", fieldBlock.count);
	setField(L, "start", fieldBlock.start);
	setField(L, "damage", fieldBlock.damage);
}

void Lua::pushReflect(lua_State* L, const Reflect& reflect)
{
	lua_createtable(L, 0, 2);
	setField(L, "percent", reflect.percent);
	setField(L, "chance", reflect.chance);
}

void LuaScriptInterface::registerFunctions()
{
	// doPlayerAddItem(uid, itemid, <optional: default: 1> count/subtype)
	// doPlayerAddItem(cid, itemid, <optional: default: 1> count, <optional: default: 1> canDropOnMap, <optional:
	// default: 1>subtype) Returns uid of the created item
	lua_register(luaState, "doPlayerAddItem", LuaScriptInterface::luaDoPlayerAddItem);

	// isValidUID(uid)
	lua_register(luaState, "isValidUID", LuaScriptInterface::luaIsValidUID);

	// isDepot(uid)
	lua_register(luaState, "isDepot", LuaScriptInterface::luaIsDepot);

	// isMovable(uid)
	lua_register(luaState, "isMovable", LuaScriptInterface::luaIsMoveable);

	// doAddContainerItem(uid, itemid, <optional> count/subtype)
	lua_register(luaState, "doAddContainerItem", LuaScriptInterface::luaDoAddContainerItem);

	// getDepotId(uid)
	lua_register(luaState, "getDepotId", LuaScriptInterface::luaGetDepotId);

	// getWorldTime()
	lua_register(luaState, "getWorldTime", LuaScriptInterface::luaGetWorldTime);

	// getWorldLight()
	lua_register(luaState, "getWorldLight", LuaScriptInterface::luaGetWorldLight);

	// setWorldLight(level, color)
	lua_register(luaState, "setWorldLight", LuaScriptInterface::luaSetWorldLight);

	// getWorldUpTime()
	lua_register(luaState, "getWorldUpTime", LuaScriptInterface::luaGetWorldUpTime);

	// getSubTypeName(subType)
	lua_register(luaState, "getSubTypeName", LuaScriptInterface::luaGetSubTypeName);

	// createCombatArea({area}, <optional> {extArea})
	lua_register(luaState, "createCombatArea", LuaScriptInterface::luaCreateCombatArea);

	// doAreaCombat(cid, type, pos, area, min, max, effect[, origin = ORIGIN_SPELL[, blockArmor = false[, blockShield =
	// false[, ignoreResistances = false]]]])
	lua_register(luaState, "doAreaCombat", LuaScriptInterface::luaDoAreaCombat);

	// doTargetCombat(cid, target, type, min, max, effect[, origin = ORIGIN_SPELL[, blockArmor = false[, blockShield =
	// false[, ignoreResistances = false]]]])
	lua_register(luaState, "doTargetCombat", LuaScriptInterface::luaDoTargetCombat);

	// doChallengeCreature(cid, target[, force = false])
	lua_register(luaState, "doChallengeCreature", LuaScriptInterface::luaDoChallengeCreature);

	// addEvent(callback, delay, ...)
	lua_register(luaState, "addEvent", LuaScriptInterface::luaAddEvent);

	// stopEvent(eventid)
	lua_register(luaState, "stopEvent", LuaScriptInterface::luaStopEvent);

	// saveServer()
	lua_register(luaState, "saveServer", LuaScriptInterface::luaSaveServer);

	// cleanMap()
	lua_register(luaState, "cleanMap", LuaScriptInterface::luaCleanMap);

	// debugPrint(text)
	lua_register(luaState, "debugPrint", LuaScriptInterface::luaDebugPrint);

	// isInWar(cid, target)
	lua_register(luaState, "isInWar", LuaScriptInterface::luaIsInWar);

	// getWaypointPosition(name)
	lua_register(luaState, "getWaypointPositionByName", LuaScriptInterface::luaGetWaypointPositionByName);

	// sendChannelMessage(channelId, type, message)
	lua_register(luaState, "sendChannelMessage", LuaScriptInterface::luaSendChannelMessage);

	// sendGuildChannelMessage(guildId, type, message)
	lua_register(luaState, "sendGuildChannelMessage", LuaScriptInterface::luaSendGuildChannelMessage);

	// isScriptsInterface()
	lua_register(luaState, "isScriptsInterface", LuaScriptInterface::luaIsScriptsInterface);

#ifndef LUAJIT_VERSION
	// bit operations for Lua, based on bitlib project release 24
	// bit.bnot, bit.band, bit.bor, bit.bxor, bit.lshift, bit.rshift
	luaL_register(luaState, "bit", LuaScriptInterface::luaBitReg);
	lua_pop(luaState, 1);
#endif

	// configManager table
	luaL_register(luaState, "configManager", LuaScriptInterface::luaConfigManagerTable);
	lua_pop(luaState, 1);

	// db table
	luaL_register(luaState, "db", LuaScriptInterface::luaDatabaseTable);
	lua_pop(luaState, 1);

	// result table
	luaL_register(luaState, "result", LuaScriptInterface::luaResultTable);
	lua_pop(luaState, 1);

	/* New functions */
	// registerClass(className, baseClass, newFunction)
	// registerTable(tableName)
	// registerMethod(className, functionName, function)
	// registerMetaMethod(className, functionName, function)
	// registerGlobalMethod(functionName, function)
	// registerVariable(tableName, name, value)
	// registerGlobalVariable(name, value)
	// registerEnum(value)
	// registerEnumIn(tableName, value)

	// _G
	registerGlobalVariable("INDEX_WHEREEVER", INDEX_WHEREEVER);
	registerGlobalBoolean("VIRTUAL_PARENT", true);

	registerGlobalMethod("isType", LuaScriptInterface::luaIsType);
	registerGlobalMethod("rawgetmetatable", LuaScriptInterface::luaRawGetMetatable);

	// os
	registerMethod("os", "mtime", LuaScriptInterface::luaSystemTime);

	// table
	registerMethod("table", "create", LuaScriptInterface::luaTableCreate);
	registerMethod("table", "pack", LuaScriptInterface::luaTablePack);

	// Variant
	registerClass("Variant", "", LuaScriptInterface::luaVariantCreate);

	registerMethod("Variant", "getNumber", LuaScriptInterface::luaVariantGetNumber);
	registerMethod("Variant", "getString", LuaScriptInterface::luaVariantGetString);
	registerMethod("Variant", "getPosition", LuaScriptInterface::luaVariantGetPosition);

	// Outfit
	registerClass("Outfit", "", LuaScriptInterface::luaOutfitCreate);
	registerMetaMethod("Outfit", "__eq", LuaScriptInterface::luaOutfitCompare);

	LuaEnums::registerEnums(this);

	LuaGame::registerFunctions(this);

	LuaPosition::registerFunctions(this);

	LuaTile::registerFunctions(this);

	LuaNetworkMessage::registerFunctions(this);

	LuaModalWindow::registerFunctions(this);

	LuaItem::registerFunctions(this);

	LuaContainer::registerFunctions(this);

	LuaCreature::registerFunctions(this);

	LuaPlayer::registerFunctions(this);

	LuaMonster::registerFunctions(this);

	LuaNpc::registerFunctions(this);

	LuaGuild::registerFunctions(this);

	LuaGroup::registerFunctions(this);

	LuaVocation::registerFunctions(this);

	LuaTown::registerFunctions(this);

	LuaHouse::registerFunctions(this);

	LuaItemType::registerFunctions(this);

	LuaCombat::registerFunctions(this);

	LuaCondition::registerFunctions(this);

	LuaMonsterType::registerFunctions(this);

	LuaLoot::registerFunctions(this);

	LuaMonsterSpell::registerFunctions(this);

	LuaParty::registerFunctions(this);

	LuaSpell::registerFunctions(this);

	LuaAction::registerFunctions(this);

	LuaTalkAction::registerFunctions(this);

	LuaCreatureEvent::registerFunctions(this);

	LuaMoveEvent::registerFunctions(this);

	LuaGlobalEvent::registerFunctions(this);

	LuaWeapon::registerFunctions(this);

	LuaXml::registerFunctions(this);

	// Teleport
	registerClass("Teleport", "Item", LuaScriptInterface::luaTeleportCreate);
	registerMetaMethod("Teleport", "__eq", LuaScriptInterface::luaUserdataCompare);

	registerMethod("Teleport", "getDestination", LuaScriptInterface::luaTeleportGetDestination);
	registerMethod("Teleport", "setDestination", LuaScriptInterface::luaTeleportSetDestination);

	// Podium
	registerClass("Podium", "Item", LuaScriptInterface::luaPodiumCreate);
	registerMetaMethod("Podium", "__eq", LuaScriptInterface::luaUserdataCompare);

	registerMethod("Podium", "getOutfit", LuaScriptInterface::luaPodiumGetOutfit);
	registerMethod("Podium", "setOutfit", LuaScriptInterface::luaPodiumSetOutfit);
	registerMethod("Podium", "hasFlag", LuaScriptInterface::luaPodiumHasFlag);
	registerMethod("Podium", "setFlag", LuaScriptInterface::luaPodiumSetFlag);
	registerMethod("Podium", "getDirection", LuaScriptInterface::luaPodiumGetDirection);
	registerMethod("Podium", "setDirection", LuaScriptInterface::luaPodiumSetDirection);
	using Http::Responder;
	registerClass("Responder", "", nullptr);
	registerMetaMethod("Responder", "__gc", Responder::luaDelete);
	registerMethod("Responder", "delete", Responder::luaDelete);
	registerMethod("Responder", "send", Responder::luaSend);
	registerMethod("Responder", "getRequestBody", Responder::luaGetRequestBody);
	registerMethod("Responder", "setResponseBody", Responder::luaSetResponseBody);
	registerMethod("Responder", "getRequestField", Responder::luaGetRequestField);
	registerMethod("Responder", "setResponseField", Responder::luaSetResponseField);
	registerMethod("Responder", "getRequestMethod", Responder::luaGetRequestMethod);
	registerMethod("Responder", "setResponseStatus", Responder::luaSetResponseStatus);
	registerMethod("Responder", "getRequestURL", Responder::luaGetRequestURL);
}

void LuaScriptInterface::registerClass(const std::string& className, const std::string& baseClass,
                                       lua_CFunction newFunction /* = nullptr*/)
{
	// className = {}
	lua_newtable(luaState);
	lua_pushvalue(luaState, -1);
	lua_setglobal(luaState, className.c_str());
	int methods = lua_gettop(luaState);

	// methodsTable = {}
	lua_newtable(luaState);
	int methodsTable = lua_gettop(luaState);

	if (newFunction) {
		// className.__call = newFunction
		lua_pushcfunction(luaState, newFunction);
		lua_setfield(luaState, methodsTable, "__call");
	}

	uint32_t parents = 0;
	if (!baseClass.empty()) {
		lua_getglobal(luaState, baseClass.c_str());
		lua_rawgeti(luaState, -1, 'p');
		parents = getNumber<uint32_t>(luaState, -1) + 1;
		lua_pop(luaState, 1);
		lua_setfield(luaState, methodsTable, "__index");
	}

	// setmetatable(className, methodsTable)
	lua_setmetatable(luaState, methods);

	// className.metatable = {}
	luaL_newmetatable(luaState, className.c_str());
	int metatable = lua_gettop(luaState);

	// className.metatable.__metatable = className
	lua_pushvalue(luaState, methods);
	lua_setfield(luaState, metatable, "__metatable");

	// className.metatable.__index = className
	lua_pushvalue(luaState, methods);
	lua_setfield(luaState, metatable, "__index");

	// className.metatable['h'] = hash
	lua_pushnumber(luaState, std::hash<std::string>()(className));
	lua_rawseti(luaState, metatable, 'h');

	// className.metatable['p'] = parents
	lua_pushnumber(luaState, parents);
	lua_rawseti(luaState, metatable, 'p');

	// className.metatable['t'] = type
	if (className == "Item") {
		lua_pushnumber(luaState, LuaData_Item);
	} else if (className == "Container") {
		lua_pushnumber(luaState, LuaData_Container);
	} else if (className == "Teleport") {
		lua_pushnumber(luaState, LuaData_Teleport);
	} else if (className == "Podium") {
		lua_pushnumber(luaState, LuaData_Podium);
	} else if (className == "Player") {
		lua_pushnumber(luaState, LuaData_Player);
	} else if (className == "Monster") {
		lua_pushnumber(luaState, LuaData_Monster);
	} else if (className == "Npc") {
		lua_pushnumber(luaState, LuaData_Npc);
	} else if (className == "Tile") {
		lua_pushnumber(luaState, LuaData_Tile);
	} else {
		lua_pushnumber(luaState, LuaData_Unknown);
	}
	lua_rawseti(luaState, metatable, 't');

	// pop className, className.metatable
	lua_pop(luaState, 2);
}

void LuaScriptInterface::registerTable(const std::string& tableName)
{
	// _G[tableName] = {}
	lua_newtable(luaState);
	lua_setglobal(luaState, tableName.c_str());
}

void LuaScriptInterface::registerMethod(const std::string& globalName, const std::string& methodName,
                                        lua_CFunction func)
{
	// globalName.methodName = func
	lua_getglobal(luaState, globalName.c_str());
	lua_pushcfunction(luaState, func);
	lua_setfield(luaState, -2, methodName.c_str());

	// pop globalName
	lua_pop(luaState, 1);
}

void LuaScriptInterface::registerMetaMethod(const std::string& className, const std::string& methodName,
                                            lua_CFunction func)
{
	// className.metatable.methodName = func
	luaL_getmetatable(luaState, className.c_str());
	lua_pushcfunction(luaState, func);
	lua_setfield(luaState, -2, methodName.c_str());

	// pop className.metatable
	lua_pop(luaState, 1);
}

void LuaScriptInterface::registerGlobalMethod(const std::string& functionName, lua_CFunction func)
{
	// _G[functionName] = func
	lua_pushcfunction(luaState, func);
	lua_setglobal(luaState, functionName.c_str());
}

void LuaScriptInterface::registerVariable(const std::string& tableName, const std::string& name, lua_Number value)
{
	// tableName.name = value
	lua_getglobal(luaState, tableName.c_str());
	setField(luaState, name.c_str(), value);

	// pop tableName
	lua_pop(luaState, 1);
}

void LuaScriptInterface::registerGlobalVariable(const std::string& name, lua_Number value)
{
	// _G[name] = value
	lua_pushnumber(luaState, value);
	lua_setglobal(luaState, name.c_str());
}

void LuaScriptInterface::registerGlobalBoolean(const std::string& name, bool value)
{
	// _G[name] = value
	pushBoolean(luaState, value);
	lua_setglobal(luaState, name.c_str());
}

int LuaScriptInterface::luaDoPlayerAddItem(lua_State* L)
{
	// doPlayerAddItem(cid, itemid, <optional: default: 1> count/subtype, <optional: default: 1> canDropOnMap)
	// doPlayerAddItem(cid, itemid, <optional: default: 1> count, <optional: default: 1> canDropOnMap, <optional:
	// default: 1>subtype)
	Player* player = getPlayer(L, 1);
	if (!player) {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_PLAYER_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	uint16_t itemId = getNumber<uint16_t>(L, 2);
	int32_t count = getNumber<int32_t>(L, 3, 1);
	bool canDropOnMap = getBoolean(L, 4, true);
	uint16_t subType = getNumber<uint16_t>(L, 5, 1);

	const ItemType& it = Item::items[itemId];
	int32_t itemCount;

	auto parameters = lua_gettop(L);
	if (parameters > 4) {
		// subtype already supplied, count then is the amount
		itemCount = std::max<int32_t>(1, count);
	} else if (it.hasSubType()) {
		if (it.stackable) {
			itemCount = static_cast<int32_t>(std::ceil(static_cast<float>(count) / 100));
		} else {
			itemCount = 1;
		}
		subType = count;
	} else {
		itemCount = std::max<int32_t>(1, count);
	}

	while (itemCount > 0) {
		uint16_t stackCount = subType;
		if (it.stackable && stackCount > 100) {
			stackCount = 100;
		}

		Item* newItem = Item::CreateItem(itemId, stackCount);
		if (!newItem) {
			reportErrorFunc(L, getErrorDesc(LUA_ERROR_ITEM_NOT_FOUND));
			pushBoolean(L, false);
			return 1;
		}

		if (it.stackable) {
			subType -= stackCount;
		}

		ReturnValue ret = g_game.internalPlayerAddItem(player, newItem, canDropOnMap);
		if (ret != RETURNVALUE_NOERROR) {
			delete newItem;
			pushBoolean(L, false);
			return 1;
		}

		if (--itemCount == 0) {
			if (newItem->getParent()) {
				uint32_t uid = getScriptEnv()->addThing(newItem);
				lua_pushnumber(L, uid);
				return 1;
			} else {
				// stackable item stacked with existing object, newItem will be released
				pushBoolean(L, false);
				return 1;
			}
		}
	}

	pushBoolean(L, false);
	return 1;
}

int LuaScriptInterface::luaDebugPrint(lua_State* L)
{
	// debugPrint(text)
	reportErrorFunc(L, getString(L, -1));
	return 0;
}

int LuaScriptInterface::luaGetWorldTime(lua_State* L)
{
	// getWorldTime()
	int16_t time = g_game.getWorldTime();
	lua_pushnumber(L, time);
	return 1;
}

int LuaScriptInterface::luaGetWorldLight(lua_State* L)
{
	// getWorldLight()
	LightInfo lightInfo = g_game.getWorldLightInfo();
	lua_pushnumber(L, lightInfo.level);
	lua_pushnumber(L, lightInfo.color);
	return 2;
}

int LuaScriptInterface::luaSetWorldLight(lua_State* L)
{
	// setWorldLight(level, color)
	if (g_config.getBoolean(ConfigManager::DEFAULT_WORLD_LIGHT)) {
		pushBoolean(L, false);
		return 1;
	}

	LightInfo lightInfo;
	lightInfo.level = getNumber<uint8_t>(L, 1);
	lightInfo.color = getNumber<uint8_t>(L, 2);
	g_game.setWorldLightInfo(lightInfo);
	pushBoolean(L, true);
	return 1;
}

int LuaScriptInterface::luaGetWorldUpTime(lua_State* L)
{
	// getWorldUpTime()
	uint64_t uptime = (OTSYS_TIME() - ProtocolStatus::start) / 1000;
	lua_pushnumber(L, uptime);
	return 1;
}

int LuaScriptInterface::luaGetSubTypeName(lua_State* L)
{
	// getSubTypeName(subType)
	int32_t subType = getNumber<int32_t>(L, 1);
	if (subType > 0) {
		pushString(L, Item::items[subType].name);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

bool LuaScriptInterface::getArea(lua_State* L, std::vector<uint32_t>& vec, uint32_t& rows)
{
	lua_pushnil(L);
	for (rows = 0; lua_next(L, -2) != 0; ++rows) {
		if (!isTable(L, -1)) {
			return false;
		}

		lua_pushnil(L);
		while (lua_next(L, -2) != 0) {
			if (!isNumber(L, -1)) {
				return false;
			}
			vec.push_back(getNumber<uint32_t>(L, -1));
			lua_pop(L, 1);
		}

		lua_pop(L, 1);
	}

	lua_pop(L, 1);
	return (rows != 0);
}

int LuaScriptInterface::luaCreateCombatArea(lua_State* L)
{
	// createCombatArea({area}, <optional> {extArea})
	ScriptEnvironment* env = getScriptEnv();
	if (env->getScriptId() != EVENT_ID_LOADING) {
		reportErrorFunc(L, "This function can only be used while loading the script.");
		pushBoolean(L, false);
		return 1;
	}

	uint32_t areaId = g_luaEnvironment.createAreaObject(env->getScriptInterface());
	AreaCombat* area = g_luaEnvironment.getAreaObject(areaId);

	int parameters = lua_gettop(L);
	if (parameters >= 2) {
		uint32_t rowsExtArea;
		std::vector<uint32_t> vecExtArea;
		if (!isTable(L, 2) || !getArea(L, vecExtArea, rowsExtArea)) {
			reportErrorFunc(L, "Invalid extended area table.");
			pushBoolean(L, false);
			return 1;
		}
		area->setupExtArea(vecExtArea, rowsExtArea);
	}

	uint32_t rowsArea = 0;
	std::vector<uint32_t> vecArea;
	if (!isTable(L, 1) || !getArea(L, vecArea, rowsArea)) {
		reportErrorFunc(L, "Invalid area table.");
		pushBoolean(L, false);
		return 1;
	}

	area->setupArea(vecArea, rowsArea);
	lua_pushnumber(L, areaId);
	return 1;
}

int LuaScriptInterface::luaDoAreaCombat(lua_State* L)
{
	// doAreaCombat(cid, type, pos, area, min, max, effect[, origin = ORIGIN_SPELL[, blockArmor = false[, blockShield =
	// false[, ignoreResistances = false]]]])
	Creature* creature = getCreature(L, 1);
	if (!creature && (!isNumber(L, 1) || getNumber<uint32_t>(L, 1) != 0)) {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	uint32_t areaId = getNumber<uint32_t>(L, 4);
	const AreaCombat* area = g_luaEnvironment.getAreaObject(areaId);
	if (area || areaId == 0) {
		CombatType_t combatType = getNumber<CombatType_t>(L, 2);

		CombatParams params;
		params.combatType = combatType;
		params.impactEffect = getNumber<uint8_t>(L, 7);
		params.blockedByArmor = getBoolean(L, 8, false);
		params.blockedByShield = getBoolean(L, 9, false);
		params.ignoreResistances = getBoolean(L, 10, false);

		CombatDamage damage;
		damage.origin = getNumber<CombatOrigin>(L, 8, ORIGIN_SPELL);
		damage.primary.type = combatType;
		damage.primary.value = normal_random(getNumber<int32_t>(L, 6), getNumber<int32_t>(L, 5));

		Combat::doAreaCombat(creature, getPosition(L, 3), area, damage, params);
		pushBoolean(L, true);
	} else {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_AREA_NOT_FOUND));
		pushBoolean(L, false);
	}
	return 1;
}

int LuaScriptInterface::luaDoTargetCombat(lua_State* L)
{
	// doTargetCombat(cid, target, type, min, max, effect[, origin = ORIGIN_SPELL[, blockArmor = false[, blockShield =
	// false[, ignoreResistances = false]]]])
	Creature* creature = getCreature(L, 1);
	if (!creature && (!isNumber(L, 1) || getNumber<uint32_t>(L, 1) != 0)) {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	Creature* target = getCreature(L, 2);
	if (!target) {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	CombatType_t combatType = getNumber<CombatType_t>(L, 3);

	CombatParams params;
	params.combatType = combatType;
	params.impactEffect = getNumber<uint8_t>(L, 6);
	params.blockedByArmor = getBoolean(L, 8, false);
	params.blockedByShield = getBoolean(L, 9, false);
	params.ignoreResistances = getBoolean(L, 10, false);

	CombatDamage damage;
	damage.origin = getNumber<CombatOrigin>(L, 7, ORIGIN_SPELL);
	damage.primary.type = combatType;
	damage.primary.value = normal_random(getNumber<int32_t>(L, 4), getNumber<int32_t>(L, 5));

	Combat::doTargetCombat(creature, target, damage, params);
	pushBoolean(L, true);
	return 1;
}

int LuaScriptInterface::luaDoChallengeCreature(lua_State* L)
{
	// doChallengeCreature(cid, target[, force = false])
	Creature* creature = getCreature(L, 1);
	if (!creature) {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	Creature* target = getCreature(L, 2);
	if (!target) {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	target->challengeCreature(creature, getBoolean(L, 3, false));
	pushBoolean(L, true);
	return 1;
}

int LuaScriptInterface::luaIsValidUID(lua_State* L)
{
	// isValidUID(uid)
	pushBoolean(L, getScriptEnv()->getThingByUID(getNumber<uint32_t>(L, -1)) != nullptr);
	return 1;
}

int LuaScriptInterface::luaIsDepot(lua_State* L)
{
	// isDepot(uid)
	Container* container = getScriptEnv()->getContainerByUID(getNumber<uint32_t>(L, -1));
	pushBoolean(L, container && container->getDepotLocker());
	return 1;
}

int LuaScriptInterface::luaIsMoveable(lua_State* L)
{
	// isMoveable(uid)
	// isMovable(uid)
	Thing* thing = getScriptEnv()->getThingByUID(getNumber<uint32_t>(L, -1));
	pushBoolean(L, thing && thing->isPushable());
	return 1;
}

int LuaScriptInterface::luaDoAddContainerItem(lua_State* L)
{
	// doAddContainerItem(uid, itemid, <optional> count/subtype)
	uint32_t uid = getNumber<uint32_t>(L, 1);

	ScriptEnvironment* env = getScriptEnv();
	Container* container = env->getContainerByUID(uid);
	if (!container) {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_CONTAINER_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	uint16_t itemId = getNumber<uint16_t>(L, 2);
	const ItemType& it = Item::items[itemId];

	int32_t itemCount = 1;
	int32_t subType = 1;
	uint32_t count = getNumber<uint32_t>(L, 3, 1);

	if (it.hasSubType()) {
		if (it.stackable) {
			itemCount = static_cast<int32_t>(std::ceil(static_cast<float>(count) / 100));
		}

		subType = count;
	} else {
		itemCount = std::max<int32_t>(1, count);
	}

	while (itemCount > 0) {
		int32_t stackCount = std::min<int32_t>(100, subType);
		Item* newItem = Item::CreateItem(itemId, stackCount);
		if (!newItem) {
			reportErrorFunc(L, getErrorDesc(LUA_ERROR_ITEM_NOT_FOUND));
			pushBoolean(L, false);
			return 1;
		}

		if (it.stackable) {
			subType -= stackCount;
		}

		ReturnValue ret = g_game.internalAddItem(container, newItem);
		if (ret != RETURNVALUE_NOERROR) {
			delete newItem;
			pushBoolean(L, false);
			return 1;
		}

		if (--itemCount == 0) {
			if (newItem->getParent()) {
				lua_pushnumber(L, env->addThing(newItem));
			} else {
				// stackable item stacked with existing object, newItem will be released
				pushBoolean(L, false);
			}
			return 1;
		}
	}

	pushBoolean(L, false);
	return 1;
}

int LuaScriptInterface::luaGetDepotId(lua_State* L)
{
	// getDepotId(uid)
	uint32_t uid = getNumber<uint32_t>(L, -1);

	Container* container = getScriptEnv()->getContainerByUID(uid);
	if (!container) {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_CONTAINER_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	DepotLocker* depotLocker = container->getDepotLocker();
	if (!depotLocker) {
		reportErrorFunc(L, "Depot not found");
		pushBoolean(L, false);
		return 1;
	}

	lua_pushnumber(L, depotLocker->getDepotId());
	return 1;
}

int LuaScriptInterface::luaAddEvent(lua_State* L)
{
	// addEvent(callback, delay, ...)
	int parameters = lua_gettop(L);
	if (parameters < 2) {
		reportErrorFunc(L, fmt::format("Not enough parameters: {:d}.", parameters));
		pushBoolean(L, false);
		return 1;
	}

	if (!isFunction(L, 1)) {
		reportErrorFunc(L, "callback parameter should be a function.");
		pushBoolean(L, false);
		return 1;
	}

	if (!isNumber(L, 2)) {
		reportErrorFunc(L, "delay parameter should be a number.");
		pushBoolean(L, false);
		return 1;
	}

	if (g_config.getBoolean(ConfigManager::WARN_UNSAFE_SCRIPTS) ||
	    g_config.getBoolean(ConfigManager::CONVERT_UNSAFE_SCRIPTS)) {
		std::vector<std::pair<int32_t, LuaDataType>> indexes;
		for (int i = 3; i <= parameters; ++i) {
			if (lua_getmetatable(L, i) == 0) {
				continue;
			}
			lua_rawgeti(L, -1, 't');

			LuaDataType type = getNumber<LuaDataType>(L, -1);
			if (type != LuaData_Unknown && type != LuaData_Tile) {
				indexes.push_back({i, type});
			}
			lua_pop(L, 2);
		}

		if (!indexes.empty()) {
			if (g_config.getBoolean(ConfigManager::WARN_UNSAFE_SCRIPTS)) {
				bool plural = indexes.size() > 1;

				std::string warningString = "Argument";
				if (plural) {
					warningString += 's';
				}

				for (const auto& entry : indexes) {
					if (entry == indexes.front()) {
						warningString += ' ';
					} else if (entry == indexes.back()) {
						warningString += " and ";
					} else {
						warningString += ", ";
					}
					warningString += '#';
					warningString += std::to_string(entry.first);
				}

				if (plural) {
					warningString += " are unsafe";
				} else {
					warningString += " is unsafe";
				}

				reportErrorFunc(L, warningString);
			}

			if (g_config.getBoolean(ConfigManager::CONVERT_UNSAFE_SCRIPTS)) {
				for (const auto& entry : indexes) {
					switch (entry.second) {
						case LuaData_Item:
						case LuaData_Container:
						case LuaData_Teleport:
						case LuaData_Podium: {
							lua_getglobal(L, "Item");
							lua_getfield(L, -1, "getUniqueId");
							break;
						}
						case LuaData_Player:
						case LuaData_Monster:
						case LuaData_Npc: {
							lua_getglobal(L, "Creature");
							lua_getfield(L, -1, "getId");
							break;
						}
						default:
							break;
					}
					lua_replace(L, -2);
					lua_pushvalue(L, entry.first);
					lua_call(L, 1, 1);
					lua_replace(L, entry.first);
				}
			}
		}
	}

	LuaTimerEventDesc eventDesc;
	eventDesc.parameters.reserve(parameters -
	                             2); // safe to use -2 since we garanteed that there is at least two parameters
	for (int i = 0; i < parameters - 2; ++i) {
		eventDesc.parameters.push_back(luaL_ref(L, LUA_REGISTRYINDEX));
	}

	uint32_t delay = std::max<uint32_t>(100, getNumber<uint32_t>(L, 2));
	lua_pop(L, 1);

	eventDesc.function = luaL_ref(L, LUA_REGISTRYINDEX);
	eventDesc.scriptId = getScriptEnv()->getScriptId();

	auto& lastTimerEventId = g_luaEnvironment.lastEventTimerId;
	eventDesc.eventId = g_scheduler.addEvent(
	    createSchedulerTask(delay, [=]() { g_luaEnvironment.executeTimerEvent(lastTimerEventId); }));

	g_luaEnvironment.timerEvents.emplace(lastTimerEventId, std::move(eventDesc));
	lua_pushnumber(L, lastTimerEventId++);
	return 1;
}

int LuaScriptInterface::luaStopEvent(lua_State* L)
{
	// stopEvent(eventid)
	uint32_t eventId = getNumber<uint32_t>(L, 1);

	auto& timerEvents = g_luaEnvironment.timerEvents;
	auto it = timerEvents.find(eventId);
	if (it == timerEvents.end()) {
		pushBoolean(L, false);
		return 1;
	}

	LuaTimerEventDesc timerEventDesc = std::move(it->second);
	timerEvents.erase(it);

	g_scheduler.stopEvent(timerEventDesc.eventId);
	luaL_unref(L, LUA_REGISTRYINDEX, timerEventDesc.function);

	for (auto parameter : timerEventDesc.parameters) {
		luaL_unref(L, LUA_REGISTRYINDEX, parameter);
	}

	pushBoolean(L, true);
	return 1;
}

int LuaScriptInterface::luaSaveServer(lua_State* L)
{
	g_game.saveGameState();
	pushBoolean(L, true);
	return 1;
}

int LuaScriptInterface::luaCleanMap(lua_State* L)
{
	lua_pushnumber(L, g_game.map.clean());
	return 1;
}

int LuaScriptInterface::luaIsInWar(lua_State* L)
{
	// isInWar(cid, target)
	Player* player = getPlayer(L, 1);
	if (!player) {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_PLAYER_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	Player* targetPlayer = getPlayer(L, 2);
	if (!targetPlayer) {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_PLAYER_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	pushBoolean(L, player->isInWar(targetPlayer));
	return 1;
}

int LuaScriptInterface::luaGetWaypointPositionByName(lua_State* L)
{
	// getWaypointPositionByName(name)
	auto& waypoints = g_game.map.waypoints;

	auto it = waypoints.find(getString(L, -1));
	if (it != waypoints.end()) {
		pushPosition(L, it->second);
	} else {
		pushBoolean(L, false);
	}
	return 1;
}

int LuaScriptInterface::luaSendChannelMessage(lua_State* L)
{
	// sendChannelMessage(channelId, type, message)
	uint32_t channelId = getNumber<uint32_t>(L, 1);
	ChatChannel* channel = g_chat->getChannelById(channelId);
	if (!channel) {
		pushBoolean(L, false);
		return 1;
	}

	SpeakClasses type = getNumber<SpeakClasses>(L, 2);
	std::string message = getString(L, 3);
	channel->sendToAll(message, type);
	pushBoolean(L, true);
	return 1;
}

int LuaScriptInterface::luaSendGuildChannelMessage(lua_State* L)
{
	// sendGuildChannelMessage(guildId, type, message)
	uint32_t guildId = getNumber<uint32_t>(L, 1);
	ChatChannel* channel = g_chat->getGuildChannelById(guildId);
	if (!channel) {
		pushBoolean(L, false);
		return 1;
	}

	SpeakClasses type = getNumber<SpeakClasses>(L, 2);
	std::string message = getString(L, 3);
	channel->sendToAll(message, type);
	pushBoolean(L, true);
	return 1;
}

int LuaScriptInterface::luaIsScriptsInterface(lua_State* L)
{
	// isScriptsInterface()
	if (getScriptEnv()->getScriptInterface() == &g_scripts->getScriptInterface()) {
		pushBoolean(L, true);
	} else {
		reportErrorFunc(L, "EventCallback: can only be called inside (data/scripts/)");
		pushBoolean(L, false);
	}
	return 1;
}

std::string LuaScriptInterface::escapeString(std::string s)
{
	boost::algorithm::replace_all(s, "\\", "\\\\");
	boost::algorithm::replace_all(s, "\"", "\\\"");
	boost::algorithm::replace_all(s, "'", "\\'");
	boost::algorithm::replace_all(s, "[[", "\\[[");
	return s;
}

#ifndef LUAJIT_VERSION
const luaL_Reg LuaScriptInterface::luaBitReg[] = {
    //{"tobit", LuaScriptInterface::luaBitToBit},
    {"bnot", LuaScriptInterface::luaBitNot},
    {"band", LuaScriptInterface::luaBitAnd},
    {"bor", LuaScriptInterface::luaBitOr},
    {"bxor", LuaScriptInterface::luaBitXor},
    {"lshift", LuaScriptInterface::luaBitLeftShift},
    {"rshift", LuaScriptInterface::luaBitRightShift},
    //{"arshift", LuaScriptInterface::luaBitArithmeticalRightShift},
    //{"rol", LuaScriptInterface::luaBitRotateLeft},
    //{"ror", LuaScriptInterface::luaBitRotateRight},
    //{"bswap", LuaScriptInterface::luaBitSwapEndian},
    //{"tohex", LuaScriptInterface::luaBitToHex},
    {nullptr, nullptr}};

int LuaScriptInterface::luaBitNot(lua_State* L)
{
	lua_pushnumber(L, ~getNumber<uint32_t>(L, -1));
	return 1;
}

#define MULTIOP(name, op) \
	int LuaScriptInterface::luaBit##name(lua_State* L) \
	{ \
		int n = lua_gettop(L); \
		uint32_t w = getNumber<uint32_t>(L, -1); \
		for (int i = 1; i < n; ++i) w op getNumber<uint32_t>(L, i); \
		lua_pushnumber(L, w); \
		return 1; \
	}

MULTIOP(And, &=)
MULTIOP(Or, |=)
MULTIOP(Xor, ^=)

#define SHIFTOP(name, op) \
	int LuaScriptInterface::luaBit##name(lua_State* L) \
	{ \
		uint32_t n1 = getNumber<uint32_t>(L, 1), n2 = getNumber<uint32_t>(L, 2); \
		lua_pushnumber(L, (n1 op n2)); \
		return 1; \
	}

SHIFTOP(LeftShift, <<)
SHIFTOP(RightShift, >>)
#endif

const luaL_Reg LuaScriptInterface::luaConfigManagerTable[] = {
    {"getString", LuaScriptInterface::luaConfigManagerGetString},
    {"getNumber", LuaScriptInterface::luaConfigManagerGetNumber},
    {"getBoolean", LuaScriptInterface::luaConfigManagerGetBoolean},
    {nullptr, nullptr}};

int LuaScriptInterface::luaConfigManagerGetString(lua_State* L)
{
	pushString(L, g_config.getString(getNumber<ConfigManager::string_config_t>(L, -1)));
	return 1;
}

int LuaScriptInterface::luaConfigManagerGetNumber(lua_State* L)
{
	lua_pushnumber(L, g_config.getNumber(getNumber<ConfigManager::integer_config_t>(L, -1)));
	return 1;
}

int LuaScriptInterface::luaConfigManagerGetBoolean(lua_State* L)
{
	pushBoolean(L, g_config.getBoolean(getNumber<ConfigManager::boolean_config_t>(L, -1)));
	return 1;
}

const luaL_Reg LuaScriptInterface::luaDatabaseTable[] = {
    {"query", LuaScriptInterface::luaDatabaseExecute},
    {"asyncQuery", LuaScriptInterface::luaDatabaseAsyncExecute},
    {"storeQuery", LuaScriptInterface::luaDatabaseStoreQuery},
    {"asyncStoreQuery", LuaScriptInterface::luaDatabaseAsyncStoreQuery},
    {"escapeString", LuaScriptInterface::luaDatabaseEscapeString},
    {"escapeBlob", LuaScriptInterface::luaDatabaseEscapeBlob},
    {"lastInsertId", LuaScriptInterface::luaDatabaseLastInsertId},
    {"tableExists", LuaScriptInterface::luaDatabaseTableExists},
    {nullptr, nullptr}};

int LuaScriptInterface::luaDatabaseExecute(lua_State* L)
{
	pushBoolean(L, Database::getInstance().executeQuery(getString(L, -1)));
	return 1;
}

int LuaScriptInterface::luaDatabaseAsyncExecute(lua_State* L)
{
	std::function<void(DBResult_ptr, bool)> callback;
	if (lua_gettop(L) > 1) {
		int32_t ref = luaL_ref(L, LUA_REGISTRYINDEX);
		auto scriptId = getScriptEnv()->getScriptId();
		callback = [ref, scriptId](DBResult_ptr, bool success) {
			lua_State* luaState = g_luaEnvironment.getLuaState();
			if (!luaState) {
				return;
			}

			if (!LuaScriptInterface::reserveScriptEnv()) {
				luaL_unref(luaState, LUA_REGISTRYINDEX, ref);
				return;
			}

			lua_rawgeti(luaState, LUA_REGISTRYINDEX, ref);
			pushBoolean(luaState, success);
			auto env = getScriptEnv();
			env->setScriptId(scriptId, &g_luaEnvironment);
			g_luaEnvironment.callFunction(1);

			luaL_unref(luaState, LUA_REGISTRYINDEX, ref);
		};
	}
	g_databaseTasks.addTask(getString(L, -1), callback);
	return 0;
}

int LuaScriptInterface::luaDatabaseStoreQuery(lua_State* L)
{
	if (DBResult_ptr res = Database::getInstance().storeQuery(getString(L, -1))) {
		lua_pushnumber(L, ScriptEnvironment::addResult(res));
	} else {
		pushBoolean(L, false);
	}
	return 1;
}

int LuaScriptInterface::luaDatabaseAsyncStoreQuery(lua_State* L)
{
	std::function<void(DBResult_ptr, bool)> callback;
	if (lua_gettop(L) > 1) {
		int32_t ref = luaL_ref(L, LUA_REGISTRYINDEX);
		auto scriptId = getScriptEnv()->getScriptId();
		callback = [ref, scriptId](DBResult_ptr result, bool) {
			lua_State* luaState = g_luaEnvironment.getLuaState();
			if (!luaState) {
				return;
			}

			if (!LuaScriptInterface::reserveScriptEnv()) {
				luaL_unref(luaState, LUA_REGISTRYINDEX, ref);
				return;
			}

			lua_rawgeti(luaState, LUA_REGISTRYINDEX, ref);
			if (result) {
				lua_pushnumber(luaState, ScriptEnvironment::addResult(result));
			} else {
				pushBoolean(luaState, false);
			}
			auto env = getScriptEnv();
			env->setScriptId(scriptId, &g_luaEnvironment);
			g_luaEnvironment.callFunction(1);

			luaL_unref(luaState, LUA_REGISTRYINDEX, ref);
		};
	}
	g_databaseTasks.addTask(getString(L, -1), callback, true);
	return 0;
}

int LuaScriptInterface::luaDatabaseEscapeString(lua_State* L)
{
	pushString(L, Database::getInstance().escapeString(getString(L, -1)));
	return 1;
}

int LuaScriptInterface::luaDatabaseEscapeBlob(lua_State* L)
{
	uint32_t length = getNumber<uint32_t>(L, 2);
	pushString(L, Database::getInstance().escapeBlob(getString(L, 1).c_str(), length));
	return 1;
}

int LuaScriptInterface::luaDatabaseLastInsertId(lua_State* L)
{
	lua_pushnumber(L, Database::getInstance().getLastInsertId());
	return 1;
}

int LuaScriptInterface::luaDatabaseTableExists(lua_State* L)
{
	pushBoolean(L, DatabaseManager::tableExists(getString(L, -1)));
	return 1;
}

const luaL_Reg LuaScriptInterface::luaResultTable[] = {
    {"getNumber", LuaScriptInterface::luaResultGetNumber}, {"getString", LuaScriptInterface::luaResultGetString},
    {"getStream", LuaScriptInterface::luaResultGetStream}, {"next", LuaScriptInterface::luaResultNext},
    {"free", LuaScriptInterface::luaResultFree},           {nullptr, nullptr}};

int LuaScriptInterface::luaResultGetNumber(lua_State* L)
{
	DBResult_ptr res = ScriptEnvironment::getResultByID(getNumber<uint32_t>(L, 1));
	if (!res) {
		pushBoolean(L, false);
		return 1;
	}

	const std::string& s = getString(L, 2);
	lua_pushnumber(L, res->getNumber<int64_t>(s));
	return 1;
}

int LuaScriptInterface::luaResultGetString(lua_State* L)
{
	DBResult_ptr res = ScriptEnvironment::getResultByID(getNumber<uint32_t>(L, 1));
	if (!res) {
		pushBoolean(L, false);
		return 1;
	}

	const std::string& s = getString(L, 2);
	pushString(L, res->getString(s));
	return 1;
}

int LuaScriptInterface::luaResultGetStream(lua_State* L)
{
	DBResult_ptr res = ScriptEnvironment::getResultByID(getNumber<uint32_t>(L, 1));
	if (!res) {
		pushBoolean(L, false);
		return 1;
	}

	unsigned long length;
	const char* stream = res->getStream(getString(L, 2), length);
	lua_pushlstring(L, stream, length);
	lua_pushnumber(L, length);
	return 2;
}

int LuaScriptInterface::luaResultNext(lua_State* L)
{
	DBResult_ptr res = ScriptEnvironment::getResultByID(getNumber<uint32_t>(L, -1));
	if (!res) {
		pushBoolean(L, false);
		return 1;
	}

	pushBoolean(L, res->next());
	return 1;
}

int LuaScriptInterface::luaResultFree(lua_State* L)
{
	pushBoolean(L, ScriptEnvironment::removeResult(getNumber<uint32_t>(L, -1)));
	return 1;
}

// Userdata
int LuaScriptInterface::luaUserdataCompare(lua_State* L)
{
	// userdataA == userdataB
	pushBoolean(L, getUserdata<void>(L, 1) == getUserdata<void>(L, 2));
	return 1;
}

// _G
int LuaScriptInterface::luaIsType(lua_State* L)
{
	// isType(derived, base)
	lua_getmetatable(L, -2);
	lua_getmetatable(L, -2);

	lua_rawgeti(L, -2, 'p');
	uint_fast8_t parentsB = getNumber<uint_fast8_t>(L, 1);

	lua_rawgeti(L, -3, 'h');
	size_t hashB = getNumber<size_t>(L, 1);

	lua_rawgeti(L, -3, 'p');
	uint_fast8_t parentsA = getNumber<uint_fast8_t>(L, 1);
	for (uint_fast8_t i = parentsA; i < parentsB; ++i) {
		lua_getfield(L, -3, "__index");
		lua_replace(L, -4);
	}

	lua_rawgeti(L, -4, 'h');
	size_t hashA = getNumber<size_t>(L, 1);

	pushBoolean(L, hashA == hashB);
	return 1;
}

int LuaScriptInterface::luaRawGetMetatable(lua_State* L)
{
	// rawgetmetatable(metatableName)
	luaL_getmetatable(L, getString(L, 1).c_str());
	return 1;
}

// os
int LuaScriptInterface::luaSystemTime(lua_State* L)
{
	// os.mtime()
	lua_pushnumber(L, OTSYS_TIME());
	return 1;
}

// table
int LuaScriptInterface::luaTableCreate(lua_State* L)
{
	// table.create(arrayLength, keyLength)
	lua_createtable(L, getNumber<int32_t>(L, 1), getNumber<int32_t>(L, 2));
	return 1;
}

int LuaScriptInterface::luaTablePack(lua_State* L)
{
	// table.pack(...)
	int i;
	int n = lua_gettop(L);     /* number of elements to pack */
	lua_createtable(L, n, 1);  /* create result table */
	lua_insert(L, 1);          /* put it at index 1 */
	for (i = n; i >= 1; i--) { /* assign elements */
		lua_rawseti(L, 1, i);
	}
	if (luaL_callmeta(L, -1, "__index") != 0) {
		lua_replace(L, -2);
	}
	lua_pushinteger(L, n);
	lua_setfield(L, 1, "n"); /* t.n = number of elements */
	return 1;                /* return table */
}

// Variant
int LuaScriptInterface::luaVariantCreate(lua_State* L)
{
	// Variant(number or string or position or thing)
	LuaVariant variant;
	if (isUserdata(L, 2)) {
		if (Thing* thing = getThing(L, 2)) {
			variant.setTargetPosition(thing->getPosition());
		}
	} else if (isTable(L, 2)) {
		variant.setPosition(getPosition(L, 2));
	} else if (isNumber(L, 2)) {
		variant.setNumber(getNumber<uint32_t>(L, 2));
	} else if (isString(L, 2)) {
		variant.setString(getString(L, 2));
	}
	pushVariant(L, variant);
	return 1;
}

int LuaScriptInterface::luaVariantGetNumber(lua_State* L)
{
	// Variant:getNumber()
	const LuaVariant& variant = getVariant(L, 1);
	if (variant.isNumber()) {
		lua_pushnumber(L, variant.getNumber());
	} else {
		lua_pushnumber(L, 0);
	}
	return 1;
}

int LuaScriptInterface::luaVariantGetString(lua_State* L)
{
	// Variant:getString()
	const LuaVariant& variant = getVariant(L, 1);
	if (variant.isString()) {
		pushString(L, variant.getString());
	} else {
		pushString(L, std::string());
	}
	return 1;
}

int LuaScriptInterface::luaVariantGetPosition(lua_State* L)
{
	// Variant:getPosition()
	const LuaVariant& variant = getVariant(L, 1);
	if (variant.isPosition()) {
		pushPosition(L, variant.getPosition());
	} else if (variant.isTargetPosition()) {
		pushPosition(L, variant.getTargetPosition());
	} else {
		pushPosition(L, Position());
	}
	return 1;
}

// Teleport
int LuaScriptInterface::luaTeleportCreate(lua_State* L)
{
	// Teleport(uid)
	uint32_t id = getNumber<uint32_t>(L, 2);

	Item* item = getScriptEnv()->getItemByUID(id);
	if (item && item->getTeleport()) {
		pushUserdata(L, item);
		setMetatable(L, -1, "Teleport");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaScriptInterface::luaTeleportGetDestination(lua_State* L)
{
	// teleport:getDestination()
	Teleport* teleport = getUserdata<Teleport>(L, 1);
	if (teleport) {
		pushPosition(L, teleport->getDestPos());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaScriptInterface::luaTeleportSetDestination(lua_State* L)
{
	// teleport:setDestination(position)
	Teleport* teleport = getUserdata<Teleport>(L, 1);
	if (teleport) {
		teleport->setDestPos(getPosition(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// Podium
int LuaScriptInterface::luaPodiumCreate(lua_State* L)
{
	// Podium(uid)
	uint32_t id = getNumber<uint32_t>(L, 2);

	Item* item = getScriptEnv()->getItemByUID(id);
	if (item && item->getPodium()) {
		pushUserdata(L, item);
		setMetatable(L, -1, "Podium");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaScriptInterface::luaPodiumGetOutfit(lua_State* L)
{
	// podium:getOutfit()
	const Podium* podium = getUserdata<const Podium>(L, 1);
	if (podium) {
		pushOutfit(L, podium->getOutfit());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaScriptInterface::luaPodiumSetOutfit(lua_State* L)
{
	// podium:setOutfit(outfit)
	Podium* podium = getUserdata<Podium>(L, 1);
	if (podium) {
		podium->setOutfit(getOutfit(L, 2));
		g_game.updatePodium(podium);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaScriptInterface::luaPodiumHasFlag(lua_State* L)
{
	// podium:hasFlag(flag)
	Podium* podium = getUserdata<Podium>(L, 1);
	if (podium) {
		PodiumFlags flag = getNumber<PodiumFlags>(L, 2);
		pushBoolean(L, podium->hasFlag(flag));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaScriptInterface::luaPodiumSetFlag(lua_State* L)
{
	// podium:setFlag(flag, value)
	uint8_t value = getBoolean(L, 3);
	PodiumFlags flag = getNumber<PodiumFlags>(L, 2);
	Podium* podium = getUserdata<Podium>(L, 1);

	if (podium) {
		podium->setFlagValue(flag, value);
		g_game.updatePodium(podium);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaScriptInterface::luaPodiumGetDirection(lua_State* L)
{
	// podium:getDirection()
	const Podium* podium = getUserdata<const Podium>(L, 1);
	if (podium) {
		lua_pushnumber(L, podium->getDirection());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaScriptInterface::luaPodiumSetDirection(lua_State* L)
{
	// podium:setDirection(direction)
	Podium* podium = getUserdata<Podium>(L, 1);
	if (podium) {
		podium->setDirection(getNumber<Direction>(L, 2));
		g_game.updatePodium(podium);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

// Outfit
int LuaScriptInterface::luaOutfitCreate(lua_State* L)
{
	// Outfit(looktype)
	const Outfit* outfit = Outfits::getInstance().getOutfitByLookType(getNumber<uint16_t>(L, 2));
	if (outfit) {
		pushOutfit(L, outfit);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaScriptInterface::luaOutfitCompare(lua_State* L)
{
	// outfit == outfitEx
	Outfit outfitEx = getOutfitClass(L, 2);
	Outfit outfit = getOutfitClass(L, 1);
	pushBoolean(L, outfit == outfitEx);
	return 1;
}

//
LuaEnvironment::LuaEnvironment() : LuaScriptInterface("Main Interface") {}

LuaEnvironment::~LuaEnvironment()
{
	delete testInterface;
	closeState();
}

bool LuaEnvironment::initState()
{
	luaState = luaL_newstate();
	if (!luaState) {
		return false;
	}

	luaL_openlibs(luaState);
	registerFunctions();

	runningEventId = EVENT_ID_USER;
	return true;
}

bool LuaEnvironment::reInitState()
{
	// TODO: get children, reload children
	closeState();
	return initState();
}

bool LuaEnvironment::closeState()
{
	if (!luaState) {
		return false;
	}

	for (const auto& combatEntry : combatIdMap) {
		clearCombatObjects(combatEntry.first);
	}

	for (const auto& areaEntry : areaIdMap) {
		clearAreaObjects(areaEntry.first);
	}

	for (auto& timerEntry : timerEvents) {
		LuaTimerEventDesc timerEventDesc = std::move(timerEntry.second);
		for (int32_t parameter : timerEventDesc.parameters) {
			luaL_unref(luaState, LUA_REGISTRYINDEX, parameter);
		}
		luaL_unref(luaState, LUA_REGISTRYINDEX, timerEventDesc.function);
	}

	combatIdMap.clear();
	areaIdMap.clear();
	timerEvents.clear();
	cacheFiles.clear();

	lua_close(luaState);
	luaState = nullptr;
	return true;
}

LuaScriptInterface* LuaEnvironment::getTestInterface()
{
	if (!testInterface) {
		testInterface = new LuaScriptInterface("Test Interface");
		testInterface->initState();
	}
	return testInterface;
}

Combat_ptr LuaEnvironment::getCombatObject(uint32_t id) const
{
	auto it = combatMap.find(id);
	if (it == combatMap.end()) {
		return nullptr;
	}
	return it->second;
}

Combat_ptr LuaEnvironment::createCombatObject(LuaScriptInterface* interface)
{
	Combat_ptr combat = std::make_shared<Combat>();
	combatMap[++lastCombatId] = combat;
	combatIdMap[interface].push_back(lastCombatId);
	return combat;
}

void LuaEnvironment::clearCombatObjects(LuaScriptInterface* interface)
{
	auto it = combatIdMap.find(interface);
	if (it == combatIdMap.end()) {
		return;
	}

	for (uint32_t id : it->second) {
		auto itt = combatMap.find(id);
		if (itt != combatMap.end()) {
			combatMap.erase(itt);
		}
	}
	it->second.clear();
}

AreaCombat* LuaEnvironment::getAreaObject(uint32_t id) const
{
	auto it = areaMap.find(id);
	if (it == areaMap.end()) {
		return nullptr;
	}
	return it->second;
}

uint32_t LuaEnvironment::createAreaObject(LuaScriptInterface* interface)
{
	areaMap[++lastAreaId] = new AreaCombat;
	areaIdMap[interface].push_back(lastAreaId);
	return lastAreaId;
}

void LuaEnvironment::clearAreaObjects(LuaScriptInterface* interface)
{
	auto it = areaIdMap.find(interface);
	if (it == areaIdMap.end()) {
		return;
	}

	for (uint32_t id : it->second) {
		auto itt = areaMap.find(id);
		if (itt != areaMap.end()) {
			delete itt->second;
			areaMap.erase(itt);
		}
	}
	it->second.clear();
}

void LuaEnvironment::executeTimerEvent(uint32_t eventIndex)
{
	auto it = timerEvents.find(eventIndex);
	if (it == timerEvents.end()) {
		return;
	}

	LuaTimerEventDesc timerEventDesc = std::move(it->second);
	timerEvents.erase(it);

	// push function
	lua_rawgeti(luaState, LUA_REGISTRYINDEX, timerEventDesc.function);

	// push parameters
	for (auto parameter : boost::adaptors::reverse(timerEventDesc.parameters)) {
		lua_rawgeti(luaState, LUA_REGISTRYINDEX, parameter);
	}

	// call the function
	if (reserveScriptEnv()) {
		ScriptEnvironment* env = getScriptEnv();
		env->setTimerEvent();
		env->setScriptId(timerEventDesc.scriptId, this);
		callFunction(timerEventDesc.parameters.size());
	} else {
		std::cout << "[Error - LuaScriptInterface::executeTimerEvent] Call stack overflow" << std::endl;
	}

	// free resources
	luaL_unref(luaState, LUA_REGISTRYINDEX, timerEventDesc.function);
	for (auto parameter : timerEventDesc.parameters) {
		luaL_unref(luaState, LUA_REGISTRYINDEX, parameter);
	}
}

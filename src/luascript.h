// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LUASCRIPT_H
#define FS_LUASCRIPT_H

#include "database.h"
#include "enums.h"
#include "position.h"

#if LUA_VERSION_NUM >= 502
#ifndef LUA_COMPAT_ALL
#ifndef LUA_COMPAT_MODULE
#define luaL_register(L, libname, l) (luaL_newlib(L, l), lua_pushvalue(L, -1), lua_setglobal(L, libname))
#endif
#undef lua_equal
#define lua_equal(L, i1, i2) lua_compare(L, (i1), (i2), LUA_OPEQ)
#endif
#endif

class AreaCombat;
class Combat;
class Container;
class Creature;
class Cylinder;
class InstantSpell;
class Item;
class LuaScriptInterface;
class LuaVariant;
class Npc;
class Player;
class Thing;
struct LootBlock;
struct Mount;
struct Outfit;

using Combat_ptr = std::shared_ptr<Combat>;

enum
{
	EVENT_ID_LOADING = 1,
	EVENT_ID_USER = 1000,
};

enum LuaDataType
{
	LuaData_Unknown,

	LuaData_Item,
	LuaData_Container,
	LuaData_Teleport,
	LuaData_Podium,
	LuaData_Player,
	LuaData_Monster,
	LuaData_Npc,
	LuaData_Tile,
};

struct LuaTimerEventDesc
{
	int32_t scriptId = -1;
	int32_t function = -1;
	std::vector<int32_t> parameters;
	uint32_t eventId = 0;

	LuaTimerEventDesc() = default;
	LuaTimerEventDesc(LuaTimerEventDesc&& other) = default;
};

class LuaScriptInterface;
class Cylinder;
class Game;

struct LootBlock;
struct FieldBlock;

class ScriptEnvironment
{
public:
	ScriptEnvironment();
	~ScriptEnvironment();

	// non-copyable
	ScriptEnvironment(const ScriptEnvironment&) = delete;
	ScriptEnvironment& operator=(const ScriptEnvironment&) = delete;

	void resetEnv();

	void setScriptId(int32_t scriptId, LuaScriptInterface* scriptInterface)
	{
		this->scriptId = scriptId;
		interface = scriptInterface;
	}
	bool setCallbackId(int32_t callbackId, LuaScriptInterface* scriptInterface);

	int32_t getScriptId() const { return scriptId; }
	LuaScriptInterface* getScriptInterface() { return interface; }

	void setTimerEvent() { timerEvent = true; }

	void getEventInfo(int32_t& scriptId, LuaScriptInterface*& scriptInterface, int32_t& callbackId,
	                  bool& timerEvent) const;

	void addTempItem(Item* item);
	static void removeTempItem(Item* item);
	uint32_t addThing(Thing* thing);
	void insertItem(uint32_t uid, Item* item);

	static DBResult_ptr getResultByID(uint32_t id);
	static uint32_t addResult(DBResult_ptr res);
	static bool removeResult(uint32_t id);

	void setNpc(Npc* npc) { curNpc = npc; }
	Npc* getNpc() const { return curNpc; }

	Thing* getThingByUID(uint32_t uid);
	Item* getItemByUID(uint32_t uid);
	Container* getContainerByUID(uint32_t uid);
	void removeItemByUID(uint32_t uid);

private:
	using VariantVector = std::vector<const LuaVariant*>;
	using StorageMap = std::map<uint32_t, int32_t>;
	using DBResultMap = std::map<uint32_t, DBResult_ptr>;

	LuaScriptInterface* interface;

	// for npc scripts
	Npc* curNpc = nullptr;

	// temporary item list
	static std::multimap<ScriptEnvironment*, Item*> tempItems;

	// local item map
	std::unordered_map<uint32_t, Item*> localMap;
	uint32_t lastUID = std::numeric_limits<uint16_t>::max();

	// script file id
	int32_t scriptId;
	int32_t callbackId;
	bool timerEvent;

	// result map
	static uint32_t lastResultId;
	static DBResultMap tempResults;
};

enum ErrorCode_t
{
	LUA_ERROR_PLAYER_NOT_FOUND,
	LUA_ERROR_CREATURE_NOT_FOUND,
	LUA_ERROR_ITEM_NOT_FOUND,
	LUA_ERROR_THING_NOT_FOUND,
	LUA_ERROR_TILE_NOT_FOUND,
	LUA_ERROR_HOUSE_NOT_FOUND,
	LUA_ERROR_COMBAT_NOT_FOUND,
	LUA_ERROR_CONDITION_NOT_FOUND,
	LUA_ERROR_AREA_NOT_FOUND,
	LUA_ERROR_CONTAINER_NOT_FOUND,
	LUA_ERROR_VARIANT_NOT_FOUND,
	LUA_ERROR_VARIANT_UNKNOWN,
	LUA_ERROR_SPELL_NOT_FOUND,
};

class LuaScriptInterface
{
public:
	explicit LuaScriptInterface(std::string interfaceName);
	virtual ~LuaScriptInterface();

	// non-copyable
	LuaScriptInterface(const LuaScriptInterface&) = delete;
	LuaScriptInterface& operator=(const LuaScriptInterface&) = delete;

	virtual bool initState(bool newLuaState = false);
	bool reInitState();

	int32_t loadFile(const std::string& file, Npc* npc = nullptr);

	const std::string& getFileById(int32_t scriptId);
	int32_t getEvent(const std::string& eventName);
	int32_t getEvent();
	int32_t getMetaEvent(const std::string& globalName, const std::string& eventName);

	static ScriptEnvironment* getScriptEnv()
	{
		//assert(scriptEnvIndex >= 0 && scriptEnvIndex < 16);
		return scriptEnv + scriptEnvIndex;
	}

	static bool reserveScriptEnv() { return ++scriptEnvIndex; }

	static void resetScriptEnv()
	{
		//assert(scriptEnvIndex >= 0);
		scriptEnv[scriptEnvIndex--].resetEnv();
	}

	static void reportError(const char* function, const std::string& error_desc, lua_State* L = nullptr,
	                        bool stack_trace = false);

	const std::string& getInterfaceName() const { return interfaceName; }
	const std::string& getLastLuaError() const { return lastLuaError; }

	lua_State* getLuaState() const { return luaState; }

	bool pushFunction(int32_t functionId);

	static int luaErrorHandler(lua_State* L);
	bool callFunction(int params);
	void callVoidFunction(int params);

	// push/pop common structures
	static void pushThing(lua_State* L, Thing* thing);
	static void pushVariant(lua_State* L, const LuaVariant& var);
	static void pushCallback(lua_State* L, int32_t callback);
	static void pushCylinder(lua_State* L, Cylinder* cylinder);

	static std::string popString(lua_State* L);
	static int32_t popCallback(lua_State* L);

	static std::string escapeString(std::string string);

#ifndef LUAJIT_VERSION
	static const luaL_Reg luaBitReg[7];
#endif
	static const luaL_Reg luaConfigManagerTable[4];
	static const luaL_Reg luaDatabaseTable[9];
	static const luaL_Reg luaResultTable[6];

	static int protectedCall(lua_State* L, int nargs, int nresults);

	void registerClass(const std::string& className, const std::string& baseClass, lua_CFunction newFunction = nullptr);
	void registerTable(const std::string& tableName);
	void registerMetaMethod(const std::string& className, const std::string& methodName, lua_CFunction func);
	void registerGlobalMethod(const std::string& functionName, lua_CFunction func);
	void registerVariable(const std::string& tableName, const std::string& name, lua_Number value);
	void registerGlobalVariable(const std::string& name, lua_Number value);
	void registerGlobalBoolean(const std::string& name, bool value);
	void registerMethod(const std::string& globalName, const std::string& methodName, lua_CFunction func);

	// Userdata
	static int luaUserdataCompare(lua_State* L);

	static std::string getErrorDesc(ErrorCode_t code);

	virtual bool closeState();

	void registerFunctions();

	lua_State* luaState = nullptr;

	int32_t eventTableRef = -1;
	int32_t runningEventId = EVENT_ID_USER;

	// script file cache
	std::map<int32_t, std::string> cacheFiles;

private:
	static bool getArea(lua_State* L, std::vector<uint32_t>& vec, uint32_t& rows);

	// lua functions
	static int luaDoPlayerAddItem(lua_State* L);

	// get item info
	static int luaGetDepotId(lua_State* L);

	// get world info
	static int luaGetWorldTime(lua_State* L);
	static int luaGetWorldUpTime(lua_State* L);
	static int luaGetWorldLight(lua_State* L);
	static int luaSetWorldLight(lua_State* L);

	// get subtype name
	static int luaGetSubTypeName(lua_State* L);

	// type validation
	static int luaIsDepot(lua_State* L);
	static int luaIsMoveable(lua_State* L);
	static int luaIsValidUID(lua_State* L);

	// container
	static int luaDoAddContainerItem(lua_State* L);

	//
	static int luaCreateCombatArea(lua_State* L);

	static int luaDoAreaCombat(lua_State* L);
	static int luaDoTargetCombat(lua_State* L);

	static int luaDoChallengeCreature(lua_State* L);

	static int luaDebugPrint(lua_State* L);
	static int luaAddEvent(lua_State* L);
	static int luaStopEvent(lua_State* L);

	static int luaSaveServer(lua_State* L);
	static int luaCleanMap(lua_State* L);

	static int luaIsInWar(lua_State* L);

	static int luaGetWaypointPositionByName(lua_State* L);

	static int luaSendChannelMessage(lua_State* L);
	static int luaSendGuildChannelMessage(lua_State* L);

	static int luaIsScriptsInterface(lua_State* L);

#ifndef LUAJIT_VERSION
	static int luaBitNot(lua_State* L);
	static int luaBitAnd(lua_State* L);
	static int luaBitOr(lua_State* L);
	static int luaBitXor(lua_State* L);
	static int luaBitLeftShift(lua_State* L);
	static int luaBitRightShift(lua_State* L);
#endif

	static int luaConfigManagerGetString(lua_State* L);
	static int luaConfigManagerGetNumber(lua_State* L);
	static int luaConfigManagerGetBoolean(lua_State* L);

	static int luaDatabaseExecute(lua_State* L);
	static int luaDatabaseAsyncExecute(lua_State* L);
	static int luaDatabaseStoreQuery(lua_State* L);
	static int luaDatabaseAsyncStoreQuery(lua_State* L);
	static int luaDatabaseEscapeString(lua_State* L);
	static int luaDatabaseEscapeBlob(lua_State* L);
	static int luaDatabaseLastInsertId(lua_State* L);
	static int luaDatabaseTableExists(lua_State* L);

	static int luaResultGetNumber(lua_State* L);
	static int luaResultGetString(lua_State* L);
	static int luaResultGetStream(lua_State* L);
	static int luaResultNext(lua_State* L);
	static int luaResultFree(lua_State* L);

	// _G
	static int luaIsType(lua_State* L);
	static int luaRawGetMetatable(lua_State* L);

	// os
	static int luaSystemTime(lua_State* L);

	// table
	static int luaTableCreate(lua_State* L);
	static int luaTablePack(lua_State* L);

	// Variant
	static int luaVariantCreate(lua_State* L);

	static int luaVariantGetNumber(lua_State* L);
	static int luaVariantGetString(lua_State* L);
	static int luaVariantGetPosition(lua_State* L);

	// Teleport
	static int luaTeleportCreate(lua_State* L);

	static int luaTeleportGetDestination(lua_State* L);
	static int luaTeleportSetDestination(lua_State* L);

	// Podium
	static int luaPodiumCreate(lua_State* L);

	static int luaPodiumGetOutfit(lua_State* L);
	static int luaPodiumSetOutfit(lua_State* L);
	static int luaPodiumHasFlag(lua_State* L);
	static int luaPodiumSetFlag(lua_State* L);
	static int luaPodiumGetDirection(lua_State* L);
	static int luaPodiumSetDirection(lua_State* L);

	// Outfit
	static int luaOutfitCreate(lua_State* L);
	static int luaOutfitCompare(lua_State* L);

	//
	std::string lastLuaError;

	std::string interfaceName;

	static ScriptEnvironment scriptEnv[128];
	static int32_t scriptEnvIndex;

	std::string loadingFile;
};

class LuaEnvironment : public LuaScriptInterface
{
public:
	LuaEnvironment();
	~LuaEnvironment();

	// non-copyable
	LuaEnvironment(const LuaEnvironment&) = delete;
	LuaEnvironment& operator=(const LuaEnvironment&) = delete;

	bool initState(bool newLuaState = false) override;
	bool reInitState();
	bool closeState() override;

	LuaScriptInterface* getTestInterface();

	Combat_ptr getCombatObject(uint32_t id) const;
	Combat_ptr createCombatObject(LuaScriptInterface* interface);
	void clearCombatObjects(LuaScriptInterface* interface);

	AreaCombat* getAreaObject(uint32_t id) const;
	uint32_t createAreaObject(LuaScriptInterface* interface);
	void clearAreaObjects(LuaScriptInterface* interface);

private:
	void executeTimerEvent(uint32_t eventIndex);

	std::unordered_map<uint32_t, LuaTimerEventDesc> timerEvents;
	std::unordered_map<uint32_t, Combat_ptr> combatMap;
	std::unordered_map<uint32_t, AreaCombat*> areaMap;

	std::unordered_map<LuaScriptInterface*, std::vector<uint32_t>> combatIdMap;
	std::unordered_map<LuaScriptInterface*, std::vector<uint32_t>> areaIdMap;

	LuaScriptInterface* testInterface = nullptr;

	uint32_t lastEventTimerId = 1;
	uint32_t lastCombatId = 0;
	uint32_t lastAreaId = 0;

	friend class LuaScriptInterface;
	friend class CombatSpell;
};

namespace Lua {
#define reportErrorFunc(L, a) LuaScriptInterface::reportError(__FUNCTION__, a, L, true)

static bool isNumber(lua_State* L, int32_t arg) { return lua_type(L, arg) == LUA_TNUMBER; };
static bool isString(lua_State* L, int32_t arg) { return lua_isstring(L, arg) != 0; };
static bool isBoolean(lua_State* L, int32_t arg) { return lua_isboolean(L, arg); };
static bool isTable(lua_State* L, int32_t arg) { return lua_istable(L, arg); };
static bool isFunction(lua_State* L, int32_t arg) { return lua_isfunction(L, arg); };
static bool isUserdata(lua_State* L, int32_t arg) { return lua_isuserdata(L, arg) != 0; };
std::string getStackTrace(lua_State* L, const std::string& error_desc);

// Metatables
void setMetatable(lua_State* L, int32_t index, const std::string& name);
void setWeakMetatable(lua_State* L, int32_t index, const std::string& name);

void setItemMetatable(lua_State* L, int32_t index, const Item* item);
void setCreatureMetatable(lua_State* L, int32_t index, const Creature* creature);

// Get
template <typename T>
static typename std::enable_if<std::is_enum<T>::value, T>::type getNumber(lua_State* L, int32_t arg)
{
	return static_cast<T>(static_cast<int64_t>(lua_tonumber(L, arg)));
}

template <typename T>
static typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, T>::type getNumber(
    lua_State* L, int32_t arg)
{
	double num = lua_tonumber(L, arg);
	if (num < static_cast<double>(std::numeric_limits<T>::lowest()) ||
	    num > static_cast<double>(std::numeric_limits<T>::max())) {
		reportErrorFunc(L, fmt::format("Argument {} has out-of-range value for {}: {}", arg, typeid(T).name(), num));
	}

	return static_cast<T>(num);
}

template <typename T>
static typename std::enable_if<
    (std::is_integral<T>::value && std::is_signed<T>::value) || std::is_floating_point<T>::value, T>::type
getNumber(lua_State* L, int32_t arg)
{
	double num = lua_tonumber(L, arg);
	if (num < static_cast<double>(std::numeric_limits<T>::lowest()) ||
	    num > static_cast<double>(std::numeric_limits<T>::max())) {
		reportErrorFunc(L, fmt::format("Argument {} has out-of-range value for {}: {}", arg, typeid(T).name(), num));
	}

	return static_cast<T>(num);
}

template <typename T>
static T getNumber(lua_State* L, int32_t arg, T defaultValue)
{
	const auto parameters = lua_gettop(L);
	if (parameters == 0 || arg > parameters) {
		return defaultValue;
	}
	return getNumber<T>(L, arg);
}
template <class T>
static T** getRawUserdata(lua_State* L, int32_t arg)
{
	return static_cast<T**>(lua_touserdata(L, arg));
}
template <class T>
static T* getUserdata(lua_State* L, int32_t arg)
{
	T** userdata = getRawUserdata<T>(L, arg);
	if (!userdata) {
		return nullptr;
	}
	return *userdata;
}
template <class T>
static std::shared_ptr<T>& getSharedPtr(lua_State* L, int32_t arg)
{
	return *static_cast<std::shared_ptr<T>*>(lua_touserdata(L, arg));
}

static bool getBoolean(lua_State* L, int32_t arg) { return lua_toboolean(L, arg) != 0; }
static bool getBoolean(lua_State* L, int32_t arg, bool defaultValue)
{
	const auto parameters = lua_gettop(L);
	if (parameters == 0 || arg > parameters) {
		return defaultValue;
	}
	return lua_toboolean(L, arg) != 0;
}

std::string getString(lua_State* L, int32_t arg);
Position getPosition(lua_State* L, int32_t arg, int32_t& stackpos);
Position getPosition(lua_State* L, int32_t arg);
Outfit_t getOutfit(lua_State* L, int32_t arg);
Outfit getOutfitClass(lua_State* L, int32_t arg);
LuaVariant getVariant(lua_State* L, int32_t arg);
InstantSpell* getInstantSpell(lua_State* L, int32_t arg);
Reflect getReflect(lua_State* L, int32_t arg);
FieldBlock getFieldBlock(lua_State* L, int32_t arg);

Thing* getThing(lua_State* L, int32_t arg);
Creature* getCreature(lua_State* L, int32_t arg);
Player* getPlayer(lua_State* L, int32_t arg);

template <typename T>
static T getField(lua_State* L, int32_t arg, const std::string& key)
{
	lua_getfield(L, arg, key.c_str());
	return getNumber<T>(L, -1);
}

std::string getFieldString(lua_State* L, int32_t arg, const std::string& key);

LuaDataType getUserdataType(lua_State* L, int32_t arg);

// Push
void pushBoolean(lua_State* L, bool value);
void pushCombatDamage(lua_State* L, const CombatDamage& damage);
void pushInstantSpell(lua_State* L, const InstantSpell& spell);
void pushPosition(lua_State* L, const Position& position, int32_t stackpos = 0);
void pushOutfit(lua_State* L, const Outfit_t& outfit);
void pushOutfit(lua_State* L, const Outfit* outfit);
void pushMount(lua_State* L, const Mount* mount);
void pushLoot(lua_State* L, const std::vector<LootBlock>& lootList);
void pushReflect(lua_State* L, const Reflect& reflect);
void pushFieldBlock(lua_State* L, const FieldBlock& fieldBlock);
void pushString(lua_State* L, const std::string& value);

//
static void setField(lua_State* L, const char* index, lua_Number value)
{
	lua_pushnumber(L, value);
	lua_setfield(L, -2, index);
}

static void setField(lua_State* L, const char* index, const std::string& value)
{
	pushString(L, value);
	lua_setfield(L, -2, index);
}

// Userdata
template <class T>
static void pushUserdata(lua_State* L, T* value)
{
	T** userdata = static_cast<T**>(lua_newuserdata(L, sizeof(T*)));
	*userdata = value;
}

// Shared Ptr
template <class T>
static void pushSharedPtr(lua_State* L, T value)
{
	new (lua_newuserdata(L, sizeof(T))) T(std::move(value));
}

}; // namespace Lua

#endif // FS_LUASCRIPT_H

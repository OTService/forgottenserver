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

	virtual bool initState();
	bool reInitState();

	int32_t loadFile(const std::string& file, Npc* npc = nullptr);

	const std::string& getFileById(int32_t scriptId);
	int32_t getEvent(const std::string& eventName);
	int32_t getEvent();
	int32_t getMetaEvent(const std::string& globalName, const std::string& eventName);

	static ScriptEnvironment* getScriptEnv()
	{
		assert(scriptEnvIndex >= 0 && scriptEnvIndex < 16);
		return scriptEnv + scriptEnvIndex;
	}

	static bool reserveScriptEnv() { return ++scriptEnvIndex < 16; }

	static void resetScriptEnv()
	{
		assert(scriptEnvIndex >= 0);
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

protected:
	virtual bool closeState();

	void registerFunctions();
	void registerEnums();
	void registerGameFunctions();
	void registerPositionFunctions();
	void registerTileFunctions();
	void registerNetworkMessageFunctions();
	void registerModalWindowFunctions();
	void registerItemFunctions();
	void registerCreatureFunctions();
	void registerPlayerFunctions();
	void registerMonsterFunctions();

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

	// Npc
	static int luaNpcCreate(lua_State* L);

	static int luaNpcIsNpc(lua_State* L);

	static int luaNpcSetMasterPos(lua_State* L);

	static int luaNpcGetSpeechBubble(lua_State* L);
	static int luaNpcSetSpeechBubble(lua_State* L);

	// Guild
	static int luaGuildCreate(lua_State* L);

	static int luaGuildGetId(lua_State* L);
	static int luaGuildGetName(lua_State* L);
	static int luaGuildGetMembersOnline(lua_State* L);

	static int luaGuildAddRank(lua_State* L);
	static int luaGuildGetRankById(lua_State* L);
	static int luaGuildGetRankByLevel(lua_State* L);

	static int luaGuildGetMotd(lua_State* L);
	static int luaGuildSetMotd(lua_State* L);

	// Group
	static int luaGroupCreate(lua_State* L);

	static int luaGroupGetId(lua_State* L);
	static int luaGroupGetName(lua_State* L);
	static int luaGroupGetFlags(lua_State* L);
	static int luaGroupGetAccess(lua_State* L);
	static int luaGroupGetMaxDepotItems(lua_State* L);
	static int luaGroupGetMaxVipEntries(lua_State* L);
	static int luaGroupHasFlag(lua_State* L);

	// Vocation
	static int luaVocationCreate(lua_State* L);

	static int luaVocationGetId(lua_State* L);
	static int luaVocationGetClientId(lua_State* L);
	static int luaVocationGetName(lua_State* L);
	static int luaVocationGetDescription(lua_State* L);

	static int luaVocationGetRequiredSkillTries(lua_State* L);
	static int luaVocationGetRequiredManaSpent(lua_State* L);

	static int luaVocationGetCapacityGain(lua_State* L);

	static int luaVocationGetHealthGain(lua_State* L);
	static int luaVocationGetHealthGainTicks(lua_State* L);
	static int luaVocationGetHealthGainAmount(lua_State* L);

	static int luaVocationGetManaGain(lua_State* L);
	static int luaVocationGetManaGainTicks(lua_State* L);
	static int luaVocationGetManaGainAmount(lua_State* L);

	static int luaVocationGetMaxSoul(lua_State* L);
	static int luaVocationGetSoulGainTicks(lua_State* L);

	static int luaVocationGetAttackSpeed(lua_State* L);
	static int luaVocationGetBaseSpeed(lua_State* L);

	static int luaVocationGetDemotion(lua_State* L);
	static int luaVocationGetPromotion(lua_State* L);

	static int luaVocationAllowsPvp(lua_State* L);

	// Town
	static int luaTownCreate(lua_State* L);

	static int luaTownGetId(lua_State* L);
	static int luaTownGetName(lua_State* L);
	static int luaTownGetTemplePosition(lua_State* L);

	// House
	static int luaHouseCreate(lua_State* L);

	static int luaHouseGetId(lua_State* L);
	static int luaHouseGetName(lua_State* L);
	static int luaHouseGetTown(lua_State* L);
	static int luaHouseGetExitPosition(lua_State* L);

	static int luaHouseGetRent(lua_State* L);
	static int luaHouseSetRent(lua_State* L);

	static int luaHouseGetPaidUntil(lua_State* L);
	static int luaHouseSetPaidUntil(lua_State* L);

	static int luaHouseGetPayRentWarnings(lua_State* L);
	static int luaHouseSetPayRentWarnings(lua_State* L);

	static int luaHouseGetOwnerName(lua_State* L);
	static int luaHouseGetOwnerGuid(lua_State* L);
	static int luaHouseSetOwnerGuid(lua_State* L);
	static int luaHouseStartTrade(lua_State* L);

	static int luaHouseGetBeds(lua_State* L);
	static int luaHouseGetBedCount(lua_State* L);

	static int luaHouseGetDoors(lua_State* L);
	static int luaHouseGetDoorCount(lua_State* L);
	static int luaHouseGetDoorIdByPosition(lua_State* L);

	static int luaHouseGetTiles(lua_State* L);
	static int luaHouseGetItems(lua_State* L);
	static int luaHouseGetTileCount(lua_State* L);

	static int luaHouseCanEditAccessList(lua_State* L);
	static int luaHouseGetAccessList(lua_State* L);
	static int luaHouseSetAccessList(lua_State* L);

	static int luaHouseKickPlayer(lua_State* L);

	static int luaHouseSave(lua_State* L);

	// ItemType
	static int luaItemTypeCreate(lua_State* L);

	static int luaItemTypeCorpse(lua_State* L);
	static int luaItemTypeDoor(lua_State* L);
	static int luaItemTypeContainer(lua_State* L);
	static int luaItemTypeFluidContainer(lua_State* L);
	static int luaItemTypeMovable(lua_State* L);
	static int luaItemTypeRune(lua_State* L);
	static int luaItemTypeStackable(lua_State* L);
	static int luaItemTypeReadable(lua_State* L);
	static int luaItemTypeWritable(lua_State* L);
	static int luaItemTypeEffect(lua_State* L);
	static int luaItemTypeContainerSize(lua_State* L);
	static int luaItemTypeRotateTo(lua_State* L);
	static int luaItemTypePartnerDirection(lua_State* L);
	static int luaItemTypeFemaleSleeper(lua_State* L);
	static int luaItemTypeMaleSleeper(lua_State* L);
	static int luaItemTypeMaxTextLen(lua_State* L);
	static int luaItemTypeWriteOnceItemId(lua_State* L);
	static int luaItemTypeRuneSpellName(lua_State* L);
	static int luaItemTypeWorth(lua_State* L);
	static int luaItemTypeWalkStack(lua_State* L);
	static int luaItemTypeField(lua_State* L);
	static int luaItemTypeBlocking(lua_State* L);
	static int luaItemTypeBlockProjectile(lua_State* L);
	static int luaItemTypeRotatable(lua_State* L);
	static int luaItemTypeGroundTile(lua_State* L);
	static int luaItemTypeMagicField(lua_State* L);
	static int luaItemTypeUseable(lua_State* L);
	static int luaItemTypePickupable(lua_State* L);
	static int luaItemTypeAllowPickupable(lua_State* L);

	static int luaItemTypeType(lua_State* L);
	static int luaItemTypeGroup(lua_State* L);
	static int luaItemTypeId(lua_State* L);
	static int luaItemTypeClientId(lua_State* L);
	static int luaItemTypeName(lua_State* L);
	static int luaItemTypePluralName(lua_State* L);
	static int luaItemTypeArticle(lua_State* L);
	static int luaItemTypeDescription(lua_State* L);
	static int luaItemTypeSlotPosition(lua_State* L);

	static int luaItemTypeCharges(lua_State* L);
	static int luaItemTypeFluidSource(lua_State* L);
	static int luaItemTypeCapacity(lua_State* L);
	static int luaItemTypeWeight(lua_State* L);

	static int luaItemTypeHitChance(lua_State* L);
	static int luaItemTypeMaxHitChance(lua_State* L);
	static int luaItemTypeShootRange(lua_State* L);
	static int luaItemTypeShootType(lua_State* L);
	static int luaItemTypeAttack(lua_State* L);
	static int luaItemTypeAttackSpeed(lua_State* L);
	static int luaItemTypeDefense(lua_State* L);
	static int luaItemTypeExtraDefense(lua_State* L);
	static int luaItemTypeArmor(lua_State* L);
	static int luaItemTypeWeaponType(lua_State* L);

	static int luaItemTypeElementType(lua_State* L);
	static int luaItemTypeElementDamage(lua_State* L);

	static int luaItemTypeTransformEquipId(lua_State* L);
	static int luaItemTypeTransformDeEquipId(lua_State* L);
	static int luaItemTypeDestroyId(lua_State* L);
	static int luaItemTypeDecayId(lua_State* L);
	static int luaItemTypeRequiredLevel(lua_State* L);
	static int luaItemTypeAmmoType(lua_State* L);
	static int luaItemTypeCorpseType(lua_State* L);
	static int luaItemTypeClassification(lua_State* L);
	static int luaItemTypeShowCount(lua_State* L);
	static int luaItemTypeAbilities(lua_State* L);
	static int luaItemTypeShowAttributes(lua_State* L);
	static int luaItemTypeShowCharges(lua_State* L);
	static int luaItemTypeStopDuration(lua_State* L);
	static int luaItemTypeShowDuration(lua_State* L);
	static int luaItemTypeAllowDistRead(lua_State* L);
	static int luaItemTypeWieldInfo(lua_State* L);
	static int luaItemTypeReplaceable(lua_State* L);
	static int luaItemTypeDuration(lua_State* L);
	static int luaItemTypeFloorChange(lua_State* L);
	static int luaItemTypeLevelDoor(lua_State* L);
	static int luaItemTypeVocationString(lua_State* L);
	static int luaItemTypeMinReqLevel(lua_State* L);
	static int luaItemTypeMinReqMagicLevel(lua_State* L);
	static int luaItemTypeGetMarketBuyStatistics(lua_State* L);
	static int luaItemTypeGetMarketSellStatistics(lua_State* L);

	static int luaItemTypeSubType(lua_State* L);

	static int luaItemTypeStoreItem(lua_State* L);
	static int luaItemTypeRegister(lua_State* L);

	// Combat
	static int luaCombatCreate(lua_State* L);
	static int luaCombatDelete(lua_State* L);

	static int luaCombatSetParameter(lua_State* L);
	static int luaCombatGetParameter(lua_State* L);

	static int luaCombatSetFormula(lua_State* L);

	static int luaCombatSetArea(lua_State* L);
	static int luaCombatAddCondition(lua_State* L);
	static int luaCombatClearConditions(lua_State* L);
	static int luaCombatSetCallback(lua_State* L);
	static int luaCombatSetOrigin(lua_State* L);

	static int luaCombatExecute(lua_State* L);

	// Condition
	static int luaConditionCreate(lua_State* L);
	static int luaConditionDelete(lua_State* L);

	static int luaConditionGetId(lua_State* L);
	static int luaConditionGetSubId(lua_State* L);
	static int luaConditionGetType(lua_State* L);
	static int luaConditionGetIcons(lua_State* L);
	static int luaConditionGetEndTime(lua_State* L);

	static int luaConditionClone(lua_State* L);

	static int luaConditionGetTicks(lua_State* L);
	static int luaConditionSetTicks(lua_State* L);

	static int luaConditionSetParameter(lua_State* L);
	static int luaConditionGetParameter(lua_State* L);

	static int luaConditionSetFormula(lua_State* L);
	static int luaConditionSetOutfit(lua_State* L);

	static int luaConditionAddDamage(lua_State* L);

	// Outfit
	static int luaOutfitCreate(lua_State* L);
	static int luaOutfitCompare(lua_State* L);

	// MonsterType
	static int luaMonsterTypeCreate(lua_State* L);

	static int luaMonsterTypeIsAttackable(lua_State* L);
	static int luaMonsterTypeIsChallengeable(lua_State* L);
	static int luaMonsterTypeIsConvinceable(lua_State* L);
	static int luaMonsterTypeIsSummonable(lua_State* L);
	static int luaMonsterTypeIsIgnoringSpawnBlock(lua_State* L);
	static int luaMonsterTypeIsIllusionable(lua_State* L);
	static int luaMonsterTypeIsHostile(lua_State* L);
	static int luaMonsterTypeIsPushable(lua_State* L);
	static int luaMonsterTypeIsHealthHidden(lua_State* L);
	static int luaMonsterTypeIsBoss(lua_State* L);

	static int luaMonsterTypeCanPushItems(lua_State* L);
	static int luaMonsterTypeCanPushCreatures(lua_State* L);

	static int luaMonsterTypeCanWalkOnEnergy(lua_State* L);
	static int luaMonsterTypeCanWalkOnFire(lua_State* L);
	static int luaMonsterTypeCanWalkOnPoison(lua_State* L);

	static int luaMonsterTypeName(lua_State* L);
	static int luaMonsterTypeNameDescription(lua_State* L);

	static int luaMonsterTypeHealth(lua_State* L);
	static int luaMonsterTypeMaxHealth(lua_State* L);
	static int luaMonsterTypeRunHealth(lua_State* L);
	static int luaMonsterTypeExperience(lua_State* L);
	static int luaMonsterTypeSkull(lua_State* L);

	static int luaMonsterTypeCombatImmunities(lua_State* L);
	static int luaMonsterTypeConditionImmunities(lua_State* L);

	static int luaMonsterTypeGetAttackList(lua_State* L);
	static int luaMonsterTypeAddAttack(lua_State* L);

	static int luaMonsterTypeGetDefenseList(lua_State* L);
	static int luaMonsterTypeAddDefense(lua_State* L);

	static int luaMonsterTypeGetElementList(lua_State* L);
	static int luaMonsterTypeAddElement(lua_State* L);

	static int luaMonsterTypeGetVoices(lua_State* L);
	static int luaMonsterTypeAddVoice(lua_State* L);

	static int luaMonsterTypeGetLoot(lua_State* L);
	static int luaMonsterTypeAddLoot(lua_State* L);

	static int luaMonsterTypeGetCreatureEvents(lua_State* L);
	static int luaMonsterTypeRegisterEvent(lua_State* L);

	static int luaMonsterTypeEventOnCallback(lua_State* L);
	static int luaMonsterTypeEventType(lua_State* L);

	static int luaMonsterTypeGetSummonList(lua_State* L);
	static int luaMonsterTypeAddSummon(lua_State* L);

	static int luaMonsterTypeMaxSummons(lua_State* L);

	static int luaMonsterTypeArmor(lua_State* L);
	static int luaMonsterTypeDefense(lua_State* L);
	static int luaMonsterTypeOutfit(lua_State* L);
	static int luaMonsterTypeRace(lua_State* L);
	static int luaMonsterTypeCorpseId(lua_State* L);
	static int luaMonsterTypeManaCost(lua_State* L);
	static int luaMonsterTypeBaseSpeed(lua_State* L);
	static int luaMonsterTypeLight(lua_State* L);

	static int luaMonsterTypeStaticAttackChance(lua_State* L);
	static int luaMonsterTypeTargetDistance(lua_State* L);
	static int luaMonsterTypeYellChance(lua_State* L);
	static int luaMonsterTypeYellSpeedTicks(lua_State* L);
	static int luaMonsterTypeChangeTargetChance(lua_State* L);
	static int luaMonsterTypeChangeTargetSpeed(lua_State* L);

	// Loot
	static int luaCreateLoot(lua_State* L);
	static int luaDeleteLoot(lua_State* L);
	static int luaLootSetId(lua_State* L);
	static int luaLootSetMaxCount(lua_State* L);
	static int luaLootSetSubType(lua_State* L);
	static int luaLootSetChance(lua_State* L);
	static int luaLootSetActionId(lua_State* L);
	static int luaLootSetDescription(lua_State* L);
	static int luaLootAddChildLoot(lua_State* L);

	// MonsterSpell
	static int luaCreateMonsterSpell(lua_State* L);
	static int luaDeleteMonsterSpell(lua_State* L);
	static int luaMonsterSpellSetType(lua_State* L);
	static int luaMonsterSpellSetScriptName(lua_State* L);
	static int luaMonsterSpellSetChance(lua_State* L);
	static int luaMonsterSpellSetInterval(lua_State* L);
	static int luaMonsterSpellSetRange(lua_State* L);
	static int luaMonsterSpellSetCombatValue(lua_State* L);
	static int luaMonsterSpellSetCombatType(lua_State* L);
	static int luaMonsterSpellSetAttackValue(lua_State* L);
	static int luaMonsterSpellSetNeedTarget(lua_State* L);
	static int luaMonsterSpellSetNeedDirection(lua_State* L);
	static int luaMonsterSpellSetCombatLength(lua_State* L);
	static int luaMonsterSpellSetCombatSpread(lua_State* L);
	static int luaMonsterSpellSetCombatRadius(lua_State* L);
	static int luaMonsterSpellSetCombatRing(lua_State* L);
	static int luaMonsterSpellSetConditionType(lua_State* L);
	static int luaMonsterSpellSetConditionDamage(lua_State* L);
	static int luaMonsterSpellSetConditionSpeedChange(lua_State* L);
	static int luaMonsterSpellSetConditionDuration(lua_State* L);
	static int luaMonsterSpellSetConditionDrunkenness(lua_State* L);
	static int luaMonsterSpellSetConditionTickInterval(lua_State* L);
	static int luaMonsterSpellSetCombatShootEffect(lua_State* L);
	static int luaMonsterSpellSetCombatEffect(lua_State* L);
	static int luaMonsterSpellSetOutfit(lua_State* L);

	// Party
	static int luaPartyCreate(lua_State* L);
	static int luaPartyDisband(lua_State* L);

	static int luaPartyGetLeader(lua_State* L);
	static int luaPartySetLeader(lua_State* L);

	static int luaPartyGetMembers(lua_State* L);
	static int luaPartyGetMemberCount(lua_State* L);

	static int luaPartyGetInvitees(lua_State* L);
	static int luaPartyGetInviteeCount(lua_State* L);

	static int luaPartyAddInvite(lua_State* L);
	static int luaPartyRemoveInvite(lua_State* L);

	static int luaPartyAddMember(lua_State* L);
	static int luaPartyRemoveMember(lua_State* L);

	static int luaPartyIsSharedExperienceActive(lua_State* L);
	static int luaPartyIsSharedExperienceEnabled(lua_State* L);
	static int luaPartyShareExperience(lua_State* L);
	static int luaPartySetSharedExperience(lua_State* L);

	// Spells
	static int luaSpellCreate(lua_State* L);

	static int luaSpellOnCastSpell(lua_State* L);
	static int luaSpellRegister(lua_State* L);
	static int luaSpellName(lua_State* L);
	static int luaSpellId(lua_State* L);
	static int luaSpellGroup(lua_State* L);
	static int luaSpellCooldown(lua_State* L);
	static int luaSpellGroupCooldown(lua_State* L);
	static int luaSpellLevel(lua_State* L);
	static int luaSpellMagicLevel(lua_State* L);
	static int luaSpellMana(lua_State* L);
	static int luaSpellManaPercent(lua_State* L);
	static int luaSpellSoul(lua_State* L);
	static int luaSpellRange(lua_State* L);
	static int luaSpellPremium(lua_State* L);
	static int luaSpellEnabled(lua_State* L);
	static int luaSpellNeedTarget(lua_State* L);
	static int luaSpellNeedWeapon(lua_State* L);
	static int luaSpellNeedLearn(lua_State* L);
	static int luaSpellSelfTarget(lua_State* L);
	static int luaSpellBlocking(lua_State* L);
	static int luaSpellAggressive(lua_State* L);
	static int luaSpellPzLock(lua_State* L);
	static int luaSpellVocation(lua_State* L);

	// only for InstantSpells
	static int luaSpellWords(lua_State* L);
	static int luaSpellNeedDirection(lua_State* L);
	static int luaSpellHasParams(lua_State* L);
	static int luaSpellHasPlayerNameParam(lua_State* L);
	static int luaSpellNeedCasterTargetOrDirection(lua_State* L);
	static int luaSpellIsBlockingWalls(lua_State* L);

	// only for RuneSpells
	static int luaSpellRuneLevel(lua_State* L);
	static int luaSpellRuneMagicLevel(lua_State* L);
	static int luaSpellRuneId(lua_State* L);
	static int luaSpellCharges(lua_State* L);
	static int luaSpellAllowFarUse(lua_State* L);
	static int luaSpellBlockWalls(lua_State* L);
	static int luaSpellCheckFloor(lua_State* L);

	// Actions
	static int luaCreateAction(lua_State* L);
	static int luaActionOnUse(lua_State* L);
	static int luaActionRegister(lua_State* L);
	static int luaActionItemId(lua_State* L);
	static int luaActionActionId(lua_State* L);
	static int luaActionUniqueId(lua_State* L);
	static int luaActionAllowFarUse(lua_State* L);
	static int luaActionBlockWalls(lua_State* L);
	static int luaActionCheckFloor(lua_State* L);

	// Talkactions
	static int luaCreateTalkaction(lua_State* L);
	static int luaTalkactionOnSay(lua_State* L);
	static int luaTalkactionRegister(lua_State* L);
	static int luaTalkactionSeparator(lua_State* L);
	static int luaTalkactionAccess(lua_State* L);
	static int luaTalkactionAccountType(lua_State* L);

	// CreatureEvents
	static int luaCreateCreatureEvent(lua_State* L);
	static int luaCreatureEventType(lua_State* L);
	static int luaCreatureEventRegister(lua_State* L);
	static int luaCreatureEventOnCallback(lua_State* L);

	// MoveEvents
	static int luaCreateMoveEvent(lua_State* L);
	static int luaMoveEventType(lua_State* L);
	static int luaMoveEventRegister(lua_State* L);
	static int luaMoveEventOnCallback(lua_State* L);
	static int luaMoveEventLevel(lua_State* L);
	static int luaMoveEventSlot(lua_State* L);
	static int luaMoveEventMagLevel(lua_State* L);
	static int luaMoveEventPremium(lua_State* L);
	static int luaMoveEventVocation(lua_State* L);
	static int luaMoveEventTileItem(lua_State* L);
	static int luaMoveEventItemId(lua_State* L);
	static int luaMoveEventActionId(lua_State* L);
	static int luaMoveEventUniqueId(lua_State* L);
	static int luaMoveEventPosition(lua_State* L);

	// GlobalEvents
	static int luaCreateGlobalEvent(lua_State* L);
	static int luaGlobalEventType(lua_State* L);
	static int luaGlobalEventRegister(lua_State* L);
	static int luaGlobalEventOnCallback(lua_State* L);
	static int luaGlobalEventTime(lua_State* L);
	static int luaGlobalEventInterval(lua_State* L);

	// Weapon
	static int luaCreateWeapon(lua_State* L);
	static int luaWeaponId(lua_State* L);
	static int luaWeaponLevel(lua_State* L);
	static int luaWeaponMagicLevel(lua_State* L);
	static int luaWeaponMana(lua_State* L);
	static int luaWeaponManaPercent(lua_State* L);
	static int luaWeaponHealth(lua_State* L);
	static int luaWeaponHealthPercent(lua_State* L);
	static int luaWeaponSoul(lua_State* L);
	static int luaWeaponPremium(lua_State* L);
	static int luaWeaponBreakChance(lua_State* L);
	static int luaWeaponAction(lua_State* L);
	static int luaWeaponUnproperly(lua_State* L);
	static int luaWeaponVocation(lua_State* L);
	static int luaWeaponOnUseWeapon(lua_State* L);
	static int luaWeaponRegister(lua_State* L);
	static int luaWeaponElement(lua_State* L);
	static int luaWeaponAttack(lua_State* L);
	static int luaWeaponDefense(lua_State* L);
	static int luaWeaponRange(lua_State* L);
	static int luaWeaponCharges(lua_State* L);
	static int luaWeaponDuration(lua_State* L);
	static int luaWeaponDecayTo(lua_State* L);
	static int luaWeaponTransformEquipTo(lua_State* L);
	static int luaWeaponTransformDeEquipTo(lua_State* L);
	static int luaWeaponSlotType(lua_State* L);
	static int luaWeaponHitChance(lua_State* L);
	static int luaWeaponExtraElement(lua_State* L);

	// exclusively for distance weapons
	static int luaWeaponMaxHitChance(lua_State* L);
	static int luaWeaponAmmoType(lua_State* L);

	// exclusively for wands
	static int luaWeaponWandDamage(lua_State* L);

	// exclusively for wands & distance weapons
	static int luaWeaponShootType(lua_State* L);

	// XML
	static int luaCreateXmlDocument(lua_State* L);
	static int luaDeleteXmlDocument(lua_State* L);
	static int luaXmlDocumentChild(lua_State* L);

	static int luaDeleteXmlNode(lua_State* L);
	static int luaXmlNodeAttribute(lua_State* L);
	static int luaXmlNodeName(lua_State* L);
	static int luaXmlNodeFirstChild(lua_State* L);
	static int luaXmlNodeNextSibling(lua_State* L);

	//
	std::string lastLuaError;

	std::string interfaceName;

	static ScriptEnvironment scriptEnv[16];
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

	bool initState() override;
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
void getFieldBlock(lua_State* L, int32_t arg, FieldBlock& fieldBlock);

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

// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "combat.h"
#include "game.h"
#include "housetile.h"
#include "luascript.h"
#include "tile.h"

extern Game g_game;

using namespace Lua;

static int luaTileCreate(lua_State* L)
{
	// Tile(x, y, z)
	// Tile(position)
	Tile* tile;
	if (isTable(L, 2)) {
		tile = g_game.map.getTile(getPosition(L, 2));
	} else {
		uint8_t z = getNumber<uint8_t>(L, 4);
		uint16_t y = getNumber<uint16_t>(L, 3);
		uint16_t x = getNumber<uint16_t>(L, 2);
		tile = g_game.map.getTile(x, y, z);
	}

	if (tile) {
		pushUserdata<Tile>(L, tile);
		setMetatable(L, -1, "Tile");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileRemove(lua_State* L)
{
	// tile:remove()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	if (g_game.isTileInCleanList(tile)) {
		g_game.removeTileToClean(tile);
	}

	g_game.map.removeTile(tile->getPosition());
	pushBoolean(L, true);
	return 1;
}

static int luaTileGetPosition(lua_State* L)
{
	// tile:getPosition()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (tile) {
		pushPosition(L, tile->getPosition());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileGetGround(lua_State* L)
{
	// tile:getGround()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (tile && tile->getGround()) {
		pushUserdata<Item>(L, tile->getGround());
		setItemMetatable(L, -1, tile->getGround());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileGetThing(lua_State* L)
{
	// tile:getThing(index)
	int32_t index = getNumber<int32_t>(L, 2);
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	Thing* thing = tile->getThing(index);
	if (!thing) {
		lua_pushnil(L);
		return 1;
	}

	if (Creature* creature = thing->getCreature()) {
		pushUserdata<Creature>(L, creature);
		setCreatureMetatable(L, -1, creature);
	} else if (Item* item = thing->getItem()) {
		pushUserdata<Item>(L, item);
		setItemMetatable(L, -1, item);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileGetThingCount(lua_State* L)
{
	// tile:getThingCount()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (tile) {
		lua_pushnumber(L, tile->getThingCount());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileGetTopVisibleThing(lua_State* L)
{
	// tile:getTopVisibleThing(creature)
	Creature* creature = getCreature(L, 2);
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	Thing* thing = tile->getTopVisibleThing(creature);
	if (!thing) {
		lua_pushnil(L);
		return 1;
	}

	if (Creature* visibleCreature = thing->getCreature()) {
		pushUserdata<Creature>(L, visibleCreature);
		setCreatureMetatable(L, -1, visibleCreature);
	} else if (Item* visibleItem = thing->getItem()) {
		pushUserdata<Item>(L, visibleItem);
		setItemMetatable(L, -1, visibleItem);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileGetTopTopItem(lua_State* L)
{
	// tile:getTopTopItem()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	Item* item = tile->getTopTopItem();
	if (item) {
		pushUserdata<Item>(L, item);
		setItemMetatable(L, -1, item);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileGetTopDownItem(lua_State* L)
{
	// tile:getTopDownItem()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	Item* item = tile->getTopDownItem();
	if (item) {
		pushUserdata<Item>(L, item);
		setItemMetatable(L, -1, item);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileGetFieldItem(lua_State* L)
{
	// tile:getFieldItem()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	Item* item = tile->getFieldItem();
	if (item) {
		pushUserdata<Item>(L, item);
		setItemMetatable(L, -1, item);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileGetItemById(lua_State* L)
{
	// tile:getItemById(itemId[, subType = -1])
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	uint16_t itemId;
	if (isNumber(L, 2)) {
		itemId = getNumber<uint16_t>(L, 2);
	} else {
		itemId = Item::items.getItemIdByName(getString(L, 2));
		if (itemId == 0) {
			lua_pushnil(L);
			return 1;
		}
	}
	int32_t subType = getNumber<int32_t>(L, 3, -1);

	Item* item = g_game.findItemOfType(tile, itemId, false, subType);
	if (item) {
		pushUserdata<Item>(L, item);
		setItemMetatable(L, -1, item);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileGetItemByType(lua_State* L)
{
	// tile:getItemByType(itemType)
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	bool found;

	ItemTypes_t itemType = getNumber<ItemTypes_t>(L, 2);
	switch (itemType) {
		case ITEM_TYPE_TELEPORT:
			found = tile->hasFlag(TILESTATE_TELEPORT);
			break;
		case ITEM_TYPE_MAGICFIELD:
			found = tile->hasFlag(TILESTATE_MAGICFIELD);
			break;
		case ITEM_TYPE_MAILBOX:
			found = tile->hasFlag(TILESTATE_MAILBOX);
			break;
		case ITEM_TYPE_TRASHHOLDER:
			found = tile->hasFlag(TILESTATE_TRASHHOLDER);
			break;
		case ITEM_TYPE_BED:
			found = tile->hasFlag(TILESTATE_BED);
			break;
		case ITEM_TYPE_DEPOT:
			found = tile->hasFlag(TILESTATE_DEPOT);
			break;
		default:
			found = true;
			break;
	}

	if (!found) {
		lua_pushnil(L);
		return 1;
	}

	if (Item* item = tile->getGround()) {
		const ItemType& it = Item::items[item->getID()];
		if (it.type == itemType) {
			pushUserdata<Item>(L, item);
			setItemMetatable(L, -1, item);
			return 1;
		}
	}

	if (const TileItemVector* items = tile->getItemList()) {
		for (Item* item : *items) {
			const ItemType& it = Item::items[item->getID()];
			if (it.type == itemType) {
				pushUserdata<Item>(L, item);
				setItemMetatable(L, -1, item);
				return 1;
			}
		}
	}

	lua_pushnil(L);
	return 1;
}

static int luaTileGetItemByTopOrder(lua_State* L)
{
	// tile:getItemByTopOrder(topOrder)
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	int32_t topOrder = getNumber<int32_t>(L, 2);

	Item* item = tile->getItemByTopOrder(topOrder);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	pushUserdata<Item>(L, item);
	setItemMetatable(L, -1, item);
	return 1;
}

static int luaTileGetItemCountById(lua_State* L)
{
	// tile:getItemCountById(itemId[, subType = -1])
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	int32_t subType = getNumber<int32_t>(L, 3, -1);

	uint16_t itemId;
	if (isNumber(L, 2)) {
		itemId = getNumber<uint16_t>(L, 2);
	} else {
		itemId = Item::items.getItemIdByName(getString(L, 2));
		if (itemId == 0) {
			lua_pushnil(L);
			return 1;
		}
	}

	lua_pushnumber(L, tile->getItemTypeCount(itemId, subType));
	return 1;
}

static int luaTileGetBottomCreature(lua_State* L)
{
	// tile:getBottomCreature()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	const Creature* creature = tile->getBottomCreature();
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	pushUserdata<const Creature>(L, creature);
	setCreatureMetatable(L, -1, creature);
	return 1;
}

static int luaTileGetTopCreature(lua_State* L)
{
	// tile:getTopCreature()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	Creature* creature = tile->getTopCreature();
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	pushUserdata<Creature>(L, creature);
	setCreatureMetatable(L, -1, creature);
	return 1;
}

static int luaTileGetBottomVisibleCreature(lua_State* L)
{
	// tile:getBottomVisibleCreature(creature)
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	Creature* creature = getCreature(L, 2);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	const Creature* visibleCreature = tile->getBottomVisibleCreature(creature);
	if (visibleCreature) {
		pushUserdata<const Creature>(L, visibleCreature);
		setCreatureMetatable(L, -1, visibleCreature);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileGetTopVisibleCreature(lua_State* L)
{
	// tile:getTopVisibleCreature(creature)
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	Creature* creature = getCreature(L, 2);
	if (!creature) {
		lua_pushnil(L);
		return 1;
	}

	Creature* visibleCreature = tile->getTopVisibleCreature(creature);
	if (visibleCreature) {
		pushUserdata<Creature>(L, visibleCreature);
		setCreatureMetatable(L, -1, visibleCreature);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileGetItems(lua_State* L)
{
	// tile:getItems()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	TileItemVector* itemVector = tile->getItemList();
	if (!itemVector) {
		lua_pushnil(L);
		return 1;
	}

	lua_createtable(L, itemVector->size(), 0);

	int index = 0;
	for (Item* item : *itemVector) {
		pushUserdata<Item>(L, item);
		setItemMetatable(L, -1, item);
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaTileGetItemCount(lua_State* L)
{
	// tile:getItemCount()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushnumber(L, tile->getItemCount());
	return 1;
}

static int luaTileGetDownItemCount(lua_State* L)
{
	// tile:getDownItemCount()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (tile) {
		lua_pushnumber(L, tile->getDownItemCount());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileGetTopItemCount(lua_State* L)
{
	// tile:getTopItemCount()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushnumber(L, tile->getTopItemCount());
	return 1;
}

static int luaTileGetCreatures(lua_State* L)
{
	// tile:getCreatures()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	CreatureVector* creatureVector = tile->getCreatures();
	if (!creatureVector) {
		lua_pushnil(L);
		return 1;
	}

	lua_createtable(L, creatureVector->size(), 0);

	int index = 0;
	for (Creature* creature : *creatureVector) {
		pushUserdata<Creature>(L, creature);
		setCreatureMetatable(L, -1, creature);
		lua_rawseti(L, -2, ++index);
	}
	return 1;
}

static int luaTileGetCreatureCount(lua_State* L)
{
	// tile:getCreatureCount()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushnumber(L, tile->getCreatureCount());
	return 1;
}

static int luaTileHasProperty(lua_State* L)
{
	// tile:hasProperty(property[, item])
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	Item* item;
	if (lua_gettop(L) >= 3) {
		item = getUserdata<Item>(L, 3);
	} else {
		item = nullptr;
	}

	ITEMPROPERTY property = getNumber<ITEMPROPERTY>(L, 2);
	if (item) {
		pushBoolean(L, tile->hasProperty(item, property));
	} else {
		pushBoolean(L, tile->hasProperty(property));
	}
	return 1;
}

static int luaTileGetThingIndex(lua_State* L)
{
	// tile:getThingIndex(thing)
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	Thing* thing = getThing(L, 2);
	if (thing) {
		lua_pushnumber(L, tile->getThingIndex(thing));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileHasFlag(lua_State* L)
{
	// tile:hasFlag(flag)
	Tile* tile = getUserdata<Tile>(L, 1);
	if (tile) {
		tileflags_t flag = getNumber<tileflags_t>(L, 2);
		pushBoolean(L, tile->hasFlag(flag));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileQueryAdd(lua_State* L)
{
	// tile:queryAdd(thing[, flags])
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	Thing* thing = getThing(L, 2);
	if (thing) {
		uint32_t flags = getNumber<uint32_t>(L, 3, 0);
		lua_pushnumber(L, tile->queryAdd(0, *thing, 1, flags));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileAddItem(lua_State* L)
{
	// tile:addItem(itemId[, count/subType = 1[, flags = 0]])
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	uint16_t itemId;
	if (isNumber(L, 2)) {
		itemId = getNumber<uint16_t>(L, 2);
	} else {
		itemId = Item::items.getItemIdByName(getString(L, 2));
		if (itemId == 0) {
			lua_pushnil(L);
			return 1;
		}
	}

	uint32_t subType = getNumber<uint32_t>(L, 3, 1);

	Item* item = Item::CreateItem(itemId, std::min<uint32_t>(subType, 100));
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	uint32_t flags = getNumber<uint32_t>(L, 4, 0);

	ReturnValue ret = g_game.internalAddItem(tile, item, INDEX_WHEREEVER, flags);
	if (ret == RETURNVALUE_NOERROR) {
		pushUserdata<Item>(L, item);
		setItemMetatable(L, -1, item);
	} else {
		delete item;
		lua_pushnil(L);
	}
	return 1;
}

static int luaTileAddItemEx(lua_State* L)
{
	// tile:addItemEx(item[, flags = 0])
	Item* item = getUserdata<Item>(L, 2);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	if (item->getParent() != VirtualCylinder::virtualCylinder) {
		reportErrorFunc(L, "Item already has a parent");
		lua_pushnil(L);
		return 1;
	}

	uint32_t flags = getNumber<uint32_t>(L, 3, 0);
	ReturnValue ret = g_game.internalAddItem(tile, item, INDEX_WHEREEVER, flags);
	if (ret == RETURNVALUE_NOERROR) {
		ScriptEnvironment::removeTempItem(item);
	}
	lua_pushnumber(L, ret);
	return 1;
}

static int luaTileGetHouse(lua_State* L)
{
	// tile:getHouse()
	Tile* tile = getUserdata<Tile>(L, 1);
	if (!tile) {
		lua_pushnil(L);
		return 1;
	}

	if (HouseTile* houseTile = dynamic_cast<HouseTile*>(tile)) {
		pushUserdata<House>(L, houseTile->getHouse());
		setMetatable(L, -1, "House");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

namespace LuaTile {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("Tile", "", luaTileCreate);
	interface->registerMetaMethod("Tile", "__eq", interface->luaUserdataCompare);

	interface->registerMethod("Tile", "remove", luaTileRemove);

	interface->registerMethod("Tile", "getPosition", luaTileGetPosition);
	interface->registerMethod("Tile", "getGround", luaTileGetGround);
	interface->registerMethod("Tile", "getThing", luaTileGetThing);
	interface->registerMethod("Tile", "getThingCount", luaTileGetThingCount);
	interface->registerMethod("Tile", "getTopVisibleThing", luaTileGetTopVisibleThing);

	interface->registerMethod("Tile", "getTopTopItem", luaTileGetTopTopItem);
	interface->registerMethod("Tile", "getTopDownItem", luaTileGetTopDownItem);
	interface->registerMethod("Tile", "getFieldItem", luaTileGetFieldItem);

	interface->registerMethod("Tile", "getItemById", luaTileGetItemById);
	interface->registerMethod("Tile", "getItemByType", luaTileGetItemByType);
	interface->registerMethod("Tile", "getItemByTopOrder", luaTileGetItemByTopOrder);
	interface->registerMethod("Tile", "getItemCountById", luaTileGetItemCountById);

	interface->registerMethod("Tile", "getBottomCreature", luaTileGetBottomCreature);
	interface->registerMethod("Tile", "getTopCreature", luaTileGetTopCreature);
	interface->registerMethod("Tile", "getBottomVisibleCreature", luaTileGetBottomVisibleCreature);
	interface->registerMethod("Tile", "getTopVisibleCreature", luaTileGetTopVisibleCreature);

	interface->registerMethod("Tile", "getItems", luaTileGetItems);
	interface->registerMethod("Tile", "getItemCount", luaTileGetItemCount);
	interface->registerMethod("Tile", "getDownItemCount", luaTileGetDownItemCount);
	interface->registerMethod("Tile", "getTopItemCount", luaTileGetTopItemCount);

	interface->registerMethod("Tile", "getCreatures", luaTileGetCreatures);
	interface->registerMethod("Tile", "getCreatureCount", luaTileGetCreatureCount);

	interface->registerMethod("Tile", "getThingIndex", luaTileGetThingIndex);

	interface->registerMethod("Tile", "hasProperty", luaTileHasProperty);
	interface->registerMethod("Tile", "hasFlag", luaTileHasFlag);

	interface->registerMethod("Tile", "queryAdd", luaTileQueryAdd);
	interface->registerMethod("Tile", "addItem", luaTileAddItem);
	interface->registerMethod("Tile", "addItemEx", luaTileAddItemEx);

	interface->registerMethod("Tile", "getHouse", luaTileGetHouse);
}
} // namespace LuaTile

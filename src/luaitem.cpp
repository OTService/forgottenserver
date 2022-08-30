// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"
#include "luaitem.h"
#include "luascript.h"
#include "item.h"
#include "game.h"

extern Game g_game;

void LuaScriptInterface::registerItemFunctions()
{
	// Item
	registerClass("Item", "", LuaItem::luaItemCreate);
	registerMetaMethod("Item", "__eq", LuaItem::luaUserdataCompare);

	registerMethod("Item", "isItem", LuaItem::luaItemIsItem);

	registerMethod("Item", "getParent", LuaItem::luaItemGetParent);
	registerMethod("Item", "getTopParent", LuaItem::luaItemGetTopParent);

	registerMethod("Item", "getId", LuaItem::luaItemGetId);

	registerMethod("Item", "clone", LuaItem::luaItemClone);
	registerMethod("Item", "split", LuaItem::luaItemSplit);
	registerMethod("Item", "remove", LuaItem::luaItemRemove);

	registerMethod("Item", "getUniqueId", LuaItem::luaItemGetUniqueId);
	registerMethod("Item", "getActionId", LuaItem::luaItemGetActionId);
	registerMethod("Item", "setActionId", LuaItem::luaItemSetActionId);

	registerMethod("Item", "getCount", LuaItem::luaItemGetCount);
	registerMethod("Item", "getCharges", LuaItem::luaItemGetCharges);
	registerMethod("Item", "getFluidType", LuaItem::luaItemGetFluidType);
	registerMethod("Item", "getWeight", LuaItem::luaItemGetWeight);
	registerMethod("Item", "getWorth", LuaItem::luaItemGetWorth);

	registerMethod("Item", "getSubType", LuaItem::luaItemGetSubType);

	registerMethod("Item", "getName", LuaItem::luaItemGetName);
	registerMethod("Item", "getPluralName", LuaItem::luaItemGetPluralName);
	registerMethod("Item", "getArticle", LuaItem::luaItemGetArticle);

	registerMethod("Item", "getPosition", LuaItem::luaItemGetPosition);
	registerMethod("Item", "getTile", LuaItem::luaItemGetTile);

	registerMethod("Item", "hasAttribute", LuaItem::luaItemHasAttribute);
	registerMethod("Item", "getAttribute", LuaItem::luaItemGetAttribute);
	registerMethod("Item", "setAttribute", LuaItem::luaItemSetAttribute);
	registerMethod("Item", "removeAttribute", LuaItem::luaItemRemoveAttribute);
	registerMethod("Item", "getCustomAttribute", LuaItem::luaItemGetCustomAttribute);
	registerMethod("Item", "setCustomAttribute", LuaItem::luaItemSetCustomAttribute);
	registerMethod("Item", "removeCustomAttribute", LuaItem::luaItemRemoveCustomAttribute);

	registerMethod("Item", "moveTo", LuaItem::luaItemMoveTo);
	registerMethod("Item", "transform", LuaItem::luaItemTransform);
	registerMethod("Item", "decay", LuaItem::luaItemDecay);

	registerMethod("Item", "getSpecialDescription", LuaItem::luaItemGetSpecialDescription);

	registerMethod("Item", "hasProperty", LuaItem::luaItemHasProperty);
	registerMethod("Item", "isLoadedFromMap", LuaItem::luaItemIsLoadedFromMap);

	registerMethod("Item", "setStoreItem", LuaItem::luaItemSetStoreItem);
	registerMethod("Item", "isStoreItem", LuaItem::luaItemIsStoreItem);

	registerMethod("Item", "setReflect", LuaItem::luaItemSetReflect);
	registerMethod("Item", "getReflect", LuaItem::luaItemGetReflect);

	registerMethod("Item", "setBoostPercent", LuaItem::luaItemSetBoostPercent);
	registerMethod("Item", "getBoostPercent", LuaItem::luaItemGetBoostPercent);
}

int LuaItem::luaItemCreate(lua_State* L)
{
	// Item(uid)
	uint32_t id = getNumber<uint32_t>(L, 2);

	Item* item = getScriptEnv()->getItemByUID(id);
	if (item) {
		pushUserdata<Item>(L, item);
		setItemMetatable(L, -1, item);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemIsItem(lua_State* L)
{
	// item:isItem()
	pushBoolean(L, getUserdata<const Item>(L, 1) != nullptr);
	return 1;
}

int LuaItem::luaItemGetParent(lua_State* L)
{
	// item:getParent()
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	Cylinder* parent = item->getParent();
	if (!parent) {
		lua_pushnil(L);
		return 1;
	}

	pushCylinder(L, parent);
	return 1;
}

int LuaItem::luaItemGetTopParent(lua_State* L)
{
	// item:getTopParent()
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	Cylinder* topParent = item->getTopParent();
	if (!topParent) {
		lua_pushnil(L);
		return 1;
	}

	pushCylinder(L, topParent);
	return 1;
}

int LuaItem::luaItemGetId(lua_State* L)
{
	// item:getId()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		lua_pushnumber(L, item->getID());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemClone(lua_State* L)
{
	// item:clone()
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	Item* clone = item->clone();
	if (!clone) {
		lua_pushnil(L);
		return 1;
	}

	getScriptEnv()->addTempItem(clone);
	clone->setParent(VirtualCylinder::virtualCylinder);

	pushUserdata<Item>(L, clone);
	setItemMetatable(L, -1, clone);
	return 1;
}

int LuaItem::luaItemSplit(lua_State* L)
{
	// item:split([count = 1])
	Item** itemPtr = getRawUserdata<Item>(L, 1);
	if (!itemPtr) {
		lua_pushnil(L);
		return 1;
	}

	Item* item = *itemPtr;
	if (!item || !item->isStackable()) {
		lua_pushnil(L);
		return 1;
	}

	uint16_t count = std::min<uint16_t>(getNumber<uint16_t>(L, 2, 1), item->getItemCount());
	uint16_t diff = item->getItemCount() - count;

	Item* splitItem = item->clone();
	if (!splitItem) {
		lua_pushnil(L);
		return 1;
	}

	splitItem->setItemCount(count);

	ScriptEnvironment* env = getScriptEnv();
	uint32_t uid = env->addThing(item);

	Item* newItem = g_game.transformItem(item, item->getID(), diff);
	if (item->isRemoved()) {
		env->removeItemByUID(uid);
	}

	if (newItem && newItem != item) {
		env->insertItem(uid, newItem);
	}

	*itemPtr = newItem;

	splitItem->setParent(VirtualCylinder::virtualCylinder);
	env->addTempItem(splitItem);

	pushUserdata<Item>(L, splitItem);
	setItemMetatable(L, -1, splitItem);
	return 1;
}

int LuaItem::luaItemRemove(lua_State* L)
{
	// item:remove([count = -1])
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		int32_t count = getNumber<int32_t>(L, 2, -1);
		pushBoolean(L, g_game.internalRemoveItem(item, count) == RETURNVALUE_NOERROR);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetUniqueId(lua_State* L)
{
	// item:getUniqueId()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		uint32_t uniqueId = item->getUniqueId();
		if (uniqueId == 0) {
			uniqueId = getScriptEnv()->addThing(item);
		}
		lua_pushnumber(L, uniqueId);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetActionId(lua_State* L)
{
	// item:getActionId()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		lua_pushnumber(L, item->getActionId());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemSetActionId(lua_State* L)
{
	// item:setActionId(actionId)
	uint16_t actionId = getNumber<uint16_t>(L, 2);
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		item->setActionId(actionId);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetCount(lua_State* L)
{
	// item:getCount()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		lua_pushnumber(L, item->getItemCount());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetCharges(lua_State* L)
{
	// item:getCharges()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		lua_pushnumber(L, item->getCharges());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetFluidType(lua_State* L)
{
	// item:getFluidType()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		lua_pushnumber(L, item->getFluidType());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetWeight(lua_State* L)
{
	// item:getWeight()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		lua_pushnumber(L, item->getWeight());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetWorth(lua_State* L)
{
	// item:getWorth()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		lua_pushnumber(L, item->getWorth());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetSubType(lua_State* L)
{
	// item:getSubType()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		lua_pushnumber(L, item->getSubType());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetName(lua_State* L)
{
	// item:getName()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		pushString(L, item->getName());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetPluralName(lua_State* L)
{
	// item:getPluralName()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		pushString(L, item->getPluralName());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetArticle(lua_State* L)
{
	// item:getArticle()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		pushString(L, item->getArticle());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetPosition(lua_State* L)
{
	// item:getPosition()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		pushPosition(L, item->getPosition());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetTile(lua_State* L)
{
	// item:getTile()
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	Tile* tile = item->getTile();
	if (tile) {
		pushUserdata<Tile>(L, tile);
		setMetatable(L, -1, "Tile");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemHasAttribute(lua_State* L)
{
	// item:hasAttribute(key)
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	itemAttrTypes attribute;
	if (isNumber(L, 2)) {
		attribute = getNumber<itemAttrTypes>(L, 2);
	} else if (isString(L, 2)) {
		attribute = stringToItemAttribute(getString(L, 2));
	} else {
		attribute = ITEM_ATTRIBUTE_NONE;
	}

	pushBoolean(L, item->hasAttribute(attribute));
	return 1;
}

int LuaItem::luaItemGetAttribute(lua_State* L)
{
	// item:getAttribute(key)
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	itemAttrTypes attribute;
	if (isNumber(L, 2)) {
		attribute = getNumber<itemAttrTypes>(L, 2);
	} else if (isString(L, 2)) {
		attribute = stringToItemAttribute(getString(L, 2));
	} else {
		attribute = ITEM_ATTRIBUTE_NONE;
	}

	if (ItemAttributes::isIntAttrType(attribute)) {
		lua_pushnumber(L, item->getIntAttr(attribute));
	} else if (ItemAttributes::isStrAttrType(attribute)) {
		pushString(L, item->getStrAttr(attribute));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemSetAttribute(lua_State* L)
{
	// item:setAttribute(key, value)
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	itemAttrTypes attribute;
	if (isNumber(L, 2)) {
		attribute = getNumber<itemAttrTypes>(L, 2);
	} else if (isString(L, 2)) {
		attribute = stringToItemAttribute(getString(L, 2));
	} else {
		attribute = ITEM_ATTRIBUTE_NONE;
	}

	if (ItemAttributes::isIntAttrType(attribute)) {
		if (attribute == ITEM_ATTRIBUTE_UNIQUEID) {
			reportErrorFunc(L, "Attempt to set protected key \"uid\"");
			pushBoolean(L, false);
			return 1;
		}

		item->setIntAttr(attribute, getNumber<int32_t>(L, 3));
		pushBoolean(L, true);
	} else if (ItemAttributes::isStrAttrType(attribute)) {
		item->setStrAttr(attribute, getString(L, 3));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemRemoveAttribute(lua_State* L)
{
	// item:removeAttribute(key)
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	itemAttrTypes attribute;
	if (isNumber(L, 2)) {
		attribute = getNumber<itemAttrTypes>(L, 2);
	} else if (isString(L, 2)) {
		attribute = stringToItemAttribute(getString(L, 2));
	} else {
		attribute = ITEM_ATTRIBUTE_NONE;
	}

	bool ret = attribute != ITEM_ATTRIBUTE_UNIQUEID;
	if (ret) {
		item->removeAttribute(attribute);
	} else {
		reportErrorFunc(L, "Attempt to erase protected key \"uid\"");
	}
	pushBoolean(L, ret);
	return 1;
}

int LuaItem::luaItemGetCustomAttribute(lua_State* L)
{
	// item:getCustomAttribute(key)
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	const ItemAttributes::CustomAttribute* attr;
	if (isNumber(L, 2)) {
		attr = item->getCustomAttribute(getNumber<int64_t>(L, 2));
	} else if (isString(L, 2)) {
		attr = item->getCustomAttribute(getString(L, 2));
	} else {
		lua_pushnil(L);
		return 1;
	}

	if (attr) {
		attr->pushToLua(L);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemSetCustomAttribute(lua_State* L)
{
	// item:setCustomAttribute(key, value)
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	std::string key;
	if (isNumber(L, 2)) {
		key = std::to_string(getNumber<int64_t>(L, 2));
	} else if (isString(L, 2)) {
		key = getString(L, 2);
	} else {
		lua_pushnil(L);
		return 1;
	}

	ItemAttributes::CustomAttribute val;
	if (isNumber(L, 3)) {
		double tmp = getNumber<double>(L, 3);
		if (std::floor(tmp) < tmp) {
			val.set<double>(tmp);
		} else {
			val.set<int64_t>(tmp);
		}
	} else if (isString(L, 3)) {
		val.set<std::string>(getString(L, 3));
	} else if (isBoolean(L, 3)) {
		val.set<bool>(getBoolean(L, 3));
	} else {
		lua_pushnil(L);
		return 1;
	}

	item->setCustomAttribute(key, val);
	pushBoolean(L, true);
	return 1;
}

int LuaItem::luaItemRemoveCustomAttribute(lua_State* L)
{
	// item:removeCustomAttribute(key)
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	if (isNumber(L, 2)) {
		pushBoolean(L, item->removeCustomAttribute(getNumber<int64_t>(L, 2)));
	} else if (isString(L, 2)) {
		pushBoolean(L, item->removeCustomAttribute(getString(L, 2)));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemMoveTo(lua_State* L)
{
	// item:moveTo(position or cylinder[, flags])
	Item** itemPtr = getRawUserdata<Item>(L, 1);
	if (!itemPtr) {
		lua_pushnil(L);
		return 1;
	}

	Item* item = *itemPtr;
	if (!item || item->isRemoved()) {
		lua_pushnil(L);
		return 1;
	}

	Cylinder* toCylinder;
	if (isUserdata(L, 2)) {
		const LuaDataType type = getUserdataType(L, 2);
		switch (type) {
			case LuaData_Container:
				toCylinder = getUserdata<Container>(L, 2);
				break;
			case LuaData_Player:
				toCylinder = getUserdata<Player>(L, 2);
				break;
			case LuaData_Tile:
				toCylinder = getUserdata<Tile>(L, 2);
				break;
			default:
				toCylinder = nullptr;
				break;
		}
	} else {
		toCylinder = g_game.map.getTile(getPosition(L, 2));
	}

	if (!toCylinder) {
		lua_pushnil(L);
		return 1;
	}

	if (item->getParent() == toCylinder) {
		pushBoolean(L, true);
		return 1;
	}

	uint32_t flags = getNumber<uint32_t>(
	    L, 3, FLAG_NOLIMIT | FLAG_IGNOREBLOCKITEM | FLAG_IGNOREBLOCKCREATURE | FLAG_IGNORENOTMOVEABLE);

	if (item->getParent() == VirtualCylinder::virtualCylinder) {
		pushBoolean(L, g_game.internalAddItem(toCylinder, item, INDEX_WHEREEVER, flags) == RETURNVALUE_NOERROR);
	} else {
		Item* moveItem = nullptr;
		ReturnValue ret = g_game.internalMoveItem(item->getParent(), toCylinder, INDEX_WHEREEVER, item,
		                                          item->getItemCount(), &moveItem, flags);
		if (moveItem) {
			*itemPtr = moveItem;
		}
		pushBoolean(L, ret == RETURNVALUE_NOERROR);
	}
	return 1;
}

int LuaItem::luaItemTransform(lua_State* L)
{
	// item:transform(itemId[, count/subType = -1])
	Item** itemPtr = getRawUserdata<Item>(L, 1);
	if (!itemPtr) {
		lua_pushnil(L);
		return 1;
	}

	Item*& item = *itemPtr;
	if (!item) {
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
	if (item->getID() == itemId && (subType == -1 || subType == item->getSubType())) {
		pushBoolean(L, true);
		return 1;
	}

	const ItemType& it = Item::items[itemId];
	if (it.stackable) {
		subType = std::min<int32_t>(subType, 100);
	}

	ScriptEnvironment* env = getScriptEnv();
	uint32_t uid = env->addThing(item);

	Item* newItem = g_game.transformItem(item, itemId, subType);
	if (item->isRemoved()) {
		env->removeItemByUID(uid);
	}

	if (newItem && newItem != item) {
		env->insertItem(uid, newItem);
	}

	item = newItem;
	pushBoolean(L, true);
	return 1;
}

int LuaItem::luaItemDecay(lua_State* L)
{
	// item:decay(decayId)
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		if (isNumber(L, 2)) {
			item->setDecayTo(getNumber<int32_t>(L, 2));
		}

		g_game.startDecay(item);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemGetSpecialDescription(lua_State* L)
{
	// item:getSpecialDescription()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		pushString(L, item->getSpecialDescription());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemHasProperty(lua_State* L)
{
	// item:hasProperty(property)
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		ITEMPROPERTY property = getNumber<ITEMPROPERTY>(L, 2);
		pushBoolean(L, item->hasProperty(property));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemIsLoadedFromMap(lua_State* L)
{
	// item:isLoadedFromMap()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		pushBoolean(L, item->isLoadedFromMap());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemSetStoreItem(lua_State* L)
{
	// item:setStoreItem(storeItem)
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	item->setStoreItem(getBoolean(L, 2, false));
	return 1;
}

int LuaItem::luaItemIsStoreItem(lua_State* L)
{
	// item:isStoreItem()
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		pushBoolean(L, item->isStoreItem());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemSetReflect(lua_State* L)
{
	// item:setReflect(combatType, reflect)
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	item->setReflect(getNumber<CombatType_t>(L, 2), getReflect(L, 3));
	pushBoolean(L, true);
	return 1;
}

int LuaItem::luaItemGetReflect(lua_State* L)
{
	// item:getReflect(combatType[, total = true])
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		pushReflect(L, item->getReflect(getNumber<CombatType_t>(L, 2), getBoolean(L, 3, true)));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaItem::luaItemSetBoostPercent(lua_State* L)
{
	// item:setBoostPercent(combatType, percent)
	Item* item = getUserdata<Item>(L, 1);
	if (!item) {
		lua_pushnil(L);
		return 1;
	}

	item->setBoostPercent(getNumber<CombatType_t>(L, 2), getNumber<uint16_t>(L, 3));
	pushBoolean(L, true);
	return 1;
}

int LuaItem::luaItemGetBoostPercent(lua_State* L)
{
	// item:getBoostPercent(combatType[, total = true])
	Item* item = getUserdata<Item>(L, 1);
	if (item) {
		lua_pushnumber(L, item->getBoostPercent(getNumber<CombatType_t>(L, 2), getBoolean(L, 3, true)));
	} else {
		lua_pushnil(L);
	}
	return 1;
}


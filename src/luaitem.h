// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LUAITEM_H
#define FS_LUAITEM_H

#include "luascript.h"

class LuaItem : public LuaScriptInterface
{
public:
	static int luaItemCreate(lua_State* L);

	static int luaItemIsItem(lua_State* L);

	static int luaItemGetParent(lua_State* L);
	static int luaItemGetTopParent(lua_State* L);

	static int luaItemGetId(lua_State* L);

	static int luaItemClone(lua_State* L);
	static int luaItemSplit(lua_State* L);
	static int luaItemRemove(lua_State* L);

	static int luaItemGetUniqueId(lua_State* L);
	static int luaItemGetActionId(lua_State* L);
	static int luaItemSetActionId(lua_State* L);

	static int luaItemGetCount(lua_State* L);
	static int luaItemGetCharges(lua_State* L);
	static int luaItemGetFluidType(lua_State* L);
	static int luaItemGetWeight(lua_State* L);
	static int luaItemGetWorth(lua_State* L);

	static int luaItemGetSubType(lua_State* L);

	static int luaItemGetName(lua_State* L);
	static int luaItemGetPluralName(lua_State* L);
	static int luaItemGetArticle(lua_State* L);

	static int luaItemGetPosition(lua_State* L);
	static int luaItemGetTile(lua_State* L);

	static int luaItemHasAttribute(lua_State* L);
	static int luaItemGetAttribute(lua_State* L);
	static int luaItemSetAttribute(lua_State* L);
	static int luaItemRemoveAttribute(lua_State* L);
	static int luaItemGetCustomAttribute(lua_State* L);
	static int luaItemSetCustomAttribute(lua_State* L);
	static int luaItemRemoveCustomAttribute(lua_State* L);

	static int luaItemMoveTo(lua_State* L);
	static int luaItemTransform(lua_State* L);
	static int luaItemDecay(lua_State* L);

	static int luaItemGetSpecialDescription(lua_State* L);

	static int luaItemHasProperty(lua_State* L);
	static int luaItemIsLoadedFromMap(lua_State* L);

	static int luaItemSetStoreItem(lua_State* L);
	static int luaItemIsStoreItem(lua_State* L);

	static int luaItemSetReflect(lua_State* L);
	static int luaItemGetReflect(lua_State* L);

	static int luaItemSetBoostPercent(lua_State* L);
	static int luaItemGetBoostPercent(lua_State* L);

private:
	friend class LuaScriptInterface;
};

#endif // FS_LUAITEM_H

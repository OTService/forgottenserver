// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LUACONTAINER_H
#define FS_LUACONTAINER_H

#include "luascript.h"

class LuaContainer : public LuaScriptInterface
{
public:
	static int luaContainerCreate(lua_State* L);

	static int luaContainerGetSize(lua_State* L);
	static int luaContainerGetCapacity(lua_State* L);
	static int luaContainerGetEmptySlots(lua_State* L);
	static int luaContainerGetItems(lua_State* L);
	static int luaContainerGetItemHoldingCount(lua_State* L);
	static int luaContainerGetItemCountById(lua_State* L);

	static int luaContainerGetItem(lua_State* L);
	static int luaContainerHasItem(lua_State* L);
	static int luaContainerAddItem(lua_State* L);
	static int luaContainerAddItemEx(lua_State* L);
	static int luaContainerGetCorpseOwner(lua_State* L);

private:
	friend class LuaScriptInterface;
};

#endif // FS_LUACONTAINER_H

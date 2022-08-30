// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LUANETWORKMESSAGE_H
#define FS_LUANETWORKMESSAGE_H

#include "luascript.h"

class LuaNetworkMessage : public LuaScriptInterface
{
public:
	static int luaNetworkMessageCreate(lua_State* L);
	static int luaNetworkMessageDelete(lua_State* L);

	static int luaNetworkMessageGetByte(lua_State* L);
	static int luaNetworkMessageGetU16(lua_State* L);
	static int luaNetworkMessageGetU32(lua_State* L);
	static int luaNetworkMessageGetU64(lua_State* L);
	static int luaNetworkMessageGetString(lua_State* L);
	static int luaNetworkMessageGetPosition(lua_State* L);

	static int luaNetworkMessageAddByte(lua_State* L);
	static int luaNetworkMessageAddU16(lua_State* L);
	static int luaNetworkMessageAddU32(lua_State* L);
	static int luaNetworkMessageAddU64(lua_State* L);
	static int luaNetworkMessageAddString(lua_State* L);
	static int luaNetworkMessageAddPosition(lua_State* L);
	static int luaNetworkMessageAddDouble(lua_State* L);
	static int luaNetworkMessageAddItem(lua_State* L);
	static int luaNetworkMessageAddItemId(lua_State* L);

	static int luaNetworkMessageReset(lua_State* L);
	static int luaNetworkMessageSeek(lua_State* L);
	static int luaNetworkMessageTell(lua_State* L);
	static int luaNetworkMessageLength(lua_State* L);
	static int luaNetworkMessageSkipBytes(lua_State* L);
	static int luaNetworkMessageSendToPlayer(lua_State* L);

private:
	friend class LuaScriptInterface;
};

#endif // FS_LUANETWORKMESSAGE_H

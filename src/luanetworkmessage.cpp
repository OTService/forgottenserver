// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "luanetworkmessage.h"

#include "item.h"
#include "luascript.h"
#include "networkmessage.h"
#include "player.h"

void LuaScriptInterface::registerNetworkMessageFunctions()
{
	registerClass("NetworkMessage", "", LuaNetworkMessage::luaNetworkMessageCreate);
	registerMetaMethod("NetworkMessage", "__eq", LuaNetworkMessage::luaUserdataCompare);
	registerMetaMethod("NetworkMessage", "__gc", LuaNetworkMessage::luaNetworkMessageDelete);
	registerMethod("NetworkMessage", "delete", LuaNetworkMessage::luaNetworkMessageDelete);

	registerMethod("NetworkMessage", "getByte", LuaNetworkMessage::luaNetworkMessageGetByte);
	registerMethod("NetworkMessage", "getU16", LuaNetworkMessage::luaNetworkMessageGetU16);
	registerMethod("NetworkMessage", "getU32", LuaNetworkMessage::luaNetworkMessageGetU32);
	registerMethod("NetworkMessage", "getU64", LuaNetworkMessage::luaNetworkMessageGetU64);
	registerMethod("NetworkMessage", "getString", LuaNetworkMessage::luaNetworkMessageGetString);
	registerMethod("NetworkMessage", "getPosition", LuaNetworkMessage::luaNetworkMessageGetPosition);

	registerMethod("NetworkMessage", "addByte", LuaNetworkMessage::luaNetworkMessageAddByte);
	registerMethod("NetworkMessage", "addU16", LuaNetworkMessage::luaNetworkMessageAddU16);
	registerMethod("NetworkMessage", "addU32", LuaNetworkMessage::luaNetworkMessageAddU32);
	registerMethod("NetworkMessage", "addU64", LuaNetworkMessage::luaNetworkMessageAddU64);
	registerMethod("NetworkMessage", "addString", LuaNetworkMessage::luaNetworkMessageAddString);
	registerMethod("NetworkMessage", "addPosition", LuaNetworkMessage::luaNetworkMessageAddPosition);
	registerMethod("NetworkMessage", "addDouble", LuaNetworkMessage::luaNetworkMessageAddDouble);
	registerMethod("NetworkMessage", "addItem", LuaNetworkMessage::luaNetworkMessageAddItem);
	registerMethod("NetworkMessage", "addItemId", LuaNetworkMessage::luaNetworkMessageAddItemId);

	registerMethod("NetworkMessage", "reset", LuaNetworkMessage::luaNetworkMessageReset);
	registerMethod("NetworkMessage", "seek", LuaNetworkMessage::luaNetworkMessageSeek);
	registerMethod("NetworkMessage", "tell", LuaNetworkMessage::luaNetworkMessageTell);
	registerMethod("NetworkMessage", "len", LuaNetworkMessage::luaNetworkMessageLength);
	registerMethod("NetworkMessage", "skipBytes", LuaNetworkMessage::luaNetworkMessageSkipBytes);
	registerMethod("NetworkMessage", "sendToPlayer", LuaNetworkMessage::luaNetworkMessageSendToPlayer);
}

// NetworkMessage
int LuaNetworkMessage::luaNetworkMessageCreate(lua_State* L)
{
	// NetworkMessage()
	pushUserdata<NetworkMessage>(L, new NetworkMessage);
	setMetatable(L, -1, "NetworkMessage");
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageDelete(lua_State* L)
{
	NetworkMessage** messagePtr = getRawUserdata<NetworkMessage>(L, 1);
	if (messagePtr && *messagePtr) {
		delete *messagePtr;
		*messagePtr = nullptr;
	}
	return 0;
}

int LuaNetworkMessage::luaNetworkMessageGetByte(lua_State* L)
{
	// networkMessage:getByte()
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		lua_pushnumber(L, message->getByte());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageGetU16(lua_State* L)
{
	// networkMessage:getU16()
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		lua_pushnumber(L, message->get<uint16_t>());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageGetU32(lua_State* L)
{
	// networkMessage:getU32()
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		lua_pushnumber(L, message->get<uint32_t>());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageGetU64(lua_State* L)
{
	// networkMessage:getU64()
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		lua_pushnumber(L, message->get<uint64_t>());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageGetString(lua_State* L)
{
	// networkMessage:getString()
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		pushString(L, message->getString());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageGetPosition(lua_State* L)
{
	// networkMessage:getPosition()
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		pushPosition(L, message->getPosition());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageAddByte(lua_State* L)
{
	// networkMessage:addByte(number)
	uint8_t number = getNumber<uint8_t>(L, 2);
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		message->addByte(number);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageAddU16(lua_State* L)
{
	// networkMessage:addU16(number)
	uint16_t number = getNumber<uint16_t>(L, 2);
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		message->add<uint16_t>(number);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageAddU32(lua_State* L)
{
	// networkMessage:addU32(number)
	uint32_t number = getNumber<uint32_t>(L, 2);
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		message->add<uint32_t>(number);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageAddU64(lua_State* L)
{
	// networkMessage:addU64(number)
	uint64_t number = getNumber<uint64_t>(L, 2);
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		message->add<uint64_t>(number);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageAddString(lua_State* L)
{
	// networkMessage:addString(string)
	const std::string& string = getString(L, 2);
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		message->addString(string);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageAddPosition(lua_State* L)
{
	// networkMessage:addPosition(position)
	const Position& position = getPosition(L, 2);
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		message->addPosition(position);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageAddDouble(lua_State* L)
{
	// networkMessage:addDouble(number)
	double number = getNumber<double>(L, 2);
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		message->addDouble(number);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageAddItem(lua_State* L)
{
	// networkMessage:addItem(item)
	Item* item = getUserdata<Item>(L, 2);
	if (!item) {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_ITEM_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		message->addItem(item);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageAddItemId(lua_State* L)
{
	// networkMessage:addItemId(itemId)
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (!message) {
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

	message->addItemId(itemId);
	pushBoolean(L, true);
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageReset(lua_State* L)
{
	// networkMessage:reset()
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		message->reset();
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageSeek(lua_State* L)
{
	// networkMessage:seek(position)
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message && isNumber(L, 2)) {
		pushBoolean(L, message->setBufferPosition(getNumber<uint16_t>(L, 2)));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageTell(lua_State* L)
{
	// networkMessage:tell()
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		lua_pushnumber(L, message->getBufferPosition() - message->INITIAL_BUFFER_POSITION);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageLength(lua_State* L)
{
	// networkMessage:len()
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		lua_pushnumber(L, message->getLength());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageSkipBytes(lua_State* L)
{
	// networkMessage:skipBytes(number)
	int16_t number = getNumber<int16_t>(L, 2);
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (message) {
		message->skipBytes(number);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int LuaNetworkMessage::luaNetworkMessageSendToPlayer(lua_State* L)
{
	// networkMessage:sendToPlayer(player)
	NetworkMessage* message = getUserdata<NetworkMessage>(L, 1);
	if (!message) {
		lua_pushnil(L);
		return 1;
	}

	Player* player = getPlayer(L, 2);
	if (player) {
		player->sendNetworkMessage(*message);
		pushBoolean(L, true);
	} else {
		reportErrorFunc(L, getErrorDesc(LUA_ERROR_PLAYER_NOT_FOUND));
		lua_pushnil(L);
	}
	return 1;
}

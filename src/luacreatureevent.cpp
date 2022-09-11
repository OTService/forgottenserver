// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "creatureevent.h"
#include "luascript.h"
#include "script.h"

extern Scripts* g_scripts;
extern CreatureEvents* g_creatureEvents;

using namespace Lua;

static int luaCreateCreatureEvent(lua_State* L)
{
	// CreatureEvent({event = "login", name = "test"})
	if (LuaScriptInterface::getScriptEnv()->getScriptInterface() != &g_scripts->getScriptInterface()) {
		reportErrorFunc(L, "CreatureEvents can only be registered in the Scripts interface.");
		lua_pushnil(L);
		return 1;
	}

	auto creature = std::make_shared<CreatureEvent>(LuaScriptInterface::getScriptEnv()->getScriptInterface());
	if (creature) {
		// checking if it's old revscriptsys
		if (isString(L, 2)) {
			creature->setName(getString(L, 2));
		}
		creature->fromLua = true;
		pushSharedPtr<CreatureEvent_shared_ptr>(L, creature);
		setMetatable(L, -1, "CreatureEvent");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureEventType(lua_State* L)
{
	// creatureevent:type(callback) creatureevent:event(callback)
	CreatureEvent_shared_ptr creature = getSharedPtr<CreatureEvent>(L, 1);
	if (creature) {
		const std::string& tmpStr = boost::algorithm::to_lower_copy(getString(L, 2));
		if (tmpStr == "login") {
			creature->setEventType(CREATURE_EVENT_LOGIN);
		} else if (tmpStr == "logout") {
			creature->setEventType(CREATURE_EVENT_LOGOUT);
		} else if (tmpStr == "think") {
			creature->setEventType(CREATURE_EVENT_THINK);
		} else if (tmpStr == "preparedeath") {
			creature->setEventType(CREATURE_EVENT_PREPAREDEATH);
		} else if (tmpStr == "death") {
			creature->setEventType(CREATURE_EVENT_DEATH);
		} else if (tmpStr == "kill") {
			creature->setEventType(CREATURE_EVENT_KILL);
		} else if (tmpStr == "advance") {
			creature->setEventType(CREATURE_EVENT_ADVANCE);
		} else if (tmpStr == "modalwindow") {
			creature->setEventType(CREATURE_EVENT_MODALWINDOW);
		} else if (tmpStr == "textedit") {
			creature->setEventType(CREATURE_EVENT_TEXTEDIT);
		} else if (tmpStr == "healthchange") {
			creature->setEventType(CREATURE_EVENT_HEALTHCHANGE);
		} else if (tmpStr == "manachange") {
			creature->setEventType(CREATURE_EVENT_MANACHANGE);
		} else if (tmpStr == "extendedopcode") {
			creature->setEventType(CREATURE_EVENT_EXTENDED_OPCODE);
		} else {
			std::cout << "[Error - CreatureEvent::configureLuaEvent] Invalid type for creature event: " << tmpStr
			          << std::endl;
			pushBoolean(L, false);
		}
		creature->setLoaded(true);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureEventName(lua_State* L)
{
	// creatureevent:name(name)
	CreatureEvent_shared_ptr creature = getSharedPtr<CreatureEvent>(L, 1);
	if (creature) {
		const std::string& name = getString(L, 2);
		creature->setName(name);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureEventRegister(lua_State* L)
{
	// creatureevent:register()
	CreatureEvent_shared_ptr creature = getSharedPtr<CreatureEvent>(L, 1);
	if (creature) {
		if (!creature->isScripted()) {
			pushBoolean(L, false);
			return 1;
		}
		pushBoolean(L, g_creatureEvents->registerLuaEvent(creature));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureEventOnCallback(lua_State* L)
{
	// creatureevent:onLogin / logout / etc. (callback)
	CreatureEvent_shared_ptr creature = getSharedPtr<CreatureEvent>(L, 1);
	if (creature) {
		const std::string& functionName = getString(L, 2);
		bool fileName = false;
		const static std::vector<std::string> tmp = {
		    "onLogin",   "onLogout",      "onThink",    "onPrepareDeath", "onDeath",      "onKill",
		    "onAdvance", "onModalWindow", "onTextEdit", "onHealthChange", "onManaChange", "onExtendedOpcode"};
		for (auto& it : tmp) {
			if (it == functionName) {
				fileName = true;
				break;
			}
		}
		if (!creature->loadCallback(functionName, fileName)) {
			pushBoolean(L, false);
			return 1;
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaCreatureEventDelete(lua_State* L)
{
	CreatureEvent_shared_ptr& creature = getSharedPtr<CreatureEvent>(L, 1);
	if (creature) {
		creature.reset();
	}
	return 0;
}

namespace LuaCreatureEvent {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("CreatureEvent", "", luaCreateCreatureEvent);
	interface->registerMetaMethod("CreatureEvent", "__gc", luaCreatureEventDelete);
	interface->registerMethod("CreatureEvent", "type", luaCreatureEventType);
	interface->registerMethod("CreatureEvent", "event", luaCreatureEventType);
	interface->registerMethod("CreatureEvent", "name", luaCreatureEventName);
	interface->registerMethod("CreatureEvent", "register", luaCreatureEventRegister);
	interface->registerMethod("CreatureEvent", "onLogin", luaCreatureEventOnCallback);
	interface->registerMethod("CreatureEvent", "onLogout", luaCreatureEventOnCallback);
	interface->registerMethod("CreatureEvent", "onThink", luaCreatureEventOnCallback);
	interface->registerMethod("CreatureEvent", "onPrepareDeath", luaCreatureEventOnCallback);
	interface->registerMethod("CreatureEvent", "onDeath", luaCreatureEventOnCallback);
	interface->registerMethod("CreatureEvent", "onKill", luaCreatureEventOnCallback);
	interface->registerMethod("CreatureEvent", "onAdvance", luaCreatureEventOnCallback);
	interface->registerMethod("CreatureEvent", "onModalWindow", luaCreatureEventOnCallback);
	interface->registerMethod("CreatureEvent", "onTextEdit", luaCreatureEventOnCallback);
	interface->registerMethod("CreatureEvent", "onHealthChange", luaCreatureEventOnCallback);
	interface->registerMethod("CreatureEvent", "onManaChange", luaCreatureEventOnCallback);
	interface->registerMethod("CreatureEvent", "onExtendedOpcode", luaCreatureEventOnCallback);
}
} // namespace LuaCreatureEvent

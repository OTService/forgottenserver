// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "globalevent.h"
#include "luascript.h"
#include "script.h"
#include "tools.h"

extern Scripts* g_scripts;
extern GlobalEvents* g_globalEvents;

using namespace Lua;

static int luaCreateGlobalEvent(lua_State* L)
{
	// GlobalEvent({event = "think", name = "test"})
	if (LuaScriptInterface::getScriptEnv()->getScriptInterface() != &g_scripts->getScriptInterface()) {
		reportErrorFunc(L, "GlobalEvents can only be registered in the Scripts interface.");
		lua_pushnil(L);
		return 1;
	}

	GlobalEvent* raw = new GlobalEvent(LuaScriptInterface::getScriptEnv()->getScriptInterface());
	GlobalEvent_shared_ptr global(raw);
	if (global) {
		// checking for old revscriptsys
		if (isString(L, 2)) {
			global->setName(getString(L, 2));
		}
		global->setEventType(GLOBALEVENT_NONE);
		global->fromLua = true;
		pushSharedPtr<GlobalEvent_shared_ptr>(L, global);
		setMetatable(L, -1, "GlobalEvent");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaGlobalEventName(lua_State* L)
{
	// globalevent:name(name)
	GlobalEvent_shared_ptr global = getSharedPtr<GlobalEvent>(L, 1);
	if (global) {
		const std::string& name = getString(L, 2);
		global->setName(name);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaGlobalEventType(lua_State* L)
{
	// globalevent:type(callback)
	GlobalEvent_shared_ptr global = getSharedPtr<GlobalEvent>(L, 1);
	if (global) {
		const std::string& typeName = getString(L, 2);
		const std::string& tmpStr = boost::algorithm::to_lower_copy(typeName);
		if (tmpStr == "startup") {
			global->setEventType(GLOBALEVENT_STARTUP);
		} else if (tmpStr == "shutdown") {
			global->setEventType(GLOBALEVENT_SHUTDOWN);
		} else if (tmpStr == "record") {
			global->setEventType(GLOBALEVENT_RECORD);
		} else if (tmpStr == "time") {
			global->setEventType(GLOBALEVENT_TIMER);
		} else if (tmpStr == "think") {
			// just don't throw an error message due to non existing type
		} else {
			std::cout << "[Error - luaGlobalEventType] Invalid type for global event: " << typeName << std::endl;
			pushBoolean(L, false);
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaGlobalEventRegister(lua_State* L)
{
	// globalevent:register()
	GlobalEvent_shared_ptr global = getSharedPtr<GlobalEvent>(L, 1);
	if (global) {
		if (!global->isScripted()) {
			pushBoolean(L, false);
			return 1;
		}

		if (global->getEventType() == GLOBALEVENT_NONE && global->getInterval() == 0) {
			std::cout << "[Error - luaGlobalEventRegister] No interval/time for globalevent with name "
			          << global->getName() << std::endl;
			pushBoolean(L, false);
			return 1;
		}

		pushBoolean(L, g_globalEvents->registerLuaEvent(global));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaGlobalEventOnCallback(lua_State* L)
{
	// globalevent:onThink / record / etc. (callback)
	GlobalEvent_shared_ptr global = getSharedPtr<GlobalEvent>(L, 1);
	if (global) {
		const std::string& functionName = getString(L, 2);
		if (!global->loadCallback(functionName)) {
			pushBoolean(L, false);
			return 1;
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaGlobalEventTime(lua_State* L)
{
	// globalevent:time(time)
	GlobalEvent_shared_ptr global = getSharedPtr<GlobalEvent>(L, 1);
	if (global) {
		std::string timer = getString(L, 2);
		std::vector<int32_t> params = vectorAtoi(explodeString(timer, ":"));

		int32_t hour = params.front();
		if (hour < 0 || hour > 23) {
			std::cout << "[Error - luaGlobalEventTime] Invalid hour \"" << timer
			          << "\" for globalevent with name: " << global->getName() << std::endl;
			pushBoolean(L, false);
			return 1;
		}

		global->setInterval(hour << 16);

		int32_t min = 0;
		int32_t sec = 0;
		if (params.size() > 1) {
			min = params[1];
			if (min < 0 || min > 59) {
				std::cout << "[Error - luaGlobalEventTime] Invalid minute \"" << timer
				          << "\" for globalevent with name: " << global->getName() << std::endl;
				pushBoolean(L, false);
				return 1;
			}

			if (params.size() > 2) {
				sec = params[2];
				if (sec < 0 || sec > 59) {
					std::cout << "[Error - luaGlobalEventTime] Invalid second \"" << timer
					          << "\" for globalevent with name: " << global->getName() << std::endl;
					pushBoolean(L, false);
					return 1;
				}
			}
		}

		time_t current_time = time(nullptr);
		tm* timeinfo = localtime(&current_time);
		timeinfo->tm_hour = hour;
		timeinfo->tm_min = min;
		timeinfo->tm_sec = sec;

		time_t difference = static_cast<time_t>(difftime(mktime(timeinfo), current_time));
		if (difference < 0) {
			difference += 86400;
		}

		global->setNextExecution(current_time + difference);
		global->setEventType(GLOBALEVENT_TIMER);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaGlobalEventInterval(lua_State* L)
{
	// globalevent:interval(interval)
	GlobalEvent_shared_ptr global = getSharedPtr<GlobalEvent>(L, 1);
	if (global) {
		global->setInterval(getNumber<uint32_t>(L, 2));
		global->setNextExecution(OTSYS_TIME() + getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaGlobalEventDelete(lua_State* L)
{
	GlobalEvent_shared_ptr& global = getSharedPtr<GlobalEvent>(L, 1);
	if (global) {
		global.reset();
	}
	return 0;
}

namespace LuaGlobalEvent {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("GlobalEvent", "", luaCreateGlobalEvent);
	interface->registerMetaMethod("GlobalEvent", "__gc", luaGlobalEventDelete);
	interface->registerMethod("GlobalEvent", "name", luaGlobalEventName);
	interface->registerMethod("GlobalEvent", "type", luaGlobalEventType);
	interface->registerMethod("GlobalEvent", "event", luaGlobalEventType);
	interface->registerMethod("GlobalEvent", "register", luaGlobalEventRegister);
	interface->registerMethod("GlobalEvent", "time", luaGlobalEventTime);
	interface->registerMethod("GlobalEvent", "interval", luaGlobalEventInterval);
	interface->registerMethod("GlobalEvent", "onThink", luaGlobalEventOnCallback);
	interface->registerMethod("GlobalEvent", "onTime", luaGlobalEventOnCallback);
	interface->registerMethod("GlobalEvent", "onStartup", luaGlobalEventOnCallback);
	interface->registerMethod("GlobalEvent", "onShutdown", luaGlobalEventOnCallback);
	interface->registerMethod("GlobalEvent", "onRecord", luaGlobalEventOnCallback);
}
} // namespace LuaGlobalEvent

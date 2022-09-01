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
	// GlobalEvent(eventName)
	if (LuaScriptInterface::getScriptEnv()->getScriptInterface() != &g_scripts->getScriptInterface()) {
		reportErrorFunc(L, "GlobalEvents can only be registered in the Scripts interface.");
		lua_pushnil(L);
		return 1;
	}

	GlobalEvent* global = new GlobalEvent(LuaScriptInterface::getScriptEnv()->getScriptInterface());
	if (global) {
		global->setName(getString(L, 2));
		global->setEventType(GLOBALEVENT_NONE);
		global->fromLua = true;
		pushUserdata<GlobalEvent>(L, global);
		setMetatable(L, -1, "GlobalEvent");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaGlobalEventType(lua_State* L)
{
	// globalevent:type(callback)
	GlobalEvent* global = getUserdata<GlobalEvent>(L, 1);
	if (global) {
		std::string typeName = getString(L, 2);
		std::string tmpStr = boost::algorithm::to_lower_copy(typeName);
		if (tmpStr == "startup") {
			global->setEventType(GLOBALEVENT_STARTUP);
		} else if (tmpStr == "shutdown") {
			global->setEventType(GLOBALEVENT_SHUTDOWN);
		} else if (tmpStr == "record") {
			global->setEventType(GLOBALEVENT_RECORD);
		} else {
			std::cout << "[Error - CreatureEvent::configureLuaEvent] Invalid type for global event: " << typeName
			          << std::endl;
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
	GlobalEvent* globalevent = getUserdata<GlobalEvent>(L, 1);
	if (globalevent) {
		if (!globalevent->isScripted()) {
			pushBoolean(L, false);
			return 1;
		}

		if (globalevent->getEventType() == GLOBALEVENT_NONE && globalevent->getInterval() == 0) {
			std::cout << "[Error - LuaScriptInterface::luaGlobalEventRegister] No interval for globalevent with name "
			          << globalevent->getName() << std::endl;
			pushBoolean(L, false);
			return 1;
		}

		pushBoolean(L, g_globalEvents->registerLuaEvent(globalevent));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaGlobalEventOnCallback(lua_State* L)
{
	// globalevent:onThink / record / etc. (callback)
	GlobalEvent* globalevent = getUserdata<GlobalEvent>(L, 1);
	if (globalevent) {
		if (!globalevent->loadCallback()) {
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
	GlobalEvent* globalevent = getUserdata<GlobalEvent>(L, 1);
	if (globalevent) {
		std::string timer = getString(L, 2);
		std::vector<int32_t> params = vectorAtoi(explodeString(timer, ":"));

		int32_t hour = params.front();
		if (hour < 0 || hour > 23) {
			std::cout << "[Error - GlobalEvent::configureEvent] Invalid hour \"" << timer
			          << "\" for globalevent with name: " << globalevent->getName() << std::endl;
			pushBoolean(L, false);
			return 1;
		}

		globalevent->setInterval(hour << 16);

		int32_t min = 0;
		int32_t sec = 0;
		if (params.size() > 1) {
			min = params[1];
			if (min < 0 || min > 59) {
				std::cout << "[Error - GlobalEvent::configureEvent] Invalid minute \"" << timer
				          << "\" for globalevent with name: " << globalevent->getName() << std::endl;
				pushBoolean(L, false);
				return 1;
			}

			if (params.size() > 2) {
				sec = params[2];
				if (sec < 0 || sec > 59) {
					std::cout << "[Error - GlobalEvent::configureEvent] Invalid second \"" << timer
					          << "\" for globalevent with name: " << globalevent->getName() << std::endl;
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

		globalevent->setNextExecution(current_time + difference);
		globalevent->setEventType(GLOBALEVENT_TIMER);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaGlobalEventInterval(lua_State* L)
{
	// globalevent:interval(interval)
	GlobalEvent* globalevent = getUserdata<GlobalEvent>(L, 1);
	if (globalevent) {
		globalevent->setInterval(getNumber<uint32_t>(L, 2));
		globalevent->setNextExecution(OTSYS_TIME() + getNumber<uint32_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

namespace LuaGlobalEvent {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("GlobalEvent", "", luaCreateGlobalEvent);
	interface->registerMethod("GlobalEvent", "type", luaGlobalEventType);
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

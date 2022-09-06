// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "actions.h"
#include "luascript.h"
#include "script.h"

extern Scripts* g_scripts;
extern Actions* g_actions;

using namespace Lua;

static std::vector<uint16_t> iterateTable(lua_State* L, int index)
{
	std::vector<uint16_t> ids;
	lua_pushvalue(L, index);
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		const auto value = getNumber<uint16_t>(L, -1);
		const auto key = getNumber<uint16_t>(L, -2);
		ids.push_back(value);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	return ids;
}

static int luaCreateAction(lua_State* L)
{
	// Action(type, number)
	if (LuaScriptInterface::getScriptEnv()->getScriptInterface() != &g_scripts->getScriptInterface()) {
		reportErrorFunc(L, "Actions can only be registered in the Scripts interface.");
		lua_pushnil(L);
		return 1;
	}

	// classic revscriptsys with register for backwards compatibility
	if (lua_gettop(L) == 1) {
		Action* action = new Action(LuaScriptInterface::getScriptEnv()->getScriptInterface());
		if (action) {
			action->fromLua = true;
			pushUserdata(L, action);
			setMetatable(L, -1, "Action");
			return 1;
		} else {
			lua_pushnil(L);
			return 1;
		}
	}

	Action* action = new Action(LuaScriptInterface::getScriptEnv()->getScriptInterface());
	if (action) {
		std::string type = getString(L, 2);
		uint16_t id = getNumber<uint16_t>(L, 3);
		if (type == "id") {
			if (isTable(L, 3)) {
				for (auto id : iterateTable(L, 3)) {
					action->addItemId(id);
				}
				id = action->getItemIdRange().front();
			} else {
				action->addItemId(getNumber<uint16_t>(L, 3));
			}
		} else if (type == "uid") {
			if (isTable(L, 3)) {
				for (auto id : iterateTable(L, 3)) {
					action->addUniqueId(id);
				}
				id = action->getUniqueIdRange().front();
			} else {
				action->addUniqueId(getNumber<uint16_t>(L, 3));
			}
		} else if (type == "aid") {
			if (isTable(L, 3)) {
				for (auto id : iterateTable(L, 3)) {
					action->addActionId(id);
				}
				id = action->getActionIdRange().front();
			} else {
				action->addActionId(getNumber<uint16_t>(L, 3));
			}
		}

		action->fromLua = true;
		g_actions->registerLuaEvent(action);
		Action* ref = g_actions->getActionEvent(type, id);
		action->clearActionIdRange();
		action->clearItemIdRange();
		action->clearUniqueIdRange();
		pushUserdata(L, ref);
		setMetatable(L, -1, "Action");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaActionOnUse(lua_State* L)
{
	// action:onUse(callback)
	Action* action = getUserdata<Action>(L, 1);
	if (action) {
		if (!action->loadCallback()) {
			pushBoolean(L, false);
			return 1;
		}
		action->scripted = true;
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaActionNewIndex(lua_State* L)
{
	// action__newindex(callback)
	Action* action = getUserdata<Action>(L, 1);
	std::string test = getString(L, 2);
	if (action) {
		if (!action->loadCallback(test)) {
			pushBoolean(L, false);
			return 1;
		}
		action->scripted = true;
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaActionRegister(lua_State* L)
{
	// action:register()
	Action* action = getUserdata<Action>(L, 1);
	if (action) {
		if (!action->isScripted()) {
			pushBoolean(L, false);
			return 1;
		}
		pushBoolean(L, g_actions->registerLuaEvent(action));
		action->clearActionIdRange();
		action->clearItemIdRange();
		action->clearUniqueIdRange();
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaActionItemId(lua_State* L)
{
	// action:id(ids)
	Action* action = getUserdata<Action>(L, 1);
	if (action) {
		int parameters = lua_gettop(L) - 1; // - 1 because self is a parameter aswell, which we want to skip ofc
		if (parameters > 1) {
			for (int i = 0; i < parameters; ++i) {
				action->addItemId(getNumber<uint32_t>(L, 2 + i));
			}
		} else {
			action->addItemId(getNumber<uint32_t>(L, 2));
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaActionActionId(lua_State* L)
{
	// action:aid(aids)
	Action* action = getUserdata<Action>(L, 1);
	if (action) {
		int parameters = lua_gettop(L) - 1; // - 1 because self is a parameter aswell, which we want to skip ofc
		if (parameters > 1) {
			for (int i = 0; i < parameters; ++i) {
				action->addActionId(getNumber<uint32_t>(L, 2 + i));
			}
		} else {
			action->addActionId(getNumber<uint32_t>(L, 2));
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaActionUniqueId(lua_State* L)
{
	// action:uid(uids)
	Action* action = getUserdata<Action>(L, 1);
	if (action) {
		int parameters = lua_gettop(L) - 1; // - 1 because self is a parameter aswell, which we want to skip ofc
		if (parameters > 1) {
			for (int i = 0; i < parameters; ++i) {
				action->addUniqueId(getNumber<uint32_t>(L, 2 + i));
			}
		} else {
			action->addUniqueId(getNumber<uint32_t>(L, 2));
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaActionAllowFarUse(lua_State* L)
{
	// action:allowFarUse(bool)
	Action* action = getUserdata<Action>(L, 1);
	if (action) {
		action->setAllowFarUse(getBoolean(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaActionBlockWalls(lua_State* L)
{
	// action:blockWalls(bool)
	Action* action = getUserdata<Action>(L, 1);
	if (action) {
		action->setCheckLineOfSight(getBoolean(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaActionCheckFloor(lua_State* L)
{
	// action:checkFloor(bool)
	Action* action = getUserdata<Action>(L, 1);
	if (action) {
		action->setCheckFloor(getBoolean(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

namespace LuaAction {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("Action", "", luaCreateAction);
	interface->registerMetaMethod("Action", "__newindex", luaActionNewIndex);
	interface->registerMethod("Action", "onUse", luaActionOnUse);
	interface->registerMethod("Action", "register", luaActionRegister);
	interface->registerMethod("Action", "id", luaActionItemId);
	interface->registerMethod("Action", "aid", luaActionActionId);
	interface->registerMethod("Action", "uid", luaActionUniqueId);
	interface->registerMethod("Action", "allowFarUse", luaActionAllowFarUse);
	interface->registerMethod("Action", "blockWalls", luaActionBlockWalls);
	interface->registerMethod("Action", "checkFloor", luaActionCheckFloor);
}
} // namespace LuaAction

// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "actions.h"
#include "luascript.h"
#include "script.h"

extern Scripts* g_scripts;
extern Actions* g_actions;

using namespace Lua;

static int luaCreateAction(lua_State* L)
{
	// Action({id=1, uid={5,1,2}, aid=7, allowFarUse=true, blockWalls=true, checkFloor=true}) <-- example
	if (LuaScriptInterface::getScriptEnv()->getScriptInterface() != &g_scripts->getScriptInterface()) {
		reportErrorFunc(L, "Actions can only be registered in the Scripts interface.");
		lua_pushnil(L);
		return 1;
	}

	auto action = std::make_shared<Action>(LuaScriptInterface::getScriptEnv()->getScriptInterface());
	if (action) {
		action->fromLua = true;
		pushSharedPtr<Action_shared_ptr>(L, action);
		setMetatable(L, -1, "Action");
	} else {
		lua_pushnil(L);
	}

	return 1;
}

static int luaActionOnUse(lua_State* L)
{
	// action:onUse(callback)
	Action_shared_ptr action = getSharedPtr<Action>(L, 1);
	if (action) {
		const std::string& functionName = getString(L, 2);
		bool fileName = functionName == "onUse" ? true : false;
		if (!action->loadCallback(functionName, fileName)) {
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
	Action_shared_ptr action = getSharedPtr<Action>(L, 1);
	if (action) {
		if (!action->isScripted()) {
			pushBoolean(L, false);
			return 1;
		}
		pushBoolean(L, g_actions->registerLuaEvent(action));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaActionItemId(lua_State* L)
{
	// action:id(ids)
	Action_shared_ptr action = getSharedPtr<Action>(L, 1);
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
	Action_shared_ptr action = getSharedPtr<Action>(L, 1);
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
	Action_shared_ptr action = getSharedPtr<Action>(L, 1);
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
	Action_shared_ptr action = getSharedPtr<Action>(L, 1);
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
	Action_shared_ptr action = getSharedPtr<Action>(L, 1);
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
	Action_shared_ptr action = getSharedPtr<Action>(L, 1);
	if (action) {
		action->setCheckFloor(getBoolean(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaActionDelete(lua_State* L)
{
	Action_shared_ptr& action = getSharedPtr<Action>(L, 1);
	if (action) {
		action.reset();
	}
	return 0;
}

namespace LuaAction {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("Action", "", luaCreateAction);
	interface->registerMetaMethod("Action", "__gc", luaActionDelete);
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

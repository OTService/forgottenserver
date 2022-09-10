// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "luascript.h"
#include "script.h"
#include "talkaction.h"

extern Scripts* g_scripts;
extern TalkActions* g_talkActions;

using namespace Lua;

static int luaCreateTalkaction(lua_State* L)
{
	// TalkAction(words)
	if (LuaScriptInterface::getScriptEnv()->getScriptInterface() != &g_scripts->getScriptInterface()) {
		reportErrorFunc(L, "TalkActions can only be registered in the Scripts interface.");
		lua_pushnil(L);
		return 1;
	}

	TalkAction* raw = new TalkAction(LuaScriptInterface::getScriptEnv()->getScriptInterface());
	TalkAction_shared_ptr talk(raw);
	if (talk) {
		// classic revscriptsys registering
		if (isString(L, 2)) {
			for (int i = 2; i <= lua_gettop(L); i++) {
				talk->setWords(getString(L, i));
			}
		}
		talk->fromLua = true;
		pushSharedPtr<TalkAction_shared_ptr>(L, talk);
		setMetatable(L, -1, "TalkAction");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTalkactionWord(lua_State* L)
{
	// talkAction:word(word)
	TalkAction_shared_ptr talk = getSharedPtr<TalkAction>(L, 1);
	if (talk) {
		for (int i = 2; i <= lua_gettop(L); i++) {
			talk->setWords(getString(L, i));
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTalkactionOnSay(lua_State* L)
{
	// talkAction:onSay(callback)
	TalkAction_shared_ptr talk = getSharedPtr<TalkAction>(L, 1);
	if (talk) {
		const std::string& functionName = getString(L, 2);
		bool fileName = functionName == "onSay" ? true : false;
		if (!talk->loadCallback(functionName, fileName)) {
			pushBoolean(L, false);
			return 1;
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTalkactionRegister(lua_State* L)
{
	// talkAction:register()
	TalkAction_shared_ptr talk = getSharedPtr<TalkAction>(L, 1);
	if (talk) {
		if (!talk->isScripted()) {
			pushBoolean(L, false);
			return 1;
		}
		pushBoolean(L, g_talkActions->registerLuaEvent(talk));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTalkactionSeparator(lua_State* L)
{
	// talkAction:separator(sep)
	TalkAction_shared_ptr talk = getSharedPtr<TalkAction>(L, 1);
	if (talk) {
		talk->setSeparator(getString(L, 2).c_str());
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTalkactionAccess(lua_State* L)
{
	// talkAction:access(needAccess = false)
	TalkAction_shared_ptr talk = getSharedPtr<TalkAction>(L, 1);
	if (talk) {
		talk->setNeedAccess(getBoolean(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTalkactionAccountType(lua_State* L)
{
	// talkAction:accountType(AccountType_t = ACCOUNT_TYPE_NORMAL)
	TalkAction_shared_ptr talk = getSharedPtr<TalkAction>(L, 1);
	if (talk) {
		talk->setRequiredAccountType(getNumber<AccountType_t>(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaTalkActionDelete(lua_State* L)
{
	TalkAction_shared_ptr& talk = getSharedPtr<TalkAction>(L, 1);
	if (talk) {
		talk.reset();
	}
	return 0;
}

namespace LuaTalkAction {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("TalkAction", "", luaCreateTalkaction);
	interface->registerMetaMethod("TalkAction", "__gc", luaTalkActionDelete);
	interface->registerMethod("TalkAction", "onSay", luaTalkactionOnSay);
	interface->registerMethod("TalkAction", "word", luaTalkactionWord);
	interface->registerMethod("TalkAction", "register", luaTalkactionRegister);
	interface->registerMethod("TalkAction", "separator", luaTalkactionSeparator);
	interface->registerMethod("TalkAction", "access", luaTalkactionAccess);
	interface->registerMethod("TalkAction", "accountType", luaTalkactionAccountType);
}
} // namespace LuaTalkAction

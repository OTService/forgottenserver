// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "luascript.h"
#include "movement.h"
#include "script.h"

extern Scripts* g_scripts;
extern MoveEvents* g_moveEvents;

using namespace Lua;

static int luaCreateMoveEvent(lua_State* L)
{
	// MoveEvent()
	if (LuaScriptInterface::getScriptEnv()->getScriptInterface() != &g_scripts->getScriptInterface()) {
		reportErrorFunc(L, "MoveEvents can only be registered in the Scripts interface.");
		lua_pushnil(L);
		return 1;
	}

	MoveEvent* raw = new MoveEvent(LuaScriptInterface::getScriptEnv()->getScriptInterface());
	MoveEvent_shared_ptr moveevent(raw);
	if (moveevent) {
		moveevent->fromLua = true;
		pushSharedPtr<MoveEvent_shared_ptr>(L, moveevent);
		setMetatable(L, -1, "MoveEvent");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventType(lua_State* L)
{
	// moveevent:type(callback) // moveevent:event(callback)
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (moveevent) {
		const std::string& tmpStr = boost::algorithm::to_lower_copy(getString(L, 2));
		if (tmpStr == "stepin") {
			moveevent->setEventType(MOVE_EVENT_STEP_IN);
			moveevent->stepFunction = moveevent->StepInField;
		} else if (tmpStr == "stepout") {
			moveevent->setEventType(MOVE_EVENT_STEP_OUT);
			moveevent->stepFunction = moveevent->StepOutField;
		} else if (tmpStr == "equip") {
			moveevent->setEventType(MOVE_EVENT_EQUIP);
			moveevent->equipFunction = moveevent->EquipItem;
		} else if (tmpStr == "deequip") {
			moveevent->setEventType(MOVE_EVENT_DEEQUIP);
			moveevent->equipFunction = moveevent->DeEquipItem;
		} else if (tmpStr == "additem") {
			moveevent->setEventType(MOVE_EVENT_ADD_ITEM);
			moveevent->moveFunction = moveevent->AddItemField;
		} else if (tmpStr == "removeitem") {
			moveevent->setEventType(MOVE_EVENT_REMOVE_ITEM);
			moveevent->moveFunction = moveevent->RemoveItemField;
		} else {
			std::cout << "Error: [MoveEvent::configureMoveEvent] No valid event name " << tmpStr << std::endl;
			pushBoolean(L, false);
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventRegister(lua_State* L)
{
	// moveevent:register()
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (moveevent) {
		if ((moveevent->getEventType() == MOVE_EVENT_EQUIP || moveevent->getEventType() == MOVE_EVENT_DEEQUIP) &&
		    moveevent->getSlot() == SLOTP_WHEREEVER) {
			uint32_t id = moveevent->getItemIdRange().at(0);
			ItemType& it = Item::items.getItemType(id);
			moveevent->setSlot(it.slotPosition);
		}
		if (!moveevent->isScripted()) {
			pushBoolean(L, g_moveEvents->registerLuaFunction(moveevent));
			return 1;
		}
		pushBoolean(L, g_moveEvents->registerLuaEvent(moveevent));
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventOnCallback(lua_State* L)
{
	// moveevent:onEquip / deEquip / etc. (callback)
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (moveevent) {
		const std::string& functionName = getString(L, 2);
		bool fileName = false;
		const static std::vector<std::string> tmp = {"onEquip",   "onDeequip", "onStepIn",
		                                             "onStepOut", "onAddItem", "onRemoveItem"};
		for (auto& it : tmp) {
			if (it == functionName) {
				fileName = true;
				break;
			}
		}
		if (!moveevent->loadCallback(functionName, fileName)) {
			pushBoolean(L, false);
			return 1;
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventSlot(lua_State* L)
{
	// moveevent:slot(slot)
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (!moveevent) {
		lua_pushnil(L);
		return 1;
	}

	if (moveevent->getEventType() == MOVE_EVENT_EQUIP || moveevent->getEventType() == MOVE_EVENT_DEEQUIP) {
		const std::string& slotName = boost::algorithm::to_lower_copy(getString(L, 2));
		if (slotName == "head") {
			moveevent->setSlot(SLOTP_HEAD);
		} else if (slotName == "necklace") {
			moveevent->setSlot(SLOTP_NECKLACE);
		} else if (slotName == "backpack") {
			moveevent->setSlot(SLOTP_BACKPACK);
		} else if (slotName == "armor" || slotName == "body") {
			moveevent->setSlot(SLOTP_ARMOR);
		} else if (slotName == "right-hand") {
			moveevent->setSlot(SLOTP_RIGHT);
		} else if (slotName == "left-hand") {
			moveevent->setSlot(SLOTP_LEFT);
		} else if (slotName == "hand" || slotName == "shield") {
			moveevent->setSlot(SLOTP_RIGHT | SLOTP_LEFT);
		} else if (slotName == "legs") {
			moveevent->setSlot(SLOTP_LEGS);
		} else if (slotName == "feet") {
			moveevent->setSlot(SLOTP_FEET);
		} else if (slotName == "ring") {
			moveevent->setSlot(SLOTP_RING);
		} else if (slotName == "ammo") {
			moveevent->setSlot(SLOTP_AMMO);
		} else {
			std::cout << "[Warning - MoveEvent::configureMoveEvent] Unknown slot type: " << slotName << std::endl;
			pushBoolean(L, false);
			return 1;
		}
	}

	pushBoolean(L, true);
	return 1;
}

static int luaMoveEventLevel(lua_State* L)
{
	// moveevent:level(lvl)
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (moveevent) {
		moveevent->setRequiredLevel(getNumber<uint32_t>(L, 2));
		moveevent->setWieldInfo(WIELDINFO_LEVEL);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventMagLevel(lua_State* L)
{
	// moveevent:magicLevel(lvl)
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (moveevent) {
		moveevent->setRequiredMagLevel(getNumber<uint32_t>(L, 2));
		moveevent->setWieldInfo(WIELDINFO_MAGLV);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventPremium(lua_State* L)
{
	// moveevent:premium(bool)
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (moveevent) {
		moveevent->setNeedPremium(getBoolean(L, 2));
		moveevent->setWieldInfo(WIELDINFO_PREMIUM);
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventVocation(lua_State* L)
{
	// moveevent:vocation(vocName[, showInDescription = false, lastVoc = false])
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (moveevent) {
		moveevent->addVocEquipMap(getString(L, 2));
		moveevent->setWieldInfo(WIELDINFO_VOCREQ);
		std::string tmp;
		bool showInDescription = false;
		bool lastVoc = false;
		if (getBoolean(L, 3)) {
			showInDescription = getBoolean(L, 3);
		}
		if (getBoolean(L, 4)) {
			lastVoc = getBoolean(L, 4);
		}
		if (showInDescription) {
			if (moveevent->getVocationString().empty()) {
				tmp = boost::algorithm::to_lower_copy(getString(L, 2));
				tmp += "s";
				moveevent->setVocationString(tmp);
			} else {
				tmp = moveevent->getVocationString();
				if (lastVoc) {
					tmp += " and ";
				} else {
					tmp += ", ";
				}
				tmp += boost::algorithm::to_lower_copy(getString(L, 2));
				tmp += "s";
				moveevent->setVocationString(tmp);
			}
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventTileItem(lua_State* L)
{
	// moveevent:tileItem(bool)
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (moveevent) {
		moveevent->setTileItem(getBoolean(L, 2));
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventItemId(lua_State* L)
{
	// moveevent:id(ids)
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (moveevent) {
		int parameters = lua_gettop(L) - 1; // - 1 because self is a parameter aswell, which we want to skip ofc
		if (parameters > 1) {
			for (int i = 0; i < parameters; ++i) {
				moveevent->addItemId(getNumber<uint32_t>(L, 2 + i));
			}
		} else {
			moveevent->addItemId(getNumber<uint32_t>(L, 2));
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventActionId(lua_State* L)
{
	// moveevent:aid(ids)
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (moveevent) {
		int parameters = lua_gettop(L) - 1; // - 1 because self is a parameter aswell, which we want to skip ofc
		if (parameters > 1) {
			for (int i = 0; i < parameters; ++i) {
				moveevent->addActionId(getNumber<uint32_t>(L, 2 + i));
			}
		} else {
			moveevent->addActionId(getNumber<uint32_t>(L, 2));
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventUniqueId(lua_State* L)
{
	// moveevent:uid(ids)
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (moveevent) {
		int parameters = lua_gettop(L) - 1; // - 1 because self is a parameter aswell, which we want to skip ofc
		if (parameters > 1) {
			for (int i = 0; i < parameters; ++i) {
				moveevent->addUniqueId(getNumber<uint32_t>(L, 2 + i));
			}
		} else {
			moveevent->addUniqueId(getNumber<uint32_t>(L, 2));
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventPosition(lua_State* L)
{
	// moveevent:position(positions)
	MoveEvent_shared_ptr moveevent = getSharedPtr<MoveEvent>(L, 1);
	if (moveevent) {
		int parameters = lua_gettop(L) - 1; // - 1 because self is a parameter aswell, which we want to skip ofc
		if (parameters > 1) {
			for (int i = 0; i < parameters; ++i) {
				moveevent->addPosList(getPosition(L, 2 + i));
			}
		} else {
			moveevent->addPosList(getPosition(L, 2));
		}
		pushBoolean(L, true);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaMoveEventDelete(lua_State* L)
{
	MoveEvent_shared_ptr& moveEvent = getSharedPtr<MoveEvent>(L, 1);
	if (moveEvent) {
		moveEvent.reset();
	}
	return 0;
}

namespace LuaMoveEvent {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("MoveEvent", "", luaCreateMoveEvent);
	interface->registerMetaMethod("MoveEvent", "__gc", luaMoveEventDelete);
	interface->registerMethod("MoveEvent", "type", luaMoveEventType);
	interface->registerMethod("MoveEvent", "event", luaMoveEventType);
	interface->registerMethod("MoveEvent", "register", luaMoveEventRegister);
	interface->registerMethod("MoveEvent", "level", luaMoveEventLevel);
	interface->registerMethod("MoveEvent", "magicLevel", luaMoveEventMagLevel);
	interface->registerMethod("MoveEvent", "slot", luaMoveEventSlot);
	interface->registerMethod("MoveEvent", "id", luaMoveEventItemId);
	interface->registerMethod("MoveEvent", "aid", luaMoveEventActionId);
	interface->registerMethod("MoveEvent", "uid", luaMoveEventUniqueId);
	interface->registerMethod("MoveEvent", "position", luaMoveEventPosition);
	interface->registerMethod("MoveEvent", "premium", luaMoveEventPremium);
	interface->registerMethod("MoveEvent", "vocation", luaMoveEventVocation);
	interface->registerMethod("MoveEvent", "tileItem", luaMoveEventTileItem);
	interface->registerMethod("MoveEvent", "onEquip", luaMoveEventOnCallback);
	interface->registerMethod("MoveEvent", "onDeEquip", luaMoveEventOnCallback);
	interface->registerMethod("MoveEvent", "onStepIn", luaMoveEventOnCallback);
	interface->registerMethod("MoveEvent", "onStepOut", luaMoveEventOnCallback);
	interface->registerMethod("MoveEvent", "onAddItem", luaMoveEventOnCallback);
	interface->registerMethod("MoveEvent", "onRemoveItem", luaMoveEventOnCallback);
}
} // namespace LuaMoveEvent

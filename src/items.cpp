// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "items.h"

#include "movement.h"
#include "pugicast.h"
#include "script.h"
#include "weapons.h"

extern MoveEvents* g_moveEvents;
extern Weapons* g_weapons;
extern Scripts* g_scripts;

Items::Items()
{
	items.reserve(45000);
	nameToItems.reserve(45000);
}

void Items::clear()
{
	items.clear();
	clientIdToServerIdMap.clear();
	nameToItems.clear();
	currencyItems.clear();
	inventory.clear();
}

bool Items::reload()
{
	clear();

	g_scripts->loadScripts("items/serverid", false, true);
	items.shrink_to_fit();
	buildInventoryList();
	g_moveEvents->reload();
	g_weapons->reload();
	g_weapons->loadDefaults();
	return true;
}

void Items::buildInventoryList()
{
	inventory.reserve(items.size());
	for (const auto& type : items) {
		if (type.weaponType != WEAPON_NONE || type.ammoType != AMMO_NONE || type.attack != 0 || type.defense != 0 ||
		    type.extraDefense != 0 || type.armor != 0 || type.slotPosition & SLOTP_NECKLACE ||
		    type.slotPosition & SLOTP_RING || type.slotPosition & SLOTP_AMMO || type.slotPosition & SLOTP_FEET ||
		    type.slotPosition & SLOTP_HEAD || type.slotPosition & SLOTP_ARMOR || type.slotPosition & SLOTP_LEGS) {
			inventory.push_back(type.clientId);
		}
	}
	inventory.shrink_to_fit();
	std::sort(inventory.begin(), inventory.end());
}

void Items::shrinkItemVector() { items.shrink_to_fit(); }

ItemType& Items::parseItemLua(uint16_t id, uint16_t clientid)
{
	clientIdToServerIdMap.emplace(clientid, id);
	if (id >= items.size()) {
		items.resize(id + 1);
	}

	ItemType& iType = items[id];
	iType.id = id;
	iType.clientId = clientid;

	return iType;
}

void Items::parseItemName(const std::string& name, uint16_t id)
{
	std::string lowerCaseName = boost::algorithm::to_lower_copy(name);
	if (nameToItems.find(lowerCaseName) == nameToItems.end()) {
		nameToItems.emplace(std::move(lowerCaseName), id);
	}
}

void Items::parseItemWorth(uint64_t worth, uint16_t id)
{
	if (currencyItems.find(worth) != currencyItems.end()) {
		std::cout << "[Warning - parseItemWorth] Duplicated currency worth. Item " << id << " redefines worth " << worth
		          << std::endl;
	} else {
		currencyItems.insert(CurrencyMap::value_type(worth, id));
	}
}

ItemType& Items::getItemType(size_t id)
{
	if (id < items.size()) {
		return items[id];
	}
	return items.front();
}

const ItemType& Items::getItemType(size_t id) const
{
	if (id < items.size()) {
		return items[id];
	}
	return items.front();
}

const ItemType& Items::getItemIdByClientId(uint16_t spriteId) const
{
	if (spriteId >= 100) {
		if (uint16_t serverId = clientIdToServerIdMap.getServerId(spriteId)) {
			return getItemType(serverId);
		}
	}
	return items.front();
}

uint16_t Items::getItemIdByName(const std::string& name)
{
	if (name.empty()) {
		return 0;
	}

	auto result = nameToItems.find(boost::algorithm::to_lower_copy(name));
	if (result == nameToItems.end()) return 0;

	return result->second;
}

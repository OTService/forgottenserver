local reloadTypes = {
	["config"] = RELOAD_TYPE_CONFIG,
	["configuration"] = RELOAD_TYPE_CONFIG,

	["events"] = RELOAD_TYPE_EVENTS,

	["global"] = RELOAD_TYPE_LIBRARY,

	["monster"] = RELOAD_TYPE_MONSTERS,
	["monsters"] = RELOAD_TYPE_MONSTERS,

	["mount"] = RELOAD_TYPE_MOUNTS,
	["mounts"] = RELOAD_TYPE_MOUNTS,

	["npc"] = RELOAD_TYPE_NPCS,
	["npcs"] = RELOAD_TYPE_NPCS,

	["quest"] = RELOAD_TYPE_QUESTS,
	["quests"] = RELOAD_TYPE_QUESTS,

	["raid"] = RELOAD_TYPE_RAIDS,
	["raids"] = RELOAD_TYPE_RAIDS,

	["scripts"] = RELOAD_TYPE_SCRIPTS,
	["lib"] = RELOAD_TYPE_LIBRARY,
	["libs"] = RELOAD_TYPE_LIBRARY
}

local talk = TalkAction({word={"/reload", "!reload"}, separator=" ", access=5, accountType=ACCOUNT_TYPE_GOD})

function talk.onReload(player, words, param)
	logCommand(player, words, param)

	local reload = param:lower():split(",")
	if reload[1] == "item" then
		if ItemType(tonumber(reload[2]:trim())) then
			dofile("data/items/serverid/".. reload[2]:trim() ..".lua")
			player:sendTextMessage(MESSAGE_INFO_DESCR, string.format("Reloaded item with id: %s.", reload[2]:trim()))
		else
			player:sendTextMessage(MESSAGE_INFO_DESCR, string.format("Item with id: %s does not exist.", reload[2]:trim()))
		end
		return false
	end
	
	local reloadType = reloadTypes[param:lower()]
	if not reloadType then
		player:sendTextMessage(MESSAGE_INFO_DESCR, "Reload type not found.")
		return false
	end

	-- need to clear EventCallback.data or we end up having duplicated events on /reload scripts
	if table.contains({RELOAD_TYPE_SCRIPTS, RELOAD_TYPE_ALL}, reloadType) then
		EventCallback:clear()
	end

	Game.reload(reloadType)
	player:sendTextMessage(MESSAGE_INFO_DESCR, string.format("Reloaded %s.", param:lower()))
	return false
end

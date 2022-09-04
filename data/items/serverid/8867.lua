-- name: dragon robe
Game.createItemType(8867, 8039):register({
	weight = 2850,
	description = "It is covered with runic symbols.",
	minReqLevel = 75,
	classification = 2,
	requiredLevel = 75,
	walkStack = true,
	vocationString = "sorcerers",
	clientId = 8039,
	abilities = {
		absorbPercent = {
			absorbpercentfire = 12,
			absorbpercentice = -12,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8039,
	id = 8867,
	article = "a",
	name = "dragon robe",
	armor = 12,
	wieldInfo = 5,
	slotPosition = "body",
})

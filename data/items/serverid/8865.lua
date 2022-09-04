-- name: dark lord's cape
Game.createItemType(8865, 8037):register({
	weight = 2900,
	description = "There is a dark aura surrounding it.",
	light = {
		level = 2,
		color = 37,
	},
	minReqLevel = 65,
	classification = 4,
	requiredLevel = 65,
	walkStack = true,
	vocationString = "sorcerers",
	clientId = 8037,
	abilities = {
		absorbPercent = {
			fieldabsorbpercentdeath = 8,
			absorbpercentholy = -8,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8037,
	id = 8865,
	article = "a",
	name = "dark lord's cape",
	armor = 11,
	wieldInfo = 5,
	slotPosition = "body",
})

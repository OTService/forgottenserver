-- name: robe of the ice queen
Game.createItemType(8866, 8038):register({
	weight = 2950,
	description = "The soft tissue shimmers.",
	light = {
		level = 2,
		color = 71,
	},
	minReqLevel = 75,
	classification = 4,
	requiredLevel = 75,
	walkStack = true,
	vocationString = "druids",
	clientId = 8038,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = -12,
			absorbpercentice = 12,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8038,
	id = 8866,
	article = "a",
	name = "robe of the ice queen",
	armor = 12,
	wieldInfo = 5,
	slotPosition = "body",
})

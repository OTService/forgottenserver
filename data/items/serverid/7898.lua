-- name: lightning robe
Game.createItemType(7898, 825):register({
	weight = 2250,
	minReqLevel = 50,
	classification = 2,
	requiredLevel = 50,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 825,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 8,
			absorbpercentearth = -8,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 825,
	id = 7898,
	article = "a",
	name = "lightning robe",
	armor = 11,
	wieldInfo = 5,
	slotPosition = "body",
})

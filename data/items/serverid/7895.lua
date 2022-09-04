-- name: lightning legs
Game.createItemType(7895, 822):register({
	weight = 1900,
	minReqLevel = 40,
	requiredLevel = 40,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 822,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 6,
			absorbpercentearth = -6,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 822,
	id = 7895,
	name = "lightning legs",
	armor = 8,
	wieldInfo = 5,
	slotPosition = "legs",
})

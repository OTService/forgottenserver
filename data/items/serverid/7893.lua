-- name: lightning boots
Game.createItemType(7893, 820):register({
	weight = 750,
	minReqLevel = 35,
	requiredLevel = 35,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 820,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 5,
			absorbpercentearth = -5,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 820,
	id = 7893,
	name = "lightning boots",
	armor = 2,
	wieldInfo = 5,
	slotPosition = "feet",
})

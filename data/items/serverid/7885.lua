-- name: terra legs
Game.createItemType(7885, 812):register({
	weight = 1900,
	minReqLevel = 40,
	requiredLevel = 40,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 812,
	abilities = {
		absorbPercent = {
			absorbpercentfire = -6,
			absorbpercentearth = 6,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 812,
	id = 7885,
	name = "terra legs",
	armor = 8,
	wieldInfo = 5,
	slotPosition = "legs",
})

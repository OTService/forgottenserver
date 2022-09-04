-- name: gill legs
Game.createItemType(18400, 16106):register({
	weight = 2800,
	minReqLevel = 150,
	requiredLevel = 150,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 16106,
	abilities = {
		absorbPercent = {
			absorbpercentfire = -8,
			absorbpercentearth = 8,
		},
		stats = {
			magicpoints = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 16106,
	id = 18400,
	name = "gill legs",
	armor = 7,
	wieldInfo = 5,
	slotPosition = "legs",
})

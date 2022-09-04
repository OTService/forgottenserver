-- name: gill coat
Game.createItemType(18399, 16105):register({
	weight = 1900,
	minReqLevel = 150,
	classification = 2,
	requiredLevel = 150,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 16105,
	abilities = {
		absorbPercent = {
			absorbpercentfire = -10,
			absorbpercentearth = 10,
		},
		stats = {
			magicpoints = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 16105,
	id = 18399,
	article = "a",
	name = "gill coat",
	armor = 12,
	wieldInfo = 5,
	slotPosition = "body",
})

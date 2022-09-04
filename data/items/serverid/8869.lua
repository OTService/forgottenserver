-- name: greenwood coat
Game.createItemType(8869, 8041):register({
	weight = 2850,
	minReqLevel = 75,
	classification = 2,
	requiredLevel = 75,
	walkStack = true,
	vocationString = "druids",
	clientId = 8041,
	abilities = {
		absorbPercent = {
			absorbpercentfire = -12,
			absorbpercentearth = 12,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8041,
	id = 8869,
	article = "a",
	name = "greenwood coat",
	armor = 12,
	wieldInfo = 5,
	slotPosition = "body",
})

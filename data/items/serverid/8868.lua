-- name: velvet mantle
Game.createItemType(8868, 8040):register({
	weight = 2850,
	description = "The smooth tissue seems to sparkle.",
	minReqLevel = 75,
	classification = 2,
	requiredLevel = 75,
	walkStack = true,
	vocationString = "sorcerers",
	clientId = 8040,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 12,
			absorbpercentearth = -12,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8040,
	id = 8868,
	article = "a",
	name = "velvet mantle",
	armor = 12,
	wieldInfo = 5,
	slotPosition = "body",
})

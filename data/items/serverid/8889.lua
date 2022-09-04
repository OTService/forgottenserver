-- name: skullcracker armor
Game.createItemType(8889, 8061):register({
	weight = 11000,
	minReqLevel = 85,
	classification = 2,
	requiredLevel = 85,
	walkStack = true,
	vocationString = "knights",
	clientId = 8061,
	abilities = {
		absorbPercent = {
			fieldabsorbpercentdeath = 5,
			absorbpercentholy = -5,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8061,
	id = 8889,
	article = "a",
	name = "skullcracker armor",
	armor = 14,
	wieldInfo = 5,
	slotPosition = "body",
})

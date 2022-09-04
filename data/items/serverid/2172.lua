-- name: bronze amulet
Game.createItemType(2172, 3056):register({
	weight = 500,
	showAttributes = true,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 3056,
	abilities = {
		absorbPercent = {
			absorbpercentmanadrain = 20,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 3056,
	id = 2172,
	article = "a",
	name = "bronze amulet",
	slotPosition = "necklace",
	charges = 200,
})

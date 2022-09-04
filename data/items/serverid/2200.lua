-- name: protection amulet
Game.createItemType(2200, 3084):register({
	weight = 550,
	showAttributes = true,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 3084,
	abilities = {
		absorbPercent = {
			absorbpercentphysical = 6,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 3084,
	id = 2200,
	article = "a",
	name = "protection amulet",
	slotPosition = "necklace",
	charges = 250,
})

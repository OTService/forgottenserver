-- name: stone skin amulet
Game.createItemType(2197, 3081):register({
	weight = 700,
	showAttributes = true,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 3081,
	abilities = {
		absorbPercent = {
			fieldabsorbpercentdeath = 80,
			absorbpercentphysical = 80,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 3081,
	id = 2197,
	article = "a",
	name = "stone skin amulet",
	slotPosition = "necklace",
	charges = 5,
})

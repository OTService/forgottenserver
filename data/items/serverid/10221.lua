-- name: shockwave amulet
Game.createItemType(10221, 9304):register({
	weight = 700,
	isAnimation = true,
	description = "It tickles your neck in a rather pleasant way and absorbs loads of damage.",
	minReqLevel = 80,
	requiredLevel = 80,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 9304,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 40,
			absorbpercentphysical = 60,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 9304,
	id = 10221,
	article = "a",
	name = "shockwave amulet",
	wieldInfo = 1,
	slotPosition = "necklace",
	charges = 5,
})

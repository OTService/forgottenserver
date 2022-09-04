-- name: bonfire amulet
Game.createItemType(10218, 9301):register({
	weight = 700,
	showAttributes = true,
	description = "It engulfs you in flames - fire against fire.",
	minReqLevel = 80,
	requiredLevel = 80,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 9301,
	abilities = {
		absorbPercent = {
			absorbpercentfire = 40,
			absorbpercentphysical = 60,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 9301,
	id = 10218,
	article = "a",
	name = "bonfire amulet",
	wieldInfo = 1,
	slotPosition = "necklace",
	charges = 5,
})

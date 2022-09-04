-- name: lightning pendant
Game.createItemType(7889, 816):register({
	weight = 500,
	showAttributes = true,
	isAnimation = true,
	minReqLevel = 60,
	requiredLevel = 60,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 816,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 20,
			absorbpercentearth = -10,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 816,
	id = 7889,
	article = "a",
	name = "lightning pendant",
	wieldInfo = 1,
	slotPosition = "necklace",
	charges = 200,
})

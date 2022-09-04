-- name: sacred tree amulet
Game.createItemType(10219, 9302):register({
	weight = 700,
	showAttributes = true,
	description = "Its leaves stretch out to cover your whole body if you receive earth damage.",
	minReqLevel = 80,
	requiredLevel = 80,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 9302,
	abilities = {
		absorbPercent = {
			absorbpercentearth = 40,
			absorbpercentphysical = 60,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 9302,
	id = 10219,
	article = "a",
	name = "sacred tree amulet",
	wieldInfo = 1,
	slotPosition = "necklace",
	charges = 5,
})

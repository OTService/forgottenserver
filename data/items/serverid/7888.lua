-- name: glacier amulet
Game.createItemType(7888, 815):register({
	weight = 500,
	showAttributes = true,
	minReqLevel = 60,
	requiredLevel = 60,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 815,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = -10,
			absorbpercentice = 20,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 815,
	id = 7888,
	article = "a",
	name = "glacier amulet",
	wieldInfo = 1,
	slotPosition = "necklace",
	charges = 200,
})

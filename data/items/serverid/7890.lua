-- name: magma amulet
Game.createItemType(7890, 817):register({
	weight = 500,
	showAttributes = true,
	minReqLevel = 60,
	requiredLevel = 60,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 817,
	abilities = {
		absorbPercent = {
			absorbpercentfire = 20,
			absorbpercentice = -10,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 817,
	id = 7890,
	article = "a",
	name = "magma amulet",
	wieldInfo = 1,
	slotPosition = "necklace",
	charges = 200,
})

-- name: terra amulet
Game.createItemType(7887, 814):register({
	weight = 500,
	showAttributes = true,
	minReqLevel = 60,
	requiredLevel = 60,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 814,
	abilities = {
		absorbPercent = {
			absorbpercentfire = -10,
			absorbpercentearth = 20,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 814,
	id = 7887,
	article = "a",
	name = "terra amulet",
	wieldInfo = 1,
	slotPosition = "necklace",
	charges = 200,
})

-- name: voltage armor
Game.createItemType(8879, 8051):register({
	weight = 8450,
	minReqLevel = 60,
	classification = 2,
	requiredLevel = 60,
	walkStack = true,
	vocationString = "knights and paladins",
	clientId = 8051,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 3,
			absorbpercentearth = -3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8051,
	id = 8879,
	article = "a",
	name = "voltage armor",
	armor = 13,
	wieldInfo = 5,
	slotPosition = "body",
})

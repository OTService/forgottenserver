-- name: crystal boots
Game.createItemType(11117, 10200):register({
	weight = 1850,
	minReqLevel = 70,
	requiredLevel = 70,
	walkStack = true,
	vocationString = "knights and paladins",
	clientId = 10200,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = -3,
			absorbpercentice = 3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 10200,
	id = 11117,
	name = "crystal boots",
	armor = 3,
	wieldInfo = 5,
	slotPosition = "feet",
})

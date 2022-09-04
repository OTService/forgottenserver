-- name: guardian boots
Game.createItemType(11240, 10323):register({
	weight = 2250,
	minReqLevel = 70,
	requiredLevel = 70,
	walkStack = true,
	vocationString = "knights and paladins",
	clientId = 10323,
	abilities = {
		absorbPercent = {
			absorbpercentholy = -2,
			absorbpercentphysical = 2,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 10323,
	id = 11240,
	name = "guardian boots",
	armor = 3,
	wieldInfo = 5,
	slotPosition = "feet",
})

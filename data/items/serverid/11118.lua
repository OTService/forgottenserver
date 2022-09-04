-- name: dragon scale boots
Game.createItemType(11118, 10201):register({
	weight = 1850,
	minReqLevel = 70,
	requiredLevel = 70,
	walkStack = true,
	vocationString = "knights and paladins",
	clientId = 10201,
	abilities = {
		absorbPercent = {
			absorbpercentfire = 3,
			absorbpercentice = -3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 10201,
	id = 11118,
	name = "dragon scale boots",
	armor = 3,
	wieldInfo = 5,
	slotPosition = "feet",
})

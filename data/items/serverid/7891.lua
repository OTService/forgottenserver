-- name: magma boots
Game.createItemType(7891, 818):register({
	weight = 750,
	minReqLevel = 35,
	requiredLevel = 35,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 818,
	abilities = {
		absorbPercent = {
			absorbpercentfire = 5,
			absorbpercentice = -5,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 818,
	id = 7891,
	name = "magma boots",
	armor = 2,
	wieldInfo = 5,
	slotPosition = "feet",
})

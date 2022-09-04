-- name: terra boots
Game.createItemType(7886, 813):register({
	weight = 750,
	minReqLevel = 35,
	requiredLevel = 35,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 813,
	abilities = {
		absorbPercent = {
			absorbpercentfire = -5,
			absorbpercentearth = 5,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 813,
	id = 7886,
	name = "terra boots",
	armor = 2,
	wieldInfo = 5,
	slotPosition = "feet",
})

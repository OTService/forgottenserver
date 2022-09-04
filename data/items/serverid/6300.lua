-- name: death ring
Game.createItemType(6300, 6299):register({
	weight = 80,
	showDuration = true,
	walkStack = true,
	transformEquipId = 6301,
	stopDuration = true,
	clientId = 6299,
	abilities = {
		absorbPercent = {
			fieldabsorbpercentdeath = 5,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 6299,
	id = 6300,
	article = "a",
	name = "death ring",
	slotPosition = "ring",
})

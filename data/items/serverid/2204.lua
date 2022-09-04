-- name: energy ring
Game.createItemType(2204, 3088):register({
	weight = 80,
	showAttributes = true,
	transformDeEquipId = 2167,
	isAnimation = true,
	showDuration = true,
	walkStack = true,
	clientId = 3088,
	abilities = {
		manaShield = true,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 2204,
	article = "an",
	name = "energy ring",
	duration = 600,
	slotPosition = "ring",
})

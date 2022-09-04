-- name: life ring
Game.createItemType(2205, 3089):register({
	weight = 80,
	showAttributes = true,
	transformDeEquipId = 2168,
	isAnimation = true,
	showDuration = true,
	walkStack = true,
	clientId = 3089,
	abilities = {
		manaGain = 8,
		manaTicks = 6000,
		regeneration = true,
		healthGain = 2,
		healthTicks = 6000,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 2205,
	article = "a",
	name = "life ring",
	duration = 1200,
	slotPosition = "ring",
})

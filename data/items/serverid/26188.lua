-- name: ring of green plasma
Game.createItemType(26188, 23532):register({
	weight = 90,
	transformDeEquipId = 26187,
	isAnimation = true,
	minReqLevel = 100,
	showDuration = true,
	requiredLevel = 100,
	walkStack = true,
	clientId = 23532,
	abilities = {
		manaGain = 4,
		manaTicks = 6000,
		regeneration = true,
		healthGain = 1,
		stats = {
			magicpoints = 2,
		},
		healthTicks = 6000,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 26188,
	name = "ring of green plasma",
	duration = 1800,
	wieldInfo = 1,
	slotPosition = "ring",
})

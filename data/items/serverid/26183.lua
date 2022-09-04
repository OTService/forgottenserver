-- name: collar of green plasma
Game.createItemType(26183, 23527):register({
	weight = 500,
	transformDeEquipId = 26199,
	isAnimation = true,
	minReqLevel = 100,
	showDuration = true,
	requiredLevel = 100,
	walkStack = true,
	clientId = 23527,
	abilities = {
		manaGain = 8,
		manaTicks = 6000,
		regeneration = true,
		healthGain = 2,
		stats = {
			magicpoints = 3,
		},
		healthTicks = 6000,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 26183,
	article = "a",
	name = "collar of green plasma",
	duration = 1800,
	wieldInfo = 1,
	slotPosition = "necklace",
})

-- name: dwarven ring
Game.createItemType(2215, 3099):register({
	weight = 110,
	showAttributes = true,
	transformDeEquipId = 2213,
	isAnimation = true,
	showDuration = true,
	walkStack = true,
	clientId = 3099,
	abilities = {
		conditionSuppressions = 2048,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 2215,
	article = "a",
	name = "dwarven ring",
	duration = 3600,
	slotPosition = "ring",
})

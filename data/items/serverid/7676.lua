-- name: withered plant
Game.createItemType(7676, 327):register({
	weight = 150,
	description = "This plant urgently needs some water, else it will wither away and disappear completely.",
	walkStack = true,
	clientId = 327,
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 7676,
	article = "a",
	name = "withered plant",
	decayId = 7655,
	duration = 72000,
	slotPosition = "hand",
})

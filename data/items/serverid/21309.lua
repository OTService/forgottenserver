-- name: filled elven vial
Game.createItemType(21309, 18992):register({
	weight = 100,
	isAnimation = true,
	description = "The water in this vial is sacred to some. It will not stay in this vessel forever, however.",
	light = {
		level = 4,
		color = 143,
	},
	walkStack = true,
	clientId = 18992,
	moveable = true,
	pickupable = true,
	replaceable = true,
	useable = true,
	id = 21309,
	article = "a",
	name = "filled elven vial",
	slotPosition = "hand",
})

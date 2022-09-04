-- name: wand of dragonbreath
Game.createItemType(2191, 3075):register({
	weight = 2100,
	isAnimation = true,
	description = "Legends say that this wand holds the soul of a young dragon.",
	shootType = "fire",
	shootRange = 3,
	light = {
		level = 2,
		color = 192,
	},
	minReqLevel = 13,
	classification = 1,
	requiredLevel = 13,
	walkStack = true,
	vocationString = "sorcerers",
	clientId = 3075,
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "wand",
	wareId = 3075,
	id = 2191,
	article = "a",
	name = "wand of dragonbreath",
	wieldInfo = 5,
	slotPosition = "hand",
})

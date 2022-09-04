-- name: fiery mystic blade
Game.createItemType(7746, 662):register({
	weight = 3500,
	isAnimation = true,
	shootRange = 1,
	light = {
		level = 2,
		color = 206,
	},
	minReqLevel = 60,
	extraDefense = 2,
	requiredLevel = 60,
	showCharges = true,
	walkStack = true,
	elementDamage = "elementfire",
	subType = true,
	elementType = 8,
	clientId = 662,
	abilities = {
		elementDamage = 8,
		elementType = 8,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "sword",
	useable = true,
	defense = 25,
	id = 7746,
	article = "a",
	name = "fiery mystic blade",
	decayId = 7384,
	attack = 36,
	wieldInfo = 1,
	slotPosition = "hand",
	charges = 1000,
})
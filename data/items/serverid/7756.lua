-- name: fiery cranial basher
Game.createItemType(7756, 672):register({
	weight = 7800,
	isAnimation = true,
	shootRange = 1,
	light = {
		level = 2,
		color = 206,
	},
	minReqLevel = 60,
	extraDefense = -2,
	requiredLevel = 60,
	showCharges = true,
	walkStack = true,
	elementDamage = "elementfire",
	subType = true,
	elementType = 8,
	clientId = 672,
	abilities = {
		elementDamage = 8,
		elementType = 8,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "club",
	useable = true,
	defense = 20,
	id = 7756,
	article = "a",
	name = "fiery cranial basher",
	decayId = 7415,
	attack = 36,
	wieldInfo = 1,
	slotPosition = "hand",
	charges = 1000,
})
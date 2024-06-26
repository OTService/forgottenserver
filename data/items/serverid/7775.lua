-- name: icy cranial basher
Game.createItemType(7775, 691):register({
	weight = 7800,
	isAnimation = true,
	shootRange = 1,
	light = {
		level = 2,
		color = 59,
	},
	minReqLevel = 60,
	extraDefense = -2,
	requiredLevel = 60,
	showCharges = true,
	walkStack = true,
	elementDamage = "elementfire",
	subType = true,
	elementType = 512,
	clientId = 691,
	abilities = {
		elementDamage = 8,
		elementType = 512,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "club",
	useable = true,
	defense = 20,
	id = 7775,
	article = "an",
	name = "icy cranial basher",
	decayId = 7415,
	attack = 36,
	wieldInfo = 1,
	slotPosition = "hand",
	charges = 1000,
})

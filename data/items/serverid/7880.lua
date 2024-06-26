-- name: energy crystal mace
Game.createItemType(7880, 807):register({
	weight = 8000,
	isAnimation = true,
	shootRange = 1,
	light = {
		level = 2,
		color = 59,
	},
	minReqLevel = 35,
	extraDefense = 1,
	requiredLevel = 35,
	showCharges = true,
	walkStack = true,
	elementDamage = 7,
	subType = true,
	elementType = 2,
	clientId = 807,
	abilities = {
		elementDamage = 7,
		elementType = 2,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "club",
	useable = true,
	defense = 16,
	id = 7880,
	article = "an",
	name = "energy crystal mace",
	decayId = 2445,
	attack = 31,
	wieldInfo = 1,
	slotPosition = "hand",
	charges = 1000,
})

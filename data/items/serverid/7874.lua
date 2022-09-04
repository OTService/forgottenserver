-- name: energy barbarian axe
Game.createItemType(7874, 801):register({
	weight = 5100,
	isAnimation = true,
	shootRange = 1,
	light = {
		level = 2,
		color = 59,
	},
	minReqLevel = 20,
	extraDefense = 1,
	requiredLevel = 20,
	showCharges = true,
	walkStack = true,
	elementDamage = 5,
	subType = true,
	elementType = 2,
	clientId = 801,
	abilities = {
		elementDamage = 5,
		elementType = 2,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "axe",
	useable = true,
	defense = 18,
	id = 7874,
	article = "an",
	name = "energy barbarian axe",
	decayId = 2429,
	attack = 23,
	wieldInfo = 1,
	slotPosition = "hand",
	charges = 1000,
})

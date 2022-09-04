-- name: earth relic sword
Game.createItemType(7855, 780):register({
	weight = 4800,
	isAnimation = true,
	shootRange = 1,
	light = {
		level = 2,
		color = 206,
	},
	minReqLevel = 50,
	extraDefense = 1,
	requiredLevel = 50,
	showCharges = true,
	walkStack = true,
	elementDamage = "elementfire",
	subType = true,
	elementType = 4,
	clientId = 780,
	abilities = {
		elementDamage = 8,
		elementType = 4,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "sword",
	useable = true,
	defense = 24,
	id = 7855,
	article = "an",
	name = "earth relic sword",
	decayId = 7383,
	attack = 34,
	wieldInfo = 1,
	slotPosition = "hand",
	charges = 1000,
})
-- name: fiery relic sword
Game.createItemType(7745, 661):register({
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
	elementType = 8,
	clientId = 661,
	abilities = {
		elementDamage = 8,
		elementType = 8,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "sword",
	useable = true,
	defense = 24,
	id = 7745,
	article = "a",
	name = "fiery relic sword",
	decayId = 7383,
	attack = 34,
	wieldInfo = 1,
	slotPosition = "hand",
	charges = 1000,
})

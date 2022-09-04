-- name: earth spike sword
Game.createItemType(7854, 779):register({
	weight = 5000,
	isAnimation = true,
	shootRange = 1,
	light = {
		level = 2,
		color = 206,
	},
	extraDefense = 2,
	showCharges = true,
	walkStack = true,
	elementDamage = "elementearth",
	subType = true,
	elementType = 4,
	clientId = 779,
	abilities = {
		elementDamage = 4,
		elementType = 4,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "sword",
	useable = true,
	defense = 21,
	id = 7854,
	article = "an",
	name = "earth spike sword",
	decayId = 2383,
	attack = 20,
	slotPosition = "hand",
	charges = 1000,
})
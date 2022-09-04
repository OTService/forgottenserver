-- name: earth knight axe
Game.createItemType(7860, 785):register({
	weight = 5900,
	isAnimation = true,
	shootRange = 1,
	light = {
		level = 2,
		color = 206,
	},
	minReqLevel = 25,
	extraDefense = 1,
	requiredLevel = 25,
	showCharges = true,
	walkStack = true,
	elementDamage = 6,
	subType = true,
	elementType = 4,
	clientId = 785,
	abilities = {
		elementDamage = 6,
		elementType = 4,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "axe",
	useable = true,
	defense = 21,
	id = 7860,
	article = "an",
	name = "earth knight axe",
	decayId = 2430,
	attack = 27,
	wieldInfo = 1,
	slotPosition = "hand",
	charges = 1000,
})

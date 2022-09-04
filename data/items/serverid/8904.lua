-- name: spellscroll of prophecies
Game.createItemType(8904, 8076):register({
	weight = 2650,
	description = "It shows your spells and can also shield against attacks when worn.",
	shootRange = 1,
	minReqLevel = 70,
	requiredLevel = 70,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 8076,
	abilities = {
		stats = {
			magicpoints = 3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 12,
	wareId = 8076,
	id = 8904,
	article = "a",
	name = "spellscroll of prophecies",
	wieldInfo = 5,
	slotPosition = "hand",
})

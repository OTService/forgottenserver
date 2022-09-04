-- name: spellbook of dark mysteries
Game.createItemType(8918, 8090):register({
	weight = 2850,
	description = "It shows your spells and can also shield against attacks when worn.",
	shootRange = 1,
	minReqLevel = 80,
	requiredLevel = 80,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 8090,
	abilities = {
		stats = {
			magicpoints = 3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 16,
	wareId = 8090,
	id = 8918,
	article = "a",
	name = "spellbook of dark mysteries",
	wieldInfo = 5,
	slotPosition = "hand",
})

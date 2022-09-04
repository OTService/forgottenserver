-- name: spellbook of enlightenment
Game.createItemType(8900, 8072):register({
	weight = 1950,
	description = "It shows your spells and can also shield against attacks when worn.",
	shootRange = 1,
	minReqLevel = 30,
	requiredLevel = 30,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 8072,
	abilities = {
		stats = {
			magicpoints = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 18,
	wareId = 8072,
	id = 8900,
	article = "a",
	name = "spellbook of enlightenment",
	wieldInfo = 5,
	slotPosition = "hand",
})

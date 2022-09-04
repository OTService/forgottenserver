-- name: spellbook of lost souls
Game.createItemType(8903, 8075):register({
	weight = 2560,
	description = "It shows your spells and can also shield against attacks when worn.",
	shootRange = 1,
	minReqLevel = 60,
	requiredLevel = 60,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 8075,
	abilities = {
		stats = {
			magicpoints = 2,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 20,
	wareId = 8075,
	id = 8903,
	article = "a",
	name = "spellbook of lost souls",
	wieldInfo = 5,
	slotPosition = "hand",
})

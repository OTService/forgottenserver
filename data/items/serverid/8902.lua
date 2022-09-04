-- name: spellbook of mind control
Game.createItemType(8902, 8074):register({
	weight = 2450,
	description = "It shows your spells and can also shield against attacks when worn.",
	shootRange = 1,
	minReqLevel = 50,
	requiredLevel = 50,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 8074,
	abilities = {
		stats = {
			magicpoints = 2,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 16,
	wareId = 8074,
	id = 8902,
	article = "a",
	name = "spellbook of mind control",
	wieldInfo = 5,
	slotPosition = "hand",
})

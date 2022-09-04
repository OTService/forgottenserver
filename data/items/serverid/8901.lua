-- name: spellbook of warding
Game.createItemType(8901, 8073):register({
	weight = 2100,
	description = "It shows your spells and can also shield against attacks when worn.",
	shootRange = 1,
	minReqLevel = 40,
	requiredLevel = 40,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 8073,
	abilities = {
		stats = {
			magicpoints = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 22,
	wareId = 8073,
	id = 8901,
	article = "a",
	name = "spellbook of warding",
	wieldInfo = 5,
	slotPosition = "hand",
})

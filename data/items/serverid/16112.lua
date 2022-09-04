-- name: spellbook of ancient arcana
Game.createItemType(16112, 14769):register({
	weight = 2500,
	showAttributes = true,
	isAnimation = true,
	description = "It shows your spells and can also shield against attacks when worn.",
	shootRange = 1,
	minReqLevel = 150,
	requiredLevel = 150,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 14769,
	abilities = {
		absorbPercent = {
			fieldabsorbpercentdeath = 5,
		},
		stats = {
			magicpoints = 4,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 19,
	wareId = 14769,
	id = 16112,
	article = "a",
	name = "spellbook of ancient arcana",
	wieldInfo = 5,
	slotPosition = "hand",
})

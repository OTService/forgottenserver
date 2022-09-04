-- name: snake god's wristguard
Game.createItemType(12647, 11691):register({
	weight = 2800,
	description = "It shows your spells and can also shield against attacks when worn.",
	shootRange = 1,
	minReqLevel = 100,
	requiredLevel = 100,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 11691,
	abilities = {
		stats = {
			magicpoints = 3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 14,
	wareId = 11691,
	id = 12647,
	article = "a",
	name = "snake god's wristguard",
	wieldInfo = 5,
	slotPosition = "hand",
})

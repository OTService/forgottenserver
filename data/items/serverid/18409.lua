-- name: wand of everblazing
Game.createItemType(18409, 16115):register({
	weight = 3700,
	isAnimation = true,
	description = "It contains the everblazing fire.",
	shootType = "fire",
	shootRange = 4,
	light = {
		level = 3,
		color = 206,
	},
	minReqLevel = 65,
	classification = 2,
	requiredLevel = 65,
	walkStack = true,
	vocationString = "sorcerers",
	clientId = 16115,
	abilities = {
		stats = {
			magicpoints = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "wand",
	wareId = 16115,
	id = 18409,
	article = "a",
	name = "wand of everblazing",
	wieldInfo = 5,
	slotPosition = "hand",
})

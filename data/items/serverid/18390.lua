-- name: wand of defiance
Game.createItemType(18390, 16096):register({
	weight = 3700,
	isAnimation = true,
	description = "It reveals devastating energy arcs.",
	shootType = "energy",
	shootRange = 4,
	light = {
		level = 3,
		color = 143,
	},
	minReqLevel = 65,
	classification = 2,
	requiredLevel = 65,
	walkStack = true,
	vocationString = "sorcerers",
	clientId = 16096,
	abilities = {
		stats = {
			magicpoints = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "wand",
	wareId = 16096,
	id = 18390,
	article = "a",
	name = "wand of defiance",
	wieldInfo = 5,
	slotPosition = "hand",
})

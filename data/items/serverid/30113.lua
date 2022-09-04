-- name: wand of destruction
Game.createItemType(30113, 27457):register({
	weight = 3500,
	isAnimation = true,
	shootType = "energy",
	shootRange = 4,
	light = {
		level = 3,
		color = 204,
	},
	minReqLevel = 200,
	classification = 2,
	requiredLevel = 200,
	walkStack = true,
	vocationString = "sorcerers",
	clientId = 27457,
	abilities = {
		stats = {
			magicpoints = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "wand",
	wareId = 27457,
	id = 30113,
	article = "a",
	name = "wand of destruction",
	wieldInfo = 5,
	slotPosition = "hand",
})

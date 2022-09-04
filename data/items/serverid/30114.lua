-- name: rod of destruction
Game.createItemType(30114, 27458):register({
	weight = 3500,
	isAnimation = true,
	shootType = "earth",
	shootRange = 4,
	light = {
		level = 3,
		color = 204,
	},
	minReqLevel = 200,
	classification = 2,
	requiredLevel = 200,
	walkStack = true,
	vocationString = "druids",
	clientId = 27458,
	abilities = {
		stats = {
			magicpoints = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "wand",
	wareId = 27458,
	id = 30114,
	article = "a",
	name = "rod of destruction",
	wieldInfo = 5,
	slotPosition = "hand",
})

-- name: glacial rod
Game.createItemType(18412, 16118):register({
	weight = 3700,
	description = "Hurls the icy essence of the Svargrond glaciers.",
	shootType = "smallholy",
	shootRange = 4,
	minReqLevel = 65,
	classification = 2,
	requiredLevel = 65,
	walkStack = true,
	vocationString = "druids",
	clientId = 16118,
	abilities = {
		stats = {
			magicpoints = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "wand",
	wareId = 16118,
	id = 18412,
	article = "a",
	name = "glacial rod",
	wieldInfo = 5,
	slotPosition = "hand",
})

-- name: wand of darkness
Game.createItemType(28416, 25760):register({
	weight = 3200,
	shootType = "death",
	shootRange = 4,
	minReqLevel = 41,
	showDuration = true,
	requiredLevel = 41,
	walkStack = true,
	clientId = 25760,
	abilities = {
		specialSkills = {
		},
		stats = {
			magicpoints = 2,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "wand",
	id = 28416,
	article = "a",
	name = "wand of darkness",
	duration = 900,
	wieldInfo = 1,
	slotPosition = "hand",
})

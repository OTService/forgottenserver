-- name: oceanborn leviathan armor
Game.createItemType(8884, 8056):register({
	weight = 10000,
	minReqLevel = 100,
	classification = 4,
	requiredLevel = 100,
	walkStack = true,
	vocationString = "knights",
	clientId = 8056,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = -5,
			absorbpercentice = 5,
		},
		skills = {
			shield = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8056,
	id = 8884,
	article = "an",
	name = "oceanborn leviathan armor",
	armor = 15,
	wieldInfo = 5,
	slotPosition = "body",
})

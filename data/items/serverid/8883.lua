-- name: windborn colossus armor
Game.createItemType(8883, 8055):register({
	weight = 12000,
	minReqLevel = 100,
	classification = 2,
	requiredLevel = 100,
	walkStack = true,
	vocationString = "knights",
	clientId = 8055,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 5,
			absorbpercentearth = -5,
		},
		skills = {
			club = 2,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8055,
	id = 8883,
	article = "a",
	name = "windborn colossus armor",
	armor = 15,
	wieldInfo = 5,
	slotPosition = "body",
})

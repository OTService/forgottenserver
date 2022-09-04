-- name: earthborn titan armor
Game.createItemType(8882, 8054):register({
	weight = 12000,
	minReqLevel = 100,
	classification = 4,
	requiredLevel = 100,
	walkStack = true,
	vocationString = "knights",
	clientId = 8054,
	abilities = {
		absorbPercent = {
			absorbpercentfire = -5,
			absorbpercentearth = 5,
		},
		skills = {
			axe = 2,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8054,
	id = 8882,
	article = "an",
	name = "earthborn titan armor",
	armor = 15,
	wieldInfo = 5,
	slotPosition = "body",
})

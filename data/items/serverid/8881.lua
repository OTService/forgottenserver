-- name: fireborn giant armor
Game.createItemType(8881, 8053):register({
	weight = 12000,
	minReqLevel = 100,
	classification = 2,
	requiredLevel = 100,
	walkStack = true,
	vocationString = "knights",
	clientId = 8053,
	abilities = {
		absorbPercent = {
			absorbpercentfire = 5,
			absorbpercentice = -5,
		},
		skills = {
			sword = 2,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8053,
	id = 8881,
	article = "a",
	name = "fireborn giant armor",
	armor = 15,
	wieldInfo = 5,
	slotPosition = "body",
})

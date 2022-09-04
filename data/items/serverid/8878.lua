-- name: crystalline armor
Game.createItemType(8878, 8050):register({
	weight = 8450,
	minReqLevel = 60,
	classification = 2,
	requiredLevel = 60,
	walkStack = true,
	vocationString = "knights and paladins",
	clientId = 8050,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = -3,
			absorbpercentice = 3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8050,
	id = 8878,
	article = "a",
	name = "crystalline armor",
	armor = 13,
	wieldInfo = 5,
	slotPosition = "body",
})

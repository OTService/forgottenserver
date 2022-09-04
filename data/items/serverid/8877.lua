-- name: lavos armor
Game.createItemType(8877, 8049):register({
	weight = 8500,
	minReqLevel = 60,
	classification = 2,
	requiredLevel = 60,
	walkStack = true,
	vocationString = "knights and paladins",
	clientId = 8049,
	abilities = {
		absorbPercent = {
			absorbpercentfire = 3,
			absorbpercentice = -3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8049,
	id = 8877,
	article = "a",
	name = "lavos armor",
	armor = 13,
	wieldInfo = 5,
	slotPosition = "body",
})

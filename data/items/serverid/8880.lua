-- name: swamplair armor
Game.createItemType(8880, 8052):register({
	weight = 8450,
	minReqLevel = 60,
	classification = 2,
	requiredLevel = 60,
	walkStack = true,
	vocationString = "knights and paladins",
	clientId = 8052,
	abilities = {
		absorbPercent = {
			absorbpercentfire = -3,
			absorbpercentearth = 3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8052,
	id = 8880,
	article = "a",
	name = "swamplair armor",
	armor = 13,
	wieldInfo = 5,
	slotPosition = "body",
})

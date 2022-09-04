-- name: earthsoul tabard
Game.createItemType(25187, 22531):register({
	weight = 14500,
	minReqLevel = 200,
	classification = 2,
	requiredLevel = 200,
	walkStack = true,
	vocationString = "paladins",
	clientId = 22531,
	abilities = {
		absorbPercent = {
			absorbpercentfire = -8,
			absorbpercentearth = 8,
		},
		skills = {
			distance = 4,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 22531,
	id = 25187,
	article = "an",
	name = "earthsoul tabard",
	armor = 18,
	wieldInfo = 5,
	slotPosition = "body",
})

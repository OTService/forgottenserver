-- name: firesoul tabard
Game.createItemType(25186, 22530):register({
	weight = 14500,
	minReqLevel = 200,
	classification = 2,
	requiredLevel = 200,
	walkStack = true,
	vocationString = "paladins",
	clientId = 22530,
	abilities = {
		absorbPercent = {
			absorbpercentfire = 8,
			absorbpercentice = -8,
		},
		skills = {
			distance = 4,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 22530,
	id = 25186,
	article = "a",
	name = "firesoul tabard",
	armor = 18,
	wieldInfo = 5,
	slotPosition = "body",
})

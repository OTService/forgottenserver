-- name: master archer's armor
Game.createItemType(8888, 8060):register({
	weight = 6900,
	minReqLevel = 100,
	classification = 3,
	requiredLevel = 100,
	walkStack = true,
	vocationString = "paladins",
	clientId = 8060,
	abilities = {
		skills = {
			distance = 3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 8060,
	id = 8888,
	article = "a",
	name = "master archer's armor",
	armor = 15,
	wieldInfo = 5,
	slotPosition = "body",
})

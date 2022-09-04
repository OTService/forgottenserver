-- name: depth galea
Game.createItemType(15408, 13995):register({
	weight = 4600,
	description = "Enables underwater exploration.",
	minReqLevel = 150,
	requiredLevel = 150,
	walkStack = true,
	clientId = 13995,
	abilities = {
		absorbPercent = {
			absorbpercentdrown = 100,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 13995,
	id = 15408,
	article = "a",
	name = "depth galea",
	armor = 8,
	wieldInfo = 1,
	slotPosition = "head",
})

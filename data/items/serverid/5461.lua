-- name: helmet of the deep
Game.createItemType(5461, 5460):register({
	weight = 21000,
	description = "Enables underwater exploration.",
	walkStack = true,
	clientId = 5460,
	abilities = {
		absorbPercent = {
			absorbpercentdrown = 100,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 5460,
	id = 5461,
	article = "a",
	name = "helmet of the deep",
	armor = 2,
	slotPosition = "head",
})

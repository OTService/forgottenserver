-- name: witchhunter's coat
Game.createItemType(8821, 7993):register({
	weight = 2500,
	minReqLevel = 50,
	requiredLevel = 50,
	walkStack = true,
	clientId = 7993,
	abilities = {
		absorbPercent = {
			fieldabsorbpercentdeath = 2,
			absorbpercentholy = -2,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 7993,
	id = 8821,
	article = "a",
	name = "witchhunter's coat",
	armor = 11,
	wieldInfo = 1,
	slotPosition = "body",
})

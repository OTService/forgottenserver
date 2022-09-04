-- name: lightning headband
Game.createItemType(7901, 828):register({
	weight = 1000,
	classification = 2,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 828,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 4,
			absorbpercentearth = -5,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 828,
	id = 7901,
	article = "a",
	name = "lightning headband",
	armor = 5,
	wieldInfo = 4,
	slotPosition = "head",
})

-- name: spellweaver's robe
Game.createItemType(11355, 10438):register({
	weight = 2350,
	minReqLevel = 60,
	classification = 2,
	requiredLevel = 60,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 10438,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 10,
			absorbpercentearth = -10,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 10438,
	id = 11355,
	article = "a",
	name = "spellweaver's robe",
	armor = 11,
	wieldInfo = 5,
	slotPosition = "body",
})

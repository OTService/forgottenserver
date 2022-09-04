-- name: rubber cap
Game.createItemType(23536, 21165):register({
	weight = 800,
	minReqLevel = 70,
	classification = 2,
	requiredLevel = 70,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 21165,
	abilities = {
		absorbPercent = {
			absorbpercentfire = -3,
			absorbpercentearth = 3,
		},
		stats = {
			magicpoints = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 21165,
	id = 23536,
	article = "a",
	name = "rubber cap",
	armor = 5,
	wieldInfo = 5,
	slotPosition = "head",
})

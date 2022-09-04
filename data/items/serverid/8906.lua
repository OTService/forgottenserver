-- name: fiery rainbow shield
Game.createItemType(8906, 8078):register({
	weight = 6900,
	description = "It has been temporarily imbued with fire magic and boosts your shielding skill.",
	shootRange = 1,
	minReqLevel = 100,
	requiredLevel = 100,
	walkStack = true,
	vocationString = "knights",
	clientId = 8078,
	abilities = {
		skills = {
			shield = 3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 36,
	wareId = 8077,
	id = 8906,
	article = "a",
	name = "fiery rainbow shield",
	decayId = 8905,
	duration = 900,
	wieldInfo = 5,
	slotPosition = "hand",
})

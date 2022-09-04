-- name: crude umbral spellbook
Game.createItemType(22422, 20088):register({
	weight = 4000,
	shootRange = 1,
	minReqLevel = 75,
	requiredLevel = 75,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 20088,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 2,
			absorbpercentfire = 2,
			absorbpercentice = 2,
			absorbpercentearth = 2,
		},
		stats = {
			magicpoints = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 14,
	wareId = 20088,
	id = 22422,
	article = "a",
	name = "crude umbral spellbook",
	wieldInfo = 5,
	slotPosition = "hand",
})

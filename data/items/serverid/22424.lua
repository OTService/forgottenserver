-- name: umbral master spellbook
Game.createItemType(22424, 20090):register({
	weight = 3000,
	isAnimation = true,
	shootRange = 1,
	minReqLevel = 250,
	requiredLevel = 250,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 20090,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 5,
			absorbpercentfire = 5,
			absorbpercentice = 5,
			absorbpercentearth = 5,
		},
		stats = {
			magicpoints = 4,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 20,
	wareId = 20090,
	id = 22424,
	article = "an",
	name = "umbral master spellbook",
	wieldInfo = 5,
	slotPosition = "hand",
})

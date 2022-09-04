-- name: umbral spellbook
Game.createItemType(22423, 20089):register({
	weight = 3500,
	shootRange = 1,
	minReqLevel = 120,
	requiredLevel = 120,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 20089,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 3,
			absorbpercentfire = 3,
			absorbpercentice = 3,
			absorbpercentearth = 3,
		},
		stats = {
			magicpoints = 2,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 16,
	wareId = 20089,
	id = 22423,
	article = "an",
	name = "umbral spellbook",
	wieldInfo = 5,
	slotPosition = "hand",
})

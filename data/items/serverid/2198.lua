-- name: elven amulet
Game.createItemType(2198, 3082):register({
	weight = 270,
	showAttributes = true,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 3082,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 5,
			fieldabsorbpercentdeath = 5,
			absorbpercentholy = 5,
			absorbpercentfire = 5,
			absorbpercentice = 5,
			absorbpercentearth = 5,
			absorbpercentphysical = 5,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 3082,
	id = 2198,
	article = "an",
	name = "elven amulet",
	slotPosition = "necklace",
	charges = 50,
})

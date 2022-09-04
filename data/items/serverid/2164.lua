-- name: might ring
Game.createItemType(2164, 3048):register({
	weight = 100,
	showAttributes = true,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 3048,
	abilities = {
		absorbPercent = {
			absorbpercentenergy = 20,
			fieldabsorbpercentdeath = 20,
			absorbpercentholy = 20,
			absorbpercentfire = 20,
			absorbpercentice = 20,
			absorbpercentearth = 20,
			absorbpercentphysical = 20,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 3048,
	id = 2164,
	article = "a",
	name = "might ring",
	slotPosition = "ring",
	charges = 20,
})

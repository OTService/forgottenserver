-- name: leviathan's amulet
Game.createItemType(10220, 9303):register({
	weight = 700,
	description = "It holds the last breath of Leviathan.",
	minReqLevel = 80,
	requiredLevel = 80,
	showCharges = true,
	walkStack = true,
	subType = true,
	clientId = 9303,
	abilities = {
		absorbPercent = {
			absorbpercentice = 40,
			absorbpercentphysical = 60,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 9303,
	id = 10220,
	name = "leviathan's amulet",
	wieldInfo = 1,
	slotPosition = "necklace",
	charges = 5,
})

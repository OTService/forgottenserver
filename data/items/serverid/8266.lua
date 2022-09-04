-- name: Koshei's ancient amulet
Game.createItemType(8266, 7532):register({
	weight = 550,
	showAttributes = true,
	light = {
		level = 2,
		color = 30,
	},
	walkStack = true,
	clientId = 7532,
	abilities = {
		absorbPercent = {
			fieldabsorbpercentdeath = 8,
			absorbpercentholy = -50,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	useable = true,
	wareId = 7532,
	id = 8266,
	name = "Koshei's ancient amulet",
	slotPosition = "necklace",
})

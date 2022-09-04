-- name: tiara of power
Game.createItemType(26131, 23475):register({
	weight = 1150,
	transformDeEquipId = 26130,
	light = {
		level = 5,
		color = 202,
	},
	showDuration = true,
	walkStack = true,
	clientId = 23475,
	abilities = {
		manaGain = 8,
		manaTicks = 6000,
		speed = 20,
		absorbPercent = {
			absorbpercentenergy = 8,
		},
		regeneration = true,
		healthGain = 2,
		healthTicks = 6000,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 26131,
	article = "a",
	name = "tiara of power",
	armor = 7,
	duration = 3600,
	slotPosition = "head",
})

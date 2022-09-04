-- name: death ring
Game.createItemType(6301, 6300):register({
	weight = 80,
	transformDeEquipId = 6300,
	isAnimation = true,
	description = "Wearing it makes you feel a little weaker than usual.",
	showDuration = true,
	walkStack = true,
	clientId = 6300,
	abilities = {
		absorbPercent = {
			fieldabsorbpercentdeath = 5,
		},
		skills = {
			shield = -10,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 6301,
	article = "a",
	name = "death ring",
	armor = 1,
	duration = 480,
	slotPosition = "ring",
})

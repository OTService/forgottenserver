-- name: sword ring
Game.createItemType(2210, 3094):register({
	weight = 90,
	transformDeEquipId = 2207,
	isAnimation = true,
	showDuration = true,
	walkStack = true,
	clientId = 3094,
	abilities = {
		skills = {
			sword = 4,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 2210,
	article = "a",
	name = "sword ring",
	duration = 1800,
	slotPosition = "ring",
})

-- name: power ring
Game.createItemType(2203, 3087):register({
	weight = 80,
	transformDeEquipId = 2166,
	isAnimation = true,
	showDuration = true,
	walkStack = true,
	clientId = 3087,
	abilities = {
		skills = {
			fist = 6,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 2203,
	article = "a",
	name = "power ring",
	duration = 1800,
	slotPosition = "ring",
})

-- name: ring of blue plasma
Game.createItemType(26186, 23530):register({
	weight = 90,
	transformDeEquipId = 26185,
	isAnimation = true,
	minReqLevel = 100,
	showDuration = true,
	requiredLevel = 100,
	walkStack = true,
	clientId = 23530,
	abilities = {
		skills = {
			distance = 3,
		},
		stats = {
			magicpoints = 1,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 26186,
	article = "a",
	name = "ring of blue plasma",
	duration = 1800,
	wieldInfo = 1,
	slotPosition = "ring",
})

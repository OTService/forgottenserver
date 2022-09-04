-- name: ring of red plasma
Game.createItemType(26190, 23534):register({
	weight = 90,
	transformDeEquipId = 26189,
	isAnimation = true,
	minReqLevel = 100,
	showDuration = true,
	requiredLevel = 100,
	walkStack = true,
	clientId = 23534,
	abilities = {
		absorbPercent = {
			absorbpercentphysical = 3,
		},
		skills = {
			club = 3,
			sword = 3,
			axe = 3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 26190,
	article = "a",
	name = "ring of red plasma",
	duration = 1800,
	wieldInfo = 1,
	slotPosition = "ring",
})

-- name: collar of red plasma
Game.createItemType(26184, 23528):register({
	weight = 500,
	transformDeEquipId = 26200,
	isAnimation = true,
	minReqLevel = 100,
	showDuration = true,
	requiredLevel = 100,
	walkStack = true,
	clientId = 23528,
	abilities = {
		absorbPercent = {
			absorbpercentphysical = 5,
		},
		skills = {
			club = 4,
			sword = 4,
			axe = 4,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	id = 26184,
	article = "a",
	name = "collar of red plasma",
	duration = 1800,
	wieldInfo = 1,
	slotPosition = "necklace",
})

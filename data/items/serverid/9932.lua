-- name: firewalker boots
Game.createItemType(9932, 9018):register({
	weight = 950,
	showAttributes = true,
	transformDeEquipId = 9933,
	isAnimation = true,
	description = "Wearing these boots reduces the damage gotten from fiery ground.",
	minReqLevel = 130,
	showDuration = true,
	requiredLevel = 130,
	walkStack = true,
	clientId = 9018,
	abilities = {
		fieldAbsorbPercent = {
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 9020,
	id = 9932,
	name = "firewalker boots",
	decayId = 10022,
	armor = 2,
	duration = 3600,
	wieldInfo = 1,
	slotPosition = "feet",
})

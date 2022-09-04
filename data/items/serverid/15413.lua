-- name: ornate shield
Game.createItemType(15413, 14000):register({
	weight = 7100,
	showAttributes = true,
	shootRange = 1,
	minReqLevel = 130,
	requiredLevel = 130,
	walkStack = true,
	vocationString = "knights",
	clientId = 14000,
	abilities = {
		absorbPercent = {
			absorbpercentphysical = 5,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 36,
	wareId = 14000,
	id = 15413,
	article = "an",
	name = "ornate shield",
	wieldInfo = 5,
	slotPosition = "hand",
})

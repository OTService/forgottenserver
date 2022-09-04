-- name: ornate chestplate
Game.createItemType(15406, 13993):register({
	weight = 15600,
	showAttributes = true,
	minReqLevel = 200,
	classification = 2,
	requiredLevel = 200,
	walkStack = true,
	vocationString = "knights",
	clientId = 13993,
	abilities = {
		absorbPercent = {
			absorbpercentphysical = 8,
		},
		skills = {
			shield = 3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	wareId = 13993,
	id = 15406,
	article = "an",
	name = "ornate chestplate",
	armor = 16,
	wieldInfo = 5,
	slotPosition = "body",
})

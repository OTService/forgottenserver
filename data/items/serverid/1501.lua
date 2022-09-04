-- name: fire
Game.createItemType(1501, 2132):register({
	isAnimation = true,
	blockPathFind = true,
	light = {
		level = 5,
		color = 206,
	},
	magicField = true,
	walkStack = true,
	type = "magicfield",
	field = {
		damage = -10,
		ticks = 10000,
		name = "fire",
		count = 7,
	},
	clientId = 2132,
	replaceable = true,
	id = 1501,
	group = "magicfield",
	article = "a",
	name = "fire",
	decayId = 1502,
	duration = 120,
})

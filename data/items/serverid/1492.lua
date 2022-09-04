-- name: fire
Game.createItemType(1492, 2123):register({
	isAnimation = true,
	blockPathFind = true,
	light = {
		level = 7,
		color = 200,
	},
	magicField = true,
	walkStack = true,
	type = "magicfield",
	field = {
		damage = -10,
		initDamage = 20,
		ticks = 10000,
		name = "fire",
		count = 7,
	},
	clientId = 2123,
	replaceable = true,
	id = 1492,
	group = "magicfield",
	article = "a",
	name = "fire",
})

-- name: yellow flame
Game.createItemType(1500, 21465):register({
	isAnimation = true,
	blockPathFind = true,
	light = {
		level = 7,
		color = 206,
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
	clientId = 21465,
	replaceable = true,
	id = 1500,
	group = "magicfield",
	article = "a",
	name = "yellow flame",
	decayId = 1501,
	duration = 120,
})

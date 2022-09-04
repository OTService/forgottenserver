-- name: energy trap
Game.createItemType(26022, 23366):register({
	isAnimation = true,
	blockPathFind = true,
	light = {
		level = 4,
		color = 137,
	},
	magicField = true,
	walkStack = true,
	type = "magicfield",
	field = {
		damage = -25,
		initDamage = 30,
		ticks = 10000,
		name = "energy",
	},
	clientId = 23366,
	replaceable = true,
	id = 26022,
	group = "magicfield",
	article = "an",
	name = "energy trap",
})

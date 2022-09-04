-- name: energy field
Game.createItemType(1491, 2122):register({
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
	clientId = 2122,
	replaceable = true,
	id = 1491,
	group = "magicfield",
	article = "an",
	name = "energy field",
	duration = 120,
})

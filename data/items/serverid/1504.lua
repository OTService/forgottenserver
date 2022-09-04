-- name: energy field
Game.createItemType(1504, 2135):register({
	isAnimation = true,
	blockPathFind = true,
	light = {
		level = 4,
		color = 214,
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
	clientId = 2135,
	replaceable = true,
	id = 1504,
	group = "magicfield",
	article = "an",
	name = "energy field",
	duration = 120,
})

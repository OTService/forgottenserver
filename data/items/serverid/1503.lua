-- name: poison gas
Game.createItemType(1503, 2134):register({
	isAnimation = true,
	blockPathFind = true,
	light = {
		level = 2,
		color = 214,
	},
	magicField = true,
	walkStack = true,
	type = "magicfield",
	field = {
		damage = -100,
		start = 5,
		ticks = 5000,
		name = "poison",
	},
	clientId = 2134,
	replaceable = true,
	id = 1503,
	group = "magicfield",
	name = "poison gas",
	duration = 120,
})

-- name: poison gas
Game.createItemType(1496, 2121):register({
	isAnimation = true,
	blockPathFind = true,
	light = {
		level = 2,
		color = 104,
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
	clientId = 2121,
	replaceable = true,
	id = 1496,
	group = "magicfield",
	name = "poison gas",
})

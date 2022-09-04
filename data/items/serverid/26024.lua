-- name: poison trap
Game.createItemType(26024, 23368):register({
	isAnimation = true,
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
	clientId = 23368,
	replaceable = true,
	id = 26024,
	group = "magicfield",
	article = "a",
	name = "poison trap",
})

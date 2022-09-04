-- name: fire
Game.createItemType(10987, 10070):register({
	isAnimation = true,
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
	clientId = 10070,
	replaceable = true,
	id = 10987,
	group = "magicfield",
	article = "a",
	name = "fire",
})

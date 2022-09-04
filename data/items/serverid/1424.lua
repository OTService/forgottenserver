-- name: campfire
Game.createItemType(1424, 1999):register({
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
		initDamage = 20,
		ticks = 4000,
		name = "fire",
		count = 2,
	},
	clientId = 1999,
	replaceable = true,
	id = 1424,
	group = "magicfield",
	article = "a",
	name = "campfire",
})

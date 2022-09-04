-- name: campfire
Game.createItemType(1425, 2000):register({
	isAnimation = true,
	blockPathFind = true,
	light = {
		level = 3,
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
	clientId = 2000,
	replaceable = true,
	id = 1425,
	group = "magicfield",
	article = "a",
	name = "campfire",
})

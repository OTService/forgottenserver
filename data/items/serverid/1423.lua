-- name: campfire
Game.createItemType(1423, 1998):register({
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
		ticks = 4000,
		name = "fire",
		count = 2,
	},
	clientId = 1998,
	replaceable = true,
	id = 1423,
	group = "magicfield",
	article = "a",
	name = "campfire",
})

-- name: searing fire
Game.createItemType(1506, 2137):register({
	isAnimation = true,
	blockPathFind = true,
	light = {
		level = 7,
		color = 203,
	},
	magicField = true,
	walkStack = true,
	type = "magicfield",
	field = {
		damage = -300,
		name = "fire",
	},
	clientId = 2137,
	id = 1506,
	group = "magicfield",
	article = "a",
	name = "searing fire",
	decayId = 1507,
	duration = 10,
})

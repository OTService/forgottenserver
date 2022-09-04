-- name: magic wall
Game.createItemType(20669, 10181):register({
	isAnimation = true,
	blockPathFind = true,
	light = {
		level = 3,
		color = 210,
	},
	magicField = true,
	walkStack = true,
	type = "magicfield",
	clientId = 10181,
	replaceable = true,
	id = 20669,
	article = "a",
	name = "magic wall",
	duration = 20,
})

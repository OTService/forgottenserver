-- name: magic wall
Game.createItemType(11098, 2128):register({
	isAnimation = true,
	blockPathFind = true,
	light = {
		level = 3,
		color = 5,
	},
	magicField = true,
	blockProjectile = true,
	walkStack = true,
	type = "magicfield",
	clientId = 2128,
	replaceable = true,
	id = 11098,
	article = "a",
	name = "magic wall",
	duration = 20,
})

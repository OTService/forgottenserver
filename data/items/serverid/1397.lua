-- name: mystic flame
Game.createItemType(1397, 775):register({
	alwaysOnTopOrder = 2,
	isAnimation = true,
	alwaysOnTop = true,
	description = "You feel drawn to the mesmerizing light.",
	blockPathFind = true,
	light = {
		level = 2,
		color = 173,
	},
	walkStack = true,
	type = "teleport",
	clientId = 775,
	replaceable = true,
	id = 1397,
	article = "a",
	name = "mystic flame",
	effect = "teleport",
})

-- name: mystic flame
Game.createItemType(8058, 1959):register({
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
	clientId = 1959,
	replaceable = true,
	id = 8058,
	article = "a",
	name = "mystic flame",
	effect = "teleport",
})

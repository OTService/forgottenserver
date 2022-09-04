-- name: magic forcefield
Game.createItemType(1387, 1949):register({
	alwaysOnTopOrder = 2,
	isAnimation = true,
	alwaysOnTop = true,
	description = "You can see the other side through it.",
	blockPathFind = true,
	light = {
		level = 2,
		color = 29,
	},
	walkStack = true,
	type = "teleport",
	clientId = 1949,
	replaceable = true,
	id = 1387,
	article = "a",
	name = "magic forcefield",
	effect = "teleport",
})

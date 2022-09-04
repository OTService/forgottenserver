-- name: magic forcefield
Game.createItemType(5023, 5022):register({
	alwaysOnTopOrder = 2,
	isAnimation = true,
	alwaysOnTop = true,
	description = "You can see the other side trough it.",
	blockPathFind = true,
	light = {
		level = 2,
		color = 29,
	},
	walkStack = true,
	type = "teleport",
	clientId = 5022,
	replaceable = true,
	id = 5023,
	article = "a",
	name = "magic forcefield",
	effect = "teleport",
})

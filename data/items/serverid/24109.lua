-- name: water channel
Game.createItemType(24109, 21740):register({
	alwaysOnTopOrder = 2,
	isAnimation = true,
	alwaysOnTop = true,
	description = "A cold maelstrom pulls you down.",
	blockPathFind = true,
	light = {
		level = 3,
		color = 179,
	},
	walkStack = true,
	type = "teleport",
	clientId = 21740,
	replaceable = true,
	useable = true,
	id = 24109,
	article = "a",
	name = "water channel",
})

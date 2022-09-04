-- name: dirt floor
Game.createItemType(23870, 21501):register({
	description = "There is a hole in the ceiling.",
	light = {
		level = 3,
		color = 215,
	},
	speed = true,
	walkStack = true,
	forceUse = true,
	clientId = 21501,
	replaceable = true,
	id = 23870,
	group = "ground",
	name = "dirt floor",
	groundTile = true,
})

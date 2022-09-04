-- name: dirt floor
Game.createItemType(8278, 386):register({
	description = "There is a hole in the ceiling.",
	light = {
		level = 3,
		color = 215,
	},
	speed = true,
	walkStack = true,
	forceUse = true,
	clientId = 386,
	replaceable = true,
	id = 8278,
	group = "ground",
	name = "dirt floor",
	groundTile = true,
})

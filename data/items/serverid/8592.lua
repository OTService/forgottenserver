-- name: dirt floor
Game.createItemType(8592, 7762):register({
	description = "Rightclick your rope and select 'Use', then leftclick on this spot to climb up again.",
	light = {
		level = 5,
		color = 198,
	},
	speed = true,
	walkStack = true,
	forceUse = true,
	clientId = 7762,
	replaceable = true,
	id = 8592,
	group = "ground",
	name = "dirt floor",
	groundTile = true,
})

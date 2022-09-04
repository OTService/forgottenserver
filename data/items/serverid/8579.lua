-- name: loose stone pile
Game.createItemType(8579, 7749):register({
	description = "Rightclick on your shovel, select 'Use' and then leftclick on this stonepile to dig.",
	blockPathFind = true,
	speed = true,
	walkStack = true,
	forceUse = true,
	clientId = 7749,
	replaceable = true,
	id = 8579,
	group = "ground",
	article = "a",
	name = "loose stone pile",
	groundTile = true,
})

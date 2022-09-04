-- name: shallow water
Game.createItemType(4612, 4601):register({
	isAnimation = true,
	description = "You see the silvery movement of fish.",
	walkStack = true,
	type = "trashholder",
	clientId = 4601,
	pickupable = true,
	replaceable = true,
	blockSolid = true,
	id = 4612,
	group = "ground",
	name = "shallow water",
	effect = "bluebubble",
	groundTile = true,
	slotPosition = "hand",
	fluidSource = "water",
})

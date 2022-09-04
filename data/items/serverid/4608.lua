-- name: shallow water
Game.createItemType(4608, 4597):register({
	isAnimation = true,
	description = "You see the silvery movement of fish.",
	walkStack = true,
	type = "trashholder",
	clientId = 4597,
	pickupable = true,
	replaceable = true,
	blockSolid = true,
	id = 4608,
	group = "ground",
	name = "shallow water",
	effect = "bluebubble",
	groundTile = true,
	slotPosition = "hand",
	fluidSource = "water",
})

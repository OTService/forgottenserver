-- name: shallow water
Game.createItemType(4610, 4599):register({
	isAnimation = true,
	description = "You see the silvery movement of fish.",
	walkStack = true,
	type = "trashholder",
	clientId = 4599,
	pickupable = true,
	replaceable = true,
	blockSolid = true,
	id = 4610,
	group = "ground",
	name = "shallow water",
	effect = "bluebubble",
	groundTile = true,
	slotPosition = "hand",
	fluidSource = "water",
})

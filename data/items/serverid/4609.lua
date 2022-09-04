-- name: shallow water
Game.createItemType(4609, 4598):register({
	isAnimation = true,
	description = "You see the silvery movement of fish.",
	walkStack = true,
	type = "trashholder",
	clientId = 4598,
	pickupable = true,
	replaceable = true,
	blockSolid = true,
	id = 4609,
	group = "ground",
	name = "shallow water",
	effect = "bluebubble",
	groundTile = true,
	slotPosition = "hand",
	fluidSource = "water",
})

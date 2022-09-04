-- name: shallow water
Game.createItemType(4611, 4600):register({
	isAnimation = true,
	description = "You see the silvery movement of fish.",
	walkStack = true,
	type = "trashholder",
	clientId = 4600,
	pickupable = true,
	replaceable = true,
	blockSolid = true,
	id = 4611,
	group = "ground",
	name = "shallow water",
	effect = "bluebubble",
	groundTile = true,
	slotPosition = "hand",
	fluidSource = "water",
})

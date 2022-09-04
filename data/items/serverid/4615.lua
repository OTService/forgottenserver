-- name: shallow water
Game.createItemType(4615, 4610):register({
	isAnimation = true,
	description = "You see the silvery movement of fish.",
	walkStack = true,
	type = "trashholder",
	clientId = 4610,
	pickupable = true,
	replaceable = true,
	blockSolid = true,
	id = 4615,
	group = "ground",
	name = "shallow water",
	effect = "bluebubble",
	groundTile = true,
	slotPosition = "hand",
	fluidSource = "water",
})

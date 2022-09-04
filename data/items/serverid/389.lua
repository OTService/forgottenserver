-- name: lava hole
Game.createItemType(389, 391):register({
	alwaysOnTopOrder = 2,
	isAnimation = true,
	alwaysOnTop = true,
	description = "It emits heat and light.",
	light = {
		level = 3,
		color = 200,
	},
	walkStack = true,
	clientId = 391,
	replaceable = true,
	blockSolid = true,
	id = 389,
	article = "a",
	name = "lava hole",
	fluidSource = "lava",
})

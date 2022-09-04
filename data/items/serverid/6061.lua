-- name: dead serpent spawn
Game.createItemType(6061, 6061):register({
	walkStack = true,
	type = "container",
	forceUse = true,
	containerSize = 40,
	capacity = 40,
	clientId = 6061,
	replaceable = true,
	container = true,
	id = 6061,
	group = "container",
	article = "a",
	corpseType = "blood",
	name = "dead serpent spawn",
	decayId = 4323,
	duration = 10,
	fluidSource = "blood",
})

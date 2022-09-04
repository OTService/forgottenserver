-- name: dead serpent spawn
Game.createItemType(4323, 4388):register({
	walkStack = true,
	type = "container",
	containerSize = 40,
	capacity = 40,
	clientId = 4388,
	moveable = true,
	replaceable = true,
	container = true,
	id = 4323,
	group = "container",
	article = "a",
	corpseType = "blood",
	name = "dead serpent spawn",
	decayId = 4324,
	duration = 900,
	fluidSource = "blood",
})

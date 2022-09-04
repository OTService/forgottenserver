-- name: dead brimstone bug
Game.createItemType(12527, 11571):register({
	walkStack = true,
	type = "container",
	forceUse = true,
	containerSize = 10,
	capacity = 10,
	clientId = 11571,
	replaceable = true,
	container = true,
	id = 12527,
	group = "container",
	article = "a",
	corpseType = "venom",
	name = "dead brimstone bug",
	decayId = 12528,
	duration = 10,
	fluidSource = "slime",
})

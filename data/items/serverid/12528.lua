-- name: dead brimstone bug
Game.createItemType(12528, 11572):register({
	walkStack = true,
	type = "container",
	containerSize = 10,
	capacity = 10,
	clientId = 11572,
	moveable = true,
	replaceable = true,
	container = true,
	id = 12528,
	group = "container",
	article = "a",
	corpseType = "venom",
	name = "dead brimstone bug",
	decayId = 12538,
	duration = 900,
	fluidSource = "slime",
})

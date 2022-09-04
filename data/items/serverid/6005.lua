-- name: split stone golem
Game.createItemType(6005, 6005):register({
	walkStack = true,
	type = "container",
	forceUse = true,
	containerSize = 30,
	capacity = 30,
	clientId = 6005,
	replaceable = true,
	container = true,
	id = 6005,
	group = "container",
	article = "a",
	corpseType = "undead",
	name = "split stone golem",
	decayId = 2952,
	duration = 10,
})

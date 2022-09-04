-- name: dead war golem
Game.createItemType(10003, 9090):register({
	walkStack = true,
	type = "container",
	containerSize = 38,
	capacity = 38,
	clientId = 9090,
	moveable = true,
	replaceable = true,
	container = true,
	id = 10003,
	group = "container",
	article = "a",
	corpseType = "energy",
	name = "dead war golem",
	decayId = 10004,
	duration = 600,
})

-- name: dead orc
Game.createItemType(2820, 4001):register({
	walkStack = true,
	type = "container",
	containerSize = 20,
	capacity = 20,
	clientId = 4001,
	moveable = true,
	pickupable = true,
	replaceable = true,
	container = true,
	id = 2820,
	group = "container",
	article = "a",
	corpseType = "blood",
	name = "dead orc",
	decayId = 3080,
	duration = 10,
	slotPosition = "hand",
	fluidSource = "blood",
})
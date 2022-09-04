-- name: remains of a water elemental
Game.createItemType(10499, 9582):register({
	description = "Something is wobbling inside. Maybe with a fishing rod you could reach it.",
	walkStack = true,
	type = "container",
	containerSize = 20,
	capacity = 20,
	clientId = 9582,
	replaceable = true,
	container = true,
	id = 10499,
	group = "container",
	corpseType = "undead",
	name = "remains of a water elemental",
	decayId = 10500,
	duration = 300,
	fluidSource = "water",
})

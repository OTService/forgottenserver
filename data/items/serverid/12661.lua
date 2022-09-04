-- name: remains of a water elemental
Game.createItemType(12661, 4037):register({
	description = "Something is wobbling inside. Maybe with a fishing rod you could reach it.",
	walkStack = true,
	type = "container",
	forceUse = true,
	containerSize = 20,
	capacity = 20,
	clientId = 4037,
	replaceable = true,
	container = true,
	id = 12661,
	group = "container",
	corpseType = "undead",
	name = "remains of a water elemental",
	decayId = 10500,
	duration = 600,
	fluidSource = "water",
})

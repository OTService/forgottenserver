-- name: slain undead dragon
Game.createItemType(6306, 6305):register({
	walkStack = true,
	type = "container",
	forceUse = true,
	containerSize = 48,
	capacity = 48,
	clientId = 6305,
	replaceable = true,
	container = true,
	id = 6306,
	group = "container",
	article = "a",
	corpseType = "undead",
	name = "slain undead dragon",
	decayId = 6307,
	duration = 10,
})

-- name: closed door
Game.createItemType(5281, 5280):register({
	description = "It is locked.",
	blockProjectile = true,
	walkStack = true,
	door = true,
	type = "door",
	clientId = 5280,
	replaceable = true,
	blockSolid = true,
	id = 5281,
	article = "a",
	name = "closed door",
})

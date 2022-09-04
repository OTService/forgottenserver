-- name: closed door
Game.createItemType(1234, 1653):register({
	description = "It is locked.",
	blockProjectile = true,
	walkStack = true,
	door = true,
	type = "door",
	clientId = 1653,
	replaceable = true,
	blockSolid = true,
	id = 1234,
	article = "a",
	name = "closed door",
})

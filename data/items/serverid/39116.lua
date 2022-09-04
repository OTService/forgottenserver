-- name: locked door
Game.createItemType(39116, 36460):register({
	description = "It is locked.",
	light = {
		level = 3,
		color = 215,
	},
	blockProjectile = true,
	walkStack = true,
	door = true,
	type = "door",
	clientId = 36460,
	replaceable = true,
	blockSolid = true,
	id = 39116,
	article = "a",
	name = "locked door",
})

-- name: glue dispenser
Game.createItemType(10080, 9161):register({
	isAnimation = true,
	description = "The machine is constantly creating wonder glue.",
	blockPathFind = true,
	light = {
		level = 2,
		color = 30,
	},
	walkStack = true,
	clientId = 9161,
	replaceable = true,
	blockSolid = true,
	id = 10080,
	article = "a",
	name = "glue dispenser",
})

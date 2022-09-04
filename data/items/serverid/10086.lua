-- name: energy generator
Game.createItemType(10086, 9167):register({
	isAnimation = true,
	description = "This machine is constantly producing glowing energy nets.",
	light = {
		level = 2,
		color = 213,
	},
	walkStack = true,
	clientId = 9167,
	replaceable = true,
	blockSolid = true,
	id = 10086,
	article = "an",
	name = "energy generator",
})

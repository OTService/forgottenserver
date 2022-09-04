-- name: spell rune
Game.createItemType(8919, 8091):register({
	isAnimation = true,
	description = "It is Yandur's healing rune. You can't remove it from his house, but you may use it.",
	light = {
		level = 1,
		color = 215,
	},
	walkStack = true,
	clientId = 8091,
	replaceable = true,
	useable = true,
	blockSolid = true,
	id = 8919,
	article = "a",
	name = "spell rune",
})

-- name: smoke trap
Game.createItemType(26023, 23367):register({
	isAnimation = true,
	magicField = true,
	walkStack = true,
	type = "magicfield",
	field = {
		damage = -25,
		initDamage = 30,
		ticks = 10000,
		name = "poison",
	},
	clientId = 23367,
	replaceable = true,
	id = 26023,
	group = "magicfield",
	article = "a",
	name = "smoke trap",
})

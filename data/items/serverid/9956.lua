-- name: magical torch
Game.createItemType(9956, 9042):register({
	weight = 1800,
	isAnimation = true,
	description = "Light all important locations given to you by Lucius before one of the basins stops burning.",
	light = {
		level = 8,
		color = 198,
	},
	showDuration = true,
	walkStack = true,
	clientId = 9042,
	abilities = {
		speed = 50,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	useable = true,
	defense = 25,
	id = 9956,
	article = "a",
	name = "magical torch",
	duration = 86400,
	slotPosition = "ammo",
})

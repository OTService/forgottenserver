-- name: gold pouch
Game.createItemType(26377, 23721):register({
	description = "Tired of carrying lots and lots of backpacks filled with money? In a Gold Pouch you can carry as many gold, platinum or crystal coins as your capacity allows.",
	walkStack = true,
	type = "container",
	containerSize = 40,
	capacity = 40,
	clientId = 23721,
	moveable = true,
	pickupable = true,
	replaceable = true,
	container = true,
	id = 26377,
	group = "container",
	article = "a",
	name = "gold pouch",
	slotPosition = "hand",
})

-- name: flaming arrow
Game.createItemType(7840, 763):register({
	weight = 70,
	isAnimation = true,
	shootType = "shiverarrow",
	shootRange = 1,
	minReqLevel = 20,
	requiredLevel = 20,
	walkStack = true,
	elementDamage = 14,
	subType = true,
	elementType = 8,
	maxHitChance = 91,
	clientId = 763,
	stackable = true,
	abilities = {
		elementDamage = 14,
		elementType = 8,
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "ammunition",
	wareId = 763,
	showCount = true,
	id = 7840,
	article = "a",
	pluralName = "flaming arrows",
	name = "flaming arrow",
	ammoType = "arrow",
	attack = 14,
	wieldInfo = 1,
	slotPosition = "ammo",
})
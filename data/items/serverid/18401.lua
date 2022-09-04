-- name: spellbook of vigilance
Game.createItemType(18401, 16107):register({
	weight = 2700,
	showAttributes = true,
	description = "It shows your spells and can also shield against attacks when worn.",
	shootRange = 1,
	minReqLevel = 130,
	requiredLevel = 130,
	walkStack = true,
	vocationString = "sorcerers and druids",
	clientId = 16107,
	abilities = {
		absorbPercent = {
			absorbpercentfire = -3,
			absorbpercentearth = 3,
		},
		stats = {
			magicpoints = 3,
		},
	},
	moveable = true,
	pickupable = true,
	replaceable = true,
	weaponType = "shield",
	defense = 20,
	wareId = 16107,
	id = 18401,
	article = "a",
	name = "spellbook of vigilance",
	wieldInfo = 5,
	slotPosition = "hand",
})

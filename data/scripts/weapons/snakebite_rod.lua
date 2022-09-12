-- snakebite rod
local sbr = Weapon({
	weaponType = WEAPON_WAND,
	id = 2182,
	level = 7,
	damage = {8, 18},
	mana = 2,
	vocation = {{"druid", true, false}, {"elder druid", false, false}},
	element = "earth"
})

sbr:register()

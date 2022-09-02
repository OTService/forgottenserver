local abilities = {
    elementice = true,
    elementearth = true,
    elementfire = true,
    elementenergy = true,
    elementdeath = true,
    elementholy = true,
    invisible = true,
    speed = true,
    healthgain = true,
    healthgainpercent = true,
    healthticks = true,
    managain = true,
    managainpercent = true,
    manaticks = true,
    regeneration = true,
    shoottype = true,
    manashield = true,
    skillsword = true,
    skillaxe = true,
    skillclub = true,
    skilldist = true,
    skillfish = true,
    skillshield = true,
    skillfist = true,
    maxhitpoints = true,
    maxhitpointspercent = true,
    maxmanapoints = true,
    maxmanapointspercent = true,
    magicpoints = true,
    magiclevelpoints = true,
    magicpointspercent = true,
    criticalhitchance = true,
    criticalhitamount = true,
    lifeleechchance = true,
    lifeleechamount = true,
    manaleechchance = true,
    manaleechamount = true,
    fatalchance = true,
    fatalamount = true,
    dodgechance = true,
    dodgeamount = true,
    momentumchance = true,
    momentumamount = true,
    xprate = true,
    xplowbonus = true,
    xpboost = true,
    xpstamina = true,
    lootrate = true,
    attackspeedpercent = true,
    extradamage = true,
    fieldabsorbpercentenergy = true,
    fieldabsorbpercentfire = true,
    fieldabsorbpercentpoison = true,
    fieldabsorbpercentearth = true,
    absorbpercentall = true,
    absorbpercentelements = true,
    absorbpercentmagic = true,
    absorbpercentenergy = true,
    absorbpercentfire = true,
    absorbpercentpoison = true,
    absorbpercentearth = true,
    absorbpercentice = true,
    absorbpercentholy = true,
    absorbpercentdeath = true,
    absorbpercentlifedrain = true,
    absorbpercentmanadrain = true,
    absorbpercentdrown = true,
    absorbpercentphysical = true,
    absorbpercenthealing = true,
    absorbpercentundefined = true,
    suppressdrunk = true,
    suppressenergy = true,
    suppressfire = true,
    suppresspoison = true,
    suppressdrown = true,
    suppressphysical = true,
    suppressfreeze = true,
    suppressdazzle = true,
    suppresscurse = true
}

function ItemType.register(itemType, properties)
	for key, value in pairs(properties) do
		local parse = itemType[key]
		if parse then
			if key == "light" then
				parse(itemType, value.level, value.color)
			else
				parse(itemType, value)
			end
		else
			if not itemType:abilities(abilities[key:lower()], value) then
				print(string.format("[ItemType.register - Warning] invalid key: %s", key))
			end
		end
	end

	--check bed items
	if ((itemType:femaleSleeper() ~= 0 or itemType:maleSleeper() ~= 0) and itemType:type() ~= ITEM_TYPE_BED) then
		print("[Warning - Items::parseItemNode] Item " .. itemType:getId() .. " is not set as a bed-type")
	end
end

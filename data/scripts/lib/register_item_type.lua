

function ItemType.register(itemType, properties)
	for key, value in pairs(properties) do
		local parse = itemType[key]
		if parse then
			if key == "light" then
				parse(itemType, value.level, value.color)
			elseif key == "abilities" then
                for k, v in pairs(value) do
                    if type(v) == "table" then
                        for a, b in pairs(v) do
                            itemType:abilities(a, b)
                        end
                    else
                        itemType:abilities(k, v)
                    end
                end
            else
				parse(itemType, value)
			end
		else
			print(string.format("[ItemType.register - Warning] invalid key: %s", key))
		end
	end

	--check bed items
	if ((itemType:femaleSleeper() ~= 0 or itemType:maleSleeper() ~= 0) and itemType:type() ~= ITEM_TYPE_BED) then
		print("[Warning - Items::parseItemNode] Item " .. itemType:getId() .. " is not set as a bed-type")
	end
end

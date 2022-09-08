local clawOfTheNoxiousSpawn = MoveEvent({slot="ring", id=10310})

function clawOfTheNoxiousSpawn.onEquip(player, item, slot, isCheck)
	if not isCheck then
		if not Tile(player:getPosition()):hasFlag(TILESTATE_PROTECTIONZONE) then
			doTargetCombat(0, player, COMBAT_PHYSICALDAMAGE, -150, -200, CONST_ME_DRAWBLOOD)
			player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "Ouch! The serpent claw stabbed you.")
			return true
		end
	end
	return true
end

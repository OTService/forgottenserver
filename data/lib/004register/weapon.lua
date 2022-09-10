local WeaponRegister = false
local notifiyOldSystem = false
local alreadyRegistered = false
do
	local mt = getmetatable(Weapon)
	local defaultCall = mt.__call

	mt.__call = function(self, params)
		-- we need to make sure that the Weapon contains a setup table, if not we are using an outdated version
		if type(params) == "number" then
			if not notifiyOldSystem then
				print("You are using scripts with an outdated version of revscriptsys (Weapon)")
				notifiyOldSystem = true
			end
			return defaultCall(self, params)
		end
		-- we are adding the table params with the parameters onto self without calling __newindex
		WeaponRegister = params
		-- we need to push the weaponType directly as we need to know which kind of weapon we have to create
		return defaultCall(self, params.weaponType)
	end
end

-- hooking the callback function to c
-- if not we are just adding it as a regular table index without calling __newindex
do
	local function WeaponNewIndex(self, key, value)
		-- we need to make sure that we are pushing something as a callback function
		if type(value) == "function" then
			-- we know now that it is a function and hook it in c
			self:onUseWeapon(key, value)

			-- checking for outdated revscriptsys
			if not WeaponRegister then
	    		-- using outdated version, we just gracefully return
	    		return
			end
			-- now that we know that we have a hooked event we want to pass the params and register

			-- some prior checks to see if there are words set
			if not WeaponRegister.id then
				print("There is no id set for this callback: ".. key)
				return
			end

			-- we are safe to go now as we are sure that everything is correct
			for func, params in pairs(WeaponRegister) do
				if type(params) == "table" then
					self[func](self, unpack(params))
				else
					self[func](self, params)
				end
			end

			-- now we are registering, which frees our userdata
			alreadyRegistered = true
			self:register()
			-- resetting the global variable which holds our parameter table
			WeaponRegister = false

			return
		end
		rawset(self, key, value)
	end
	rawgetmetatable("Weapon").__newindex = WeaponNewIndex
end

-- we need to check for weapons without a function and register them correctly
do
	local register = Weapon.register
	function Weapon.register(self)
		if not alreadyRegistered then
			if WeaponRegister then
				if not WeaponRegister.id then
					print("There is no id set for this callback: ".. key)
					return
				end

				-- we are safe to go now as we are sure that everything is correct
				for func, params in pairs(WeaponRegister) do
					if type(params) == "table" then
						self[func](self, unpack(params))
					else
						self[func](self, params)
					end
				end

				-- now we are registering, which frees our userdata
				register(self)
				-- resetting the global variable which holds our parameter table
				WeaponRegister = false
				return
			end
		end
		register(self)
		alreadyRegistered = false
	end
end

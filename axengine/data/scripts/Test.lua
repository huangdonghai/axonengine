--[[
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
]]--

module( ...,package.seeall )

print( "module test" )
--[[

weapons

mt	empty

1gt 1hand Grenade Throw
1hm	1hand Melee
1md	1hand mine drop
1hp	1hand pistol
2ha	2hand automatic
2hh	2hand handle
2hl	2hand launcher
2hm	2hand melee
2hr	2hand rifle
h2h	hand2hand

locomotions

AI_IDLE
AI_FORWARD
AI_BACKWARD
AI_LEFT
AI_RIGHT
AI_TURN_LEFT
AI_TURN_RIGHT

stances
AI_PRONE
AI_CROUCH
AI_STAND
AI_SWIN

flags
AI_ONGROUND
AI_ONLADDER
AI_PAIN
AI_RUSH


events

AI_JUMP
AI_HARDLANDING
AI_SOFTLANDING
AI_RELOAD
AI_ATTACK_HELD
AI_WEAPON_FIRED

--]]--

idle = {}

function idle.enterState(context)
end

states = {
	idle = {
		enterState = function() 
		end
	}
	
}
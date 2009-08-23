--[[
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
]]--

module( ..., package.seeall );

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

IDLE
FORWARD
BACKWARD
LEFT
RIGHT
TURN_LEFT
TURN_RIGHT

stances
PRONE
CROUCH
STAND
SWIN

flags
ONGROUND
ONLADDER
PAIN
RUSH


events

JUMP
HARDLANDING
SOFTLANDING
RELOAD
ATTACK_HELD
WEAPON_FIRED

--]]--

assets = {
	rig = "",
	channels = {
		torso = "Bip01 Spine",
		legs = "Bip01 Pelvis",
	},
	
	animations = {
		mtidle = "meshes/characters/_male/locomotion/male/mtidle",
		mtforeward = "meshes/characters/_male/locomotion/male/mtforeward",
		mtbackward = "meshes/characters/_male/locomotion/male/mtbackward",
		mtleft = "meshes/characters/_male/locomotion/male/mtleft",
		mtright = "meshes/characters/_male/locomotion/male/mtright",
	},
}

states = {
	idle = {},
	
		
	walk = {
		enterState = function(context)
		end,
		
		leaveState = function(context)
		end,
		
		update = function(context)
		end
	},
	
	walkback = {
	},
	
	walkright = {
	},
	
	walkleft = {
	},
	
	run = {
	},
	
	runback = {
	},
	
	runright = {
	},
	
	runleft = {
	},
	
	aim = {}
}

events = {
	jump = {
	},
	
	land = {},
	equip = {},
	unequip = {},
	attack = {},
	jam = {},
	reload = {},
}

function onUpdate(context)
	if context.animEvent ~= EVENT_NONE then
	end
	
	if context.locomotion == LM_IDLE then
		context.setState( "idle" );
	end
end


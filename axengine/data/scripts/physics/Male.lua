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


state inputs
IN_FORWARD
IN_BACKWARD
IN_LEFT
IN_RIGHT
IN_TURN_LEFT
IN_TURN_RIGHT

IN_STAND
IN_CROUCH
IN_PRONE
IN_SWIN

IN_ONGROUND
IN_ONLADDER
IN_PAIN
IN_RUSH
IN_JUMP
IN_HARDLANDING
IN_SOFTLANDING
IN_RELOAD
IN_ATTACK_HELD
IN_WEAPON_FIRED


--]]

-- local constant
local CHAN_LEGS = 0;
local CHAN_TORSO = 1;

-- constant definitions. don't change this if you don't change C++ code

-- context.weapon
local WEAPON_mt = 0;
local WEAPON_1gt = 1;
local WEAPON_1hm = 2;
local WEAPON_1md = 3;
local WEAPON_1hp = 4;
local WEAPON_2ha = 5;
local WEAPON_2hh = 6;
local WEAPON_2hl = 7;
local WEAPON_2hm = 8;
local WEAPON_2hr = 9;
local WEAPON_h2h = 10;


assets = {
	rig = "models/characters/_male/skeleton.rig",
	
	channels = {
		{ CHAN_LEGS, "legs", { "Bip01", "Bip01 Pelvis" } },
		{ CHAN_TORSO, "torso", { "Bip01 Spine" } },
	},
	
	animations = {
		mtidle = "models/characters/_male/locomotion/mtidle.anim",
		
		mtforward = "models/characters/_male/locomotion/male/mtforward.anim",
		mtbackward = "models/characters/_male/locomotion/male/mtbackward.anim",
		mtleft = "models/characters/_male/locomotion/male/mtleft.anim",
		mtright = "models/characters/_male/locomotion/male/mtright.anim",
		
		mtfastforward = "models/characters/_male/locomotion/male/mtfastforward.anim",
		mtfastbackward = "models/characters/_male/locomotion/male/mtfastbackward.anim",
		mtfastleft = "models/characters/_male/locomotion/male/mtfastleft.anim",
		mtfastright = "models/characters/_male/locomotion/male/mtfastright.anim",
	},
}

torso = {
	commonLoco = function(context)
		if context.IN_FORWARD and not context.IN_BACKWARD then
			context.switchState( "walk", 0.1 );
			
		elseif context.IN_BACKWARD and not context.IN_FORWARD then
			context.switchState( "walkback", 0.1 );
			
		elseif context.IN_LEFT and not context.IN_RIGHT then
			context.switchState( "walkleft", 0.1 );
			
		elseif context.IN_RIGHT and not context.IN_LEFT then
			context.switchState( "walkright", 0.1 );
			
		else
			context:switchState( "idle", 0.1 );
		end
	end,
	
	idle = {
		enterState = function(context)
			context:playCycle("mtidle");
		end,
		
		update = function(context)
			commonLoco(context);
		end
	},
	
		
	walk = {
		enterState = function(context)
			context:playCycle("mtforward");
		end,
		
		update = function(context)
			commonLoco(context);
		end
	},
	
	walkback = {
		enterState = function(context)
			context:playCycle("mtbackward");
		end,
		
		update = function(context)
			commonLoco(context);
		end
	},
	
	walkright = {
		enterState = function(context)
			context:playCycle("mtright");
		end,
		
		update = function(context)
			commonLoco(context);
		end
	},
	
	walkleft = {
		enterState = function(context)
			context:playCycle("mtleft");
		end,
		
		update = function(context)
			commonLoco(context);
		end
	},
	
	run = {
	},
	
	runback = {
	},
	
	runright = {
	},
	
	runleft = {
	},
	
	aim = {},
}
torso.start = torso.idle;

legs = {
	commonLoco = function(context)
		if context.IN_FORWARD then
			if context.IN_RUSH then
				context:switchState( "run", 0.1 );
			else
				context:switchState( "walk", 0.1 );
			end
			
		elseif context.IN_BACKWARD then
			if context.IN_RUSH then
				context:switchState( "runback", 0.1 );
			else
				context:switchState( "walkback", 0.1 );
			end
			
		elseif context.IN_LEFT then
			if context.IN_RUSH then
				context:switchState( "runleft", 0.1 );
			else
				context:switchState( "walkleft", 0.1 );
			end
			
		elseif context.IN_RIGHT then
			if context.IN_RUSH then
				context:switchState( "runright", 0.1 );
			else
				context:switchState( "walkright", 0.1 );
			end
			
		else
			context:switchState( "idle", 0.1 );
		end
	end,
	
	idle = {
		enterState = function(context)
			context:playCycle("mtidle");
		end,
		
		update = function(context)
			legs.commonLoco(context);
		end
	},
	
		
	walk = {
		enterState = function(context)
			context:playCycle("mtforward");
		end,
		
		update = function(context)
			legs.commonLoco(context);
		end
	},
	
	walkback = {
		enterState = function(context)
			context:playCycle("mtbackward");
		end,
		
		update = function(context)
			legs.commonLoco(context);
		end
	},
	
	walkright = {
		enterState = function(context)
			context:playCycle("mtright");
		end,
		
		update = function(context)
			legs.commonLoco(context);
		end
	},
	
	walkleft = {
		enterState = function(context)
			context:playCycle("mtleft");
		end,
		
		update = function(context)
			legs.commonLoco(context);
		end
	},
	
	run = {
		enterState = function(context)
			context:playCycle("mtfastforward");
		end,
		
		update = function(context)
			legs.commonLoco(context);
		end
	},
	
	runback = {
		enterState = function(context)
			context:playCycle("mtfastbackward");
		end,
		
		update = function(context)
			legs.commonLoco(context);
		end
	},
	
	runright = {
		enterState = function(context)
			context:playCycle("mtfastright");
		end,
		
		update = function(context)
			legs.commonLoco(context);
		end
	},
	
	runleft = {
		enterState = function(context)
			context:playCycle("mtfastleft");
		end,
		
		update = function(context)
			legs.commonLoco(context);
		end
	},
	
	aim = {},
}

legs.start = legs.idle;

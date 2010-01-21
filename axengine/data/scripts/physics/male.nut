
class LegsChannel extends AnimationChannel
{
	function commonLoco(context)
	{
		if (context.IN_FORWARD) {
			if (context.IN_RUSH) {
				context.switchState( "run", 0.1 );
			} else {
				context.switchState( "walk", 0.1 );
			}
			
		} else if (context.IN_BACKWARD) {
			if (context.IN_RUSH) {
				context.switchState( "runback", 0.1 );
			} else {
				context.switchState( "walkback", 0.1 );
			}
			
		} else if (context.IN_LEFT) {
			if (context.IN_RUSH) {
				context.switchState( "runleft", 0.1 );
			} else {
				context.switchState( "walkleft", 0.1 );
			}
			
		} else if (context.IN_RIGHT) {
			if (context.IN_RUSH) {
				context.switchState( "runright", 0.1 );
			} else {
				context.switchState( "walkright", 0.1 );
			}
			
		} else {
			context.switchState( "idle", 0.1 );
		}
	}
	
	idle = {
		enterState = function(context) {
			context.playCycle("mtidle");
		}
		
		update = function(context)
			legs.commonLoco(context);
		}
	},
	
		
	walk = {
		enterState = function(context) {
			context.playCycle("mtforward");
		}
		
		update = function(context) {
			legs.commonLoco(context);
		}
	},
	
	walkback = {
		enterState = function(context) {
			context.playCycle("mtbackward");
		}
		
		update = function(context) {
			legs.commonLoco(context);
		}
	},
	
	walkright = {
		enterState = function(context) {
			context.playCycle("mtright");
		}
		
		update = function(context) {
			legs.commonLoco(context);
		}
	},
	
	walkleft = {
		enterState = function(context) {
			context.playCycle("mtleft");
		}
		
		update = function(context) {
			legs.commonLoco(context);
		}
	},
	
	run = {
		enterState = function(context) {
			context.playCycle("mtfastforward");
		}
		
		update = function(context) {
			legs.commonLoco(context);
		}
	},
	
	runback = {
		enterState = function(context) {
			context.playCycle("mtfastbackward");
		}
		
		update = function(context) {
			legs.commonLoco(context);
		}
	},
	
	runright = {
		enterState = function(context) {
			context.playCycle("mtfastright");
		}
		
		update = function(context) {
			legs.commonLoco(context);
		}
	},
	
	runleft = {
		enterState = function(context) {
			context.playCycle("mtfastleft");
		}
		
		update = function(context) {
			legs.commonLoco(context);
		}
	},
	
	aim = {},
}

class TorsoChannel extends AnimationChannel
{
	commonLoco = function() {
		if (context.IN_FORWARD && !context.IN_BACKWARD) {
			context.switchState( "walk", 0.1 );
			
		} else if (context.IN_BACKWARD && !context.IN_FORWARD) {
			context.switchState( "walkback", 0.1 );
			
		} else if (context.IN_LEFT && !context.IN_RIGHT) {
			context.switchState( "walkleft", 0.1 );
			
		} else if (context.IN_RIGHT && !context.IN_LEFT) {
			context.switchState( "walkright", 0.1 );
			
		} else {
			context.switchState( "idle", 0.1 );
		}
	}
	
	idle = {
		enterState = function() {
			context.playCycle("mtidle");
		}
		
		update = function() {
			commonLoco(context);
		}
	}
	
		
	walk = {
		enterState = function() {
			context.playCycle("mtforward");
		}
		
		update = function() {
			commonLoco(context);
		}
	},
	
	walkback = {
		enterState = function() {
			context:playCycle("mtbackward");
		}
		
		update = function() {
			commonLoco(context);
		}
	},
	
	walkright = {
		enterState = function() {
			context:playCycle("mtright");
		}
		
		update = function()
			commonLoco(context);
		}
	},
	
	walkleft = {
		enterState = function() {
			context:playCycle("mtleft");
		}
		
		update = function() {
			commonLoco(context);
		}
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

class MaleContext extends AnimationContext
{
	static assets = {
		rig = "models/characters/_male/skeleton.rig",
		
		channels = [
			[CHAN_LEGS, "legs", ["Bip01", "Bip01 Pelvis"]],
			[CHAN_TORSO, "torso", ["Bip01 Spine"] ],
		],
		
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
	
	legsChannel = null;
	torseChannel = null;
	
	constructor()
	{
		legsChannel = LegsChannel(this);
		torseChannel = TorseChannel(this);
	}
}
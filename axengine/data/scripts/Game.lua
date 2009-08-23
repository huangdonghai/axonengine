--[[
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
]]--

module( ..., package.seeall );

print( "loading Game module..." );

require( "Game.Node" )
require( "Game.Fixed" )
require( "Game.StaticFixed" )
require( "Game.TreeFixed" )

require( "Game.Entity" )
require( "Game.Player" )

-- entities
--require( "Game.Light.PointLight" )
--require( "Game.Light.SpotLight" )

require( "Game.Physics.Animated" )
require( "Game.Physics.Articulated" )
require( "Game.Physics.RigidBody" )

require( "Game.Effect.Particle" )
require( "Game.Effect.Sound" )
require( "Game.Effect.Light" )


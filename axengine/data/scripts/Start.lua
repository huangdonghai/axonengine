--[[
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
]]--

require( "Utils" )


-- global function
function AX_DECLARE_CLASS( me, super )
	print( "registering " .. me._NAME .. " class" )
	Utils.merge( me, super, true )
	registerClass( me._NAME, super._NAME )
end

function AX_CREATE_OBJECT( cls )
	local result = {}
	result.Properties = {}
	
	-- create properties
	Utils.createProps( result.Properties, cls.Properties, true )
	
	-- set methods
	Utils.mergeFunctions( result, cls )
	
	return result
	
--	return Utils.cloneTable( cls )
end

-- define namespace
Game = {}
Ui = {}

-- load common utils
require( "Utils" );

-- load class
require( "Object" );
require( "Game" );
require( "Physics" );
-- require( "Ui" );
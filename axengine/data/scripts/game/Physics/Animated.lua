--[[
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
]]--

module( ..., package.seeall );

Properties = {
	active = true,
	int_loopCount = -1,	-- -1 for loop forever
	Asset = {
		mdl_model="models/characters/_male/fullmale.mesh",
		mdl_rig="",
		anim_animation="models/characters/_male/locomotion/male/mtfastforward.anim",
	},
	Options = {
		oneAreaOnly = true,
	},
}

m_table = {}

function onThink( self,frametime )
--	print( self.objectName, "onThink", frametime )
end

function onReset(self)
	print( self.objectName, "onReset" )
	
	local props = self.Properties;
	
	m_table.model = props.Asset.mdl_model;
	m_table.rig = props.Asset.mdl_rig;
	m_table.anim = props.Asset.anim_animation;
	m_table.loopCount = props.loopCount;
	
	-- load light
	self:loadAsset(m_table);
end

AX_DECLARE_CLASS(_M, "Animated")

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
	Asset = {
		mdl_model="models/box.mesh",
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
	m_table.rigid = props.Asset.mdl_rigid;
	
	-- load asset
	self:loadAsset(m_table);
end

AX_DECLARE_CLASS( _M, Game.Entity )

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
	Style = {
		snd_sound = "",
		minDist = 0,
		maxDist = 64,
		looping = false,
		interval = 0
	},
}

m_table = {}

function onReset(self)
	print( self.objectName, "onReset" )
	
	local props = self.Properties;
	
	m_table.sound = props.Style.snd_sound;
	m_table.minDist = props.Style.minDist;
	m_table.maxDist = props.Style.maxDist;
	m_table.looping = props.Style.looping;
	m_table.interval = props.Style.interval;
	
	-- load sound
	self:loadSound(m_table);
	
	-- active it
	self:activeSound(props.active);
end


AX_DECLARE_CLASS( _M, Game.Entity )

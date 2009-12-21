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
	radius = 10,
	Style = {
		glare = 0,
		glareScale = 1,
		enum_lightStyle = { PointLight=2, SpotLight=3, _default=2 },
		castShadowMap = false,
		enum_shadowMapSize = { {"128", 128}, {"256", 256}, {"512", 512},
							   {"1024", 1024}, {"2048", 2048}, _default=256 },
		spotAngle = 60,		-- if == 0, is point light
	},
	Color = {
		color = { r=1,g=1,b=1 },
		intensity = 1,
		specularX = 1,
		hdrStops = 0,		-- -1=darker..0=normal..1=brighter
	},
	Options = {
		oneAreaOnly = true,
		mdl_model="",
		tex_texture = "",
	},
}

Editor = {
	icon="light"
}

m_lt = {}

function onInit(self)
	print(self.objectName, "onInit")
end

function onFinalize(self)
	print(self.objectName, "onFinalize")
end

function onThink( self,frametime )
--	print(self.objectName, "onThink", frametime)
end

function onReset(self)
	print(self.objectName, "onReset")
	
	local props = self.Properties;
	
	m_lt.radius = props.radius;
	m_lt.type = props.Style.enum_lightStyle;
	m_lt.color = props.Color.color;
	m_lt.intensity = props.Color.intensity;
	m_lt.specularX = props.Color.specularX;
	m_lt.castShadowMap = props.Style.castShadowMap;
	m_lt.spotAngle = props.Style.spotAngle;
	m_lt.shadowMapSize = props.Style.enum_shadowMapSize;
	
	-- load light
	self:loadLight(m_lt);
	
	-- active it
	self:activeLight(props.active);
end

AX_DECLARE_CLASS(_M, Game.Entity)

--------------------------------------------------------------------------------
-- signals and slots
--------------------------------------------------------------------------------
SigSlot = {
	Signals = {
		actived = "bool",
	},

	Slots = {
		active = { slot_active,"bool" },
		enable = { slot_enable,"bool" },
		disable = { slot_disable,"bool" },
	},
}

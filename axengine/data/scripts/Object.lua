--[[
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
]]--

module( ..., package.seeall )

print( "registering " .. _NAME .. " class" )
registerClass( _NAME )

--[[
Object Properties type prefix

[bool]		-- optional
int
[float]		-- optional
mdl			-- model
tex			-- texture
mtr			-- material
anim		-- animation
spt			-- speedtree
snd			-- sound file
enum		-- enumation integer
flag		-- bitflag

[str]		-- opetion
[vec]		-- vector3, if have x,y,z field, then is vec
[color]		-- color, if have r,g,b field, then is color
[point]		-- point, if have x,y field, then is point
[rect]		-- rect, if have x,y,width,height field, then is rect

Object call backs

onInit
onPropertyChanged
onFinalize

]]--
-- some class info
objNameSpace = ""

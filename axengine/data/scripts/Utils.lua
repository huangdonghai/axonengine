--[[
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
]]--

module( ..., package.seeall )

print( "registering " .. ... .. " class" )

-- utility function

function setSuper(child, super)
	child.__index = super
	setmetatable( child, child )
	child._SUPER = super
end


function count(_tbl)
	local count = 0;
	if (_tbl) then
		for i,v in pairs(_tbl) do
			count = count+1;
		end
	end	
	return count;
end

function dumpTable(tbl)
	if (type(tbl) ~= "table") then
		return
	end
	for i, v in pairs(tbl) do
		print( i, v )
	end
end

-- clone a table
function cloneTable(_obj, norecurse)
	if (type(_obj) == "table") then
		local _newobj = {};
		if (norecurse) then
			for i,f in pairs(_obj) do
				_newobj[i] = f;
			end
		else
			for i,f in pairs(_obj) do
				if ((type(f) == "table") and (_obj~=f)) then -- avoid recursing into itself
					_newobj[i] = cloneTable(f);
				else
					_newobj[i] = f;
				end
			end
		end
		return _newobj;
	else
		return _obj;
	end
end

-- merge two table, but don't merge function
function mergeWithoutFunction(dst, src, recurse)
	for i,v in pairs(src) do
		if (type(v) ~= "function") then
			if(recurse) then
				if((type(v) == "table") and (v ~= src))then  -- avoid recursing into itself
					if (not dst[i]) then
						dst[i] = {};
					end
					mergeWithoutFunction(dst[i], v, recurse);
				elseif (not dst[i]) then
					dst[i] = v;
				end
			elseif (not dst[i]) then
				dst[i] = v;
			end
		end
	end
	
	return dst;
end

-- merge function
function mergeFunctions( dst, src )
	for i,v in pairs(src) do
		if (type(v) == "function") then
			dst[i] = v
		end
	end
	
	return dst;
end

-- merge two table, include function
function merge(dst, src, recursive)
	for i,v in pairs(src) do
		if (recursive) then
			if((type(v) == "table") and (v ~= src))then  -- avoid recursing into itself
				if (not dst[i]) then
					dst[i] = {};
				end
				merge(dst[i], v, recursive);
			elseif (not dst[i]) then
				dst[i] = v;
			end
		elseif (not dst[i]) then
			dst[i] = v;
		end
	end
	
	return dst;
end

-- create properties
function createProps(dst, src, recursive)
	if (type(src) ~= "table") then
		return dst;
	end
	for i,v in pairs(src) do
		if (recursive) then
			if ((type(v) == "table") and (v ~= src)) then  -- avoid recursing into itself
				if (v._default ~= nil) then
					dst[i] = v._default
				else
					if (not dst[i]) then
						dst[i] = {};
					end
					createProps(dst[i], v, true);
				end
			elseif (not dst[i]) then
				dst[i] = v;
			end
		elseif (not dst[i]) then
			dst[i] = v;
		end
	end
	
	return dst;
end

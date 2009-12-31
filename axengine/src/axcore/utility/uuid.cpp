/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

#pragma comment(lib, "Rpcrt4.lib")

AX_BEGIN_NAMESPACE

	Uuid::Uuid()
	{
		AX_STATIC_ASSERT(sizeof(Uuid)==sizeof(UUID));

		UuidCreate((UUID*)this);
	}

	Uuid &Uuid::fromString( const String &str )
	{
		UuidFromStringA((RPC_CSTR)str.c_str(), (UUID*)this);
		return *this;
	}

	String Uuid::toString()
	{
		RPC_CSTR str;

		UuidToStringA((UUID*)this, &str);

		String result = reinterpret_cast<char*>(str);

		RpcStringFreeA(&str);
		return result;
	}

	size_t Uuid::hash() const
	{
		const size_t *rep = reinterpret_cast<const size_t*>(this);
		size_t result = rep[0];

		hash_combine(result, rep[1]);
		hash_combine(result, rep[2]);
		hash_combine(result, rep[3]);

		return result;
	}

	String Uuid::generateUuid() {
		UUID uuid;
		UuidCreate(&uuid);

		RPC_CSTR str;

		UuidToStringA(&uuid, &str);

		String result = reinterpret_cast<char*>(str);

		RpcStringFreeA(&str);
		return result;
	}


AX_END_NAMESPACE


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

	RPC_STATUS status = UuidCreateNil((UUID*)this);
}

Uuid &Uuid::fromString(const String &str)
{
	RPC_STATUS status = UuidFromStringA((RPC_CSTR)str.c_str(), (UUID*)this);
	AX_ASSERT(status == RPC_S_OK);
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
	const uint_t *rep = reinterpret_cast<const uint_t*>(this);
	size_t result = rep[0];

	hash_combine(result, rep[1]);
	hash_combine(result, rep[2]);
	hash_combine(result, rep[3]);

	return result;
}

Uuid Uuid::generateUuid()
{
	Uuid result;
	RPC_STATUS status = UuidCreate((UUID*)&result);

	AX_ASSERT(status == RPC_S_OK);

	return result;
}

struct UuidItem {
	int ref;
	Uuid uuid;
};

template <class T>
class FixedCache : ThreadSafe
{
	typedef const T * KeyType;

	struct hashPtr {
		size_t operator()(const KeyType &p) const
		{
			return std::tr1::hash<T>()(*p);
		}
	};
	struct equalPtr{
		bool operator()(const KeyType &a, const KeyType &b) const
		{
			return *a == *b;
		}
	};

	class Item {
	public:
		AtomicInt m_ref;
		T m_value;
	};

public:
	FixedCache()
	{
		m_nullHandle = find(T());
	}

	~FixedCache()
	{
		// TODO
	}

	// m_ref added
	handle_t find(const T &t)
	{
		SCOPE_LOCK;

		const T *ptr = &t;
		DataType::iterator it = m_data.find(ptr);
		if (it != m_data.end()) {
			Item *handle = it->second;
			handle->m_ref.incref();
			return handle;
		}

		Item *item = new Item();
		item->m_ref.incref();
		item->m_value = t;
		m_data[&item->m_value] = item;

		return item;
	}

	void incref(handle_t h)
	{
		Item *item = reinterpret_cast<Item *>(h);
		item->m_ref.incref();
	}

	void decref(handle_t h)
	{
		Item *item = reinterpret_cast<Item *>(h);

		if (item->m_ref.decref() == 0) {
			SCOPE_LOCK;
			m_data.erase(&item->m_value);
			delete item;
		}
	}

	const T &getValue(handle_t h)
	{
		Item *item = reinterpret_cast<Item *>(h);
		return item->m_value;
	}

public:
	handle_t m_nullHandle;
	typedef Dict<KeyType, Item*, hashPtr, equalPtr> DataType;
	DataType m_data;
};

FixedCache<Uuid> s_uuidCache;


FixedUuid::FixedUuid()
{
	m_handle = s_uuidCache.m_nullHandle;
}

FixedUuid::FixedUuid( const Uuid &rhs )
{
	m_handle = s_uuidCache.find(rhs);
}

FixedUuid::FixedUuid(const FixedUuid &rhs)
{
	s_uuidCache.incref(rhs.m_handle);
	m_handle = rhs.m_handle;
}

FixedUuid::~FixedUuid()
{
	s_uuidCache.decref(m_handle);
}

FixedUuid & FixedUuid::operator=( const FixedUuid &rhs )
{
	s_uuidCache.decref(m_handle);
	s_uuidCache.incref(rhs.m_handle);
	m_handle = rhs.m_handle;
	return *this;
}

const Uuid & FixedUuid::toUuid() const
{
	return s_uuidCache.getValue(m_handle);
}

AX_END_NAMESPACE


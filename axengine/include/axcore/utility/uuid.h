/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_UUID_H
#define AX_UUID_H

AX_BEGIN_NAMESPACE

struct AX_API Uuid
{
public:
	/* construction & destruction */
	Uuid();

	bool operator==(const Uuid &rhs) const
	{
		return m_dwords[0] == rhs.m_dwords[0] &&
			m_dwords[1] == rhs.m_dwords[1] &&
			m_dwords[2] == rhs.m_dwords[2] &&
			m_dwords[3] == rhs.m_dwords[3];
	}

	Uuid &fromString(const std::string &str);
	std::string toString();

	size_t hash() const;

	static Uuid generateUuid();

private:
	union {
		struct {
			uint_t m_timeLow;					/* bits  0-31 of time field */
			ushort_t m_timeMid;					/* bits 32-47 of time field */
			ushort_t m_timeHiAndVersion;		/* bits 48-59 of time field plus 4 bit version */
			byte_t m_clockSeqHiAndReserved;		/* bits  8-13 of clock sequence field plus 2 bit variant */
			byte_t m_clockSeqLow;				/* bits  0-7  of clock sequence field */
			byte_t m_node[6];
		};
		uint_t m_dwords[4];
	};
};

class AX_API FixedUuid
{
public:
	FixedUuid();
	FixedUuid(const Uuid &rhs);
	FixedUuid(const FixedUuid &rhs);
	~FixedUuid();

	FixedUuid &operator=(const FixedUuid &rhs);
	bool operator==(const FixedUuid &rhs) const { return m_handle == rhs.m_handle; }

	const Uuid &toUuid() const;
	operator const Uuid &() const { return toUuid(); }

	size_t hash() const
	{
		return m_handle.toInt();
	}

private:
	Handle m_handle;
};

AX_END_NAMESPACE

AX_DECLARE_HASH_FUNCTION(Uuid);
AX_DECLARE_HASH_FUNCTION(FixedUuid);

#endif // AX_UUID_H

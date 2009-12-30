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

	class AX_API Uuid {
	public:
		/* construction & destruction */
		Uuid();

		Uuid& fromString(const String& str);
		String toString();

		size_t hash() const;

		static String generateUuid();

	private:
		uint_t m_timeLow;					/* bits  0-31 of time field */
		ushort_t m_timeMid;					/* bits 32-47 of time field */
		ushort_t m_timeHiAndVersion;		/* bits 48-59 of time field plus 4 bit version */
		byte_t m_clockSeqHiAndReserved;		/* bits  8-13 of clock sequence field plus 2 bit variant */
		byte_t m_clockSeqLow;				/* bits  0-7  of clock sequence field */
		byte_t m_node[6];
	};

AX_END_NAMESPACE

#endif // AX_UUID_H

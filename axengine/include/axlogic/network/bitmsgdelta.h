/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_NETWORK_BITMSGDELTA_H
#define AX_NETWORK_BITMSGDELTA_H

AX_BEGIN_NAMESPACE

	class BitMsgDelta {
	public:
		BitMsgDelta();
		~BitMsgDelta() {}

		void init( const BitMsg *base, BitMsg *newBase, BitMsg *delta );
		void init( const BitMsg *base, BitMsg *newBase, const BitMsg *delta );
		bool hasChanged( void ) const;

		void writeBits( int value, int numBits );
		void writeChar( int c );
		void writeByte( int c );
		void writeShort( int c );
		void writeUShort( int c );
		void writeLong( int c );
		void writeFloat( float f );
		void writeFloat( float f, int exponentBits, int mantissaBits );
		void writeAngle8( float f );
		void writeAngle16( float f );
		void writeDir( const Vector3 &dir, int numBits );
		void writeString( const char *s, int maxLength = -1 );
		void writeData( const void *data, int length );
		void writeDict( const StringDict &dict );
			 
		void writeDeltaChar( int oldValue, int newValue );
		void writeDeltaByte( int oldValue, int newValue );
		void writeDeltaShort( int oldValue, int newValue );
		void writeDeltaLong( int oldValue, int newValue );
		void writeDeltaFloat( float oldValue, float newValue );
		void writeDeltaFloat( float oldValue, float newValue, int exponentBits, int mantissaBits );
		void writeDeltaByteCounter( int oldValue, int newValue );
		void writeDeltaShortCounter( int oldValue, int newValue );
		void writeDeltaLongCounter( int oldValue, int newValue );

		int readBits( int numBits ) const;
		int readChar( void ) const;
		int readByte( void ) const;
		int readShort( void ) const;
		int readUShort( void ) const;
		int readLong( void ) const;
		float readFloat( void ) const;
		float readFloat( int exponentBits, int mantissaBits ) const;
		float readAngle8( void ) const;
		float readAngle16( void ) const;
		Vector3 readDir( int numBits ) const;
		void readString( char *buffer, int bufferSize ) const;
		void readData( void *data, int length ) const;
		void readDict( StringDict &dict );

		int readDeltaChar( int oldValue ) const;
		int readDeltaByte( int oldValue ) const;
		int readDeltaShort( int oldValue ) const;
		int readDeltaLong( int oldValue ) const;
		float readDeltaFloat( float oldValue ) const;
		float readDeltaFloat( float oldValue, int exponentBits, int mantissaBits ) const;
		int readDeltaByteCounter( int oldValue ) const;
		int readDeltaShortCounter( int oldValue ) const;
		int readDeltaLongCounter( int oldValue ) const;

	private:
		const BitMsg *m_base;			// base
		BitMsg *m_newBase;		// new base
		BitMsg *m_writeDelta;		// delta from base to new base for writing
		const BitMsg *m_readDelta;		// delta from base to new base for reading
		mutable bool m_changed;		// true if the new base is different from the base

	private:
		void writeDelta( int oldValue, int newValue, int numBits );
		int readDelta( int oldValue, int numBits ) const;
	};

	inline BitMsgDelta::BitMsgDelta() {
		m_base = NULL;
		m_newBase = NULL;
		m_writeDelta = NULL;
		m_readDelta = NULL;
		m_changed = false;
	}

	inline void BitMsgDelta::init( const BitMsg *base, BitMsg *newBase, BitMsg *delta ) {
		this->m_base = base;
		this->m_newBase = newBase;
		this->m_writeDelta = delta;
		this->m_readDelta = delta;
		this->m_changed = false;
	}

	inline void BitMsgDelta::init( const BitMsg *base, BitMsg *newBase, const BitMsg *delta ) {
		this->m_base = base;
		this->m_newBase = newBase;
		this->m_writeDelta = NULL;
		this->m_readDelta = delta;
		this->m_changed = false;
	}

	inline bool BitMsgDelta::hasChanged( void ) const {
		return m_changed;
	}

	inline void BitMsgDelta::writeChar( int c ) {
		writeBits( c, -8 );
	}

	inline void BitMsgDelta::writeByte( int c ) {
		writeBits( c, 8 );
	}

	inline void BitMsgDelta::writeShort( int c ) {
		writeBits( c, -16 );
	}

	inline void BitMsgDelta::writeUShort( int c ) {
		writeBits( c, 16 );
	}

	inline void BitMsgDelta::writeLong( int c ) {
		writeBits( c, 32 );
	}

	inline void BitMsgDelta::writeFloat( float f ) {
		writeBits( *reinterpret_cast<int *>(&f), 32 );
	}

	inline void BitMsgDelta::writeFloat( float f, int exponentBits, int mantissaBits ) {
		int bits = Math::floatToBits( f, exponentBits, mantissaBits );
		writeBits( bits, 1 + exponentBits + mantissaBits );
	}

	inline void BitMsgDelta::writeAngle8( float f ) {
		writeBits( Math::angleToByte( f ), 8 );
	}

	inline void BitMsgDelta::writeAngle16( float f ) {
		writeBits( Math::angleToShort(f), 16 );
	}

	inline void BitMsgDelta::writeDir( const Vector3 &dir, int numBits ) {
		writeBits( BitMsg::dirToBits( dir, numBits ), numBits );
	}

	inline void BitMsgDelta::writeDeltaChar( int oldValue, int newValue ) {
		writeDelta( oldValue, newValue, -8 );
	}

	inline void BitMsgDelta::writeDeltaByte( int oldValue, int newValue ) {
		writeDelta( oldValue, newValue, 8 );
	}

	inline void BitMsgDelta::writeDeltaShort( int oldValue, int newValue ) {
		writeDelta( oldValue, newValue, -16 );
	}

	inline void BitMsgDelta::writeDeltaLong( int oldValue, int newValue ) {
		writeDelta( oldValue, newValue, 32 );
	}

	inline void BitMsgDelta::writeDeltaFloat( float oldValue, float newValue ) {
		writeDelta( *reinterpret_cast<int *>(&oldValue), *reinterpret_cast<int *>(&newValue), 32 );
	}

	inline void BitMsgDelta::writeDeltaFloat( float oldValue, float newValue, int exponentBits, int mantissaBits ) {
		int oldBits = Math::floatToBits( oldValue, exponentBits, mantissaBits );
		int newBits = Math::floatToBits( newValue, exponentBits, mantissaBits );
		writeDelta( oldBits, newBits, 1 + exponentBits + mantissaBits );
	}

	inline int BitMsgDelta::readChar( void ) const {
		return (signed char)readBits( -8 );
	}

	inline int BitMsgDelta::readByte( void ) const {
		return (unsigned char)readBits( 8 );
	}

	inline int BitMsgDelta::readShort( void ) const {
		return (short)readBits( -16 );
	}

	inline int BitMsgDelta::readUShort( void ) const {
		return (unsigned short)readBits( 16 );
	}

	inline int BitMsgDelta::readLong( void ) const {
		return readBits( 32 );
	}

	inline float BitMsgDelta::readFloat( void ) const {
		float value;
		*reinterpret_cast<int *>(&value) = readBits( 32 );
		return value;
	}

	inline float BitMsgDelta::readFloat( int exponentBits, int mantissaBits ) const {
		int bits = readBits( 1 + exponentBits + mantissaBits );
		return Math::bitsToFloat( bits, exponentBits, mantissaBits );
	}

	inline float BitMsgDelta::readAngle8( void ) const {
		return Math::byteToAngle( readByte() );
	}

	inline float BitMsgDelta::readAngle16( void ) const {
		return Math::shortToAngle( readShort() );
	}

	inline Vector3 BitMsgDelta::readDir( int numBits ) const {
		return BitMsg::bitsToDir( readBits( numBits ), numBits );
	}

	inline int BitMsgDelta::readDeltaChar( int oldValue ) const {
		return (signed char)readDelta( oldValue, -8 );
	}

	inline int BitMsgDelta::readDeltaByte( int oldValue ) const {
		return (unsigned char)readDelta( oldValue, 8 );
	}

	inline int BitMsgDelta::readDeltaShort( int oldValue ) const {
		return (short)readDelta( oldValue, -16 );
	}

	inline int BitMsgDelta::readDeltaLong( int oldValue ) const {
		return readDelta( oldValue, 32 );
	}

	inline float BitMsgDelta::readDeltaFloat( float oldValue ) const {
		float value;
		*reinterpret_cast<int *>(&value) = readDelta( *reinterpret_cast<int *>(&oldValue), 32 );
		return value;
	}

	inline float BitMsgDelta::readDeltaFloat( float oldValue, int exponentBits, int mantissaBits ) const {
		int oldBits = Math::floatToBits( oldValue, exponentBits, mantissaBits );
		int newBits = readDelta( oldBits, 1 + exponentBits + mantissaBits );
		return Math::bitsToFloat( newBits, exponentBits, mantissaBits );
	}

AX_END_NAMESPACE

#endif // end guardian

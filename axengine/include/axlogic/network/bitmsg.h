/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_NETWORK_BITMSG_H
#define AX_NETWORK_BITMSG_H

AX_BEGIN_NAMESPACE
	typedef Dict<String, String> StringDict;

	// network address
	struct NetAdr {
		byte_t ip[4];
		ushort_t port;
	};

	class BitMsg {
	public:
		BitMsg();
		~BitMsg() {}

		void init( byte_t* data, int length );
		void init( const byte_t* data, int length );
		byte_t* getData( void );
		const byte_t* getData( void ) const;
		int getMaxSize( void ) const;
		void setAllowOverflow( bool set );
		bool isOverflowed( void ) const;

		int getSize( void ) const;
		void setSize( int size );
		int getWriteBit( void ) const;
		void setWriteBit( int bit );
		int getNumBitsWritten( void ) const;
		int getRemainingWriteBits( void ) const;
		void saveWriteState( int &s, int &b ) const;
		void restoreWriteState( int s, int b );

		int getReadCount( void ) const;
		void setReadCount( int bytes );
		int getReadBit( void ) const;
		void setReadBit( int bit );
		int getNumBitsRead( void ) const;
		int getRemainingReadBits( void ) const;
		void saveReadState( int &c, int &b ) const;
		void restoreReadState( int c, int b );

		void beginWriting( void );
		int getRemainingSpace( void ) const;
		void writeByteAlign( void );
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
		void writeString( const char *s, int maxLength = -1, bool make7Bit = true );
		void writeData( const void *data, int length );
		void writeNetadr( const NetAdr& adr );

		void writeDeltaChar( int oldValue, int newValue );
		void writeDeltaByte( int oldValue, int newValue );
		void writeDeltaShort( int oldValue, int newValue );
		void writeDeltaLong( int oldValue, int newValue );
		void writeDeltaFloat( float oldValue, float newValue );
		void writeDeltaFloat( float oldValue, float newValue, int exponentBits, int mantissaBits );
		void writeDeltaByteCounter( int oldValue, int newValue );
		void writeDeltaShortCounter( int oldValue, int newValue );
		void writeDeltaLongCounter( int oldValue, int newValue );
		bool writeDeltaDict( const StringDict &dict, const StringDict *base );

		void beginReading( void ) const;
		int getRemaingData( void ) const;
		void readByteAlign( void ) const;
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
		int readString( char *buffer, int bufferSize ) const;
		int readData( void *data, int length ) const;
		void readNetadr( NetAdr *adr ) const;

		int readDeltaChar( int oldValue ) const;
		int readDeltaByte( int oldValue ) const;
		int readDeltaShort( int oldValue ) const;
		int readDeltaLong( int oldValue ) const;
		float readDeltaFloat( float oldValue ) const;
		float readDeltaFloat( float oldValue, int exponentBits, int mantissaBits ) const;
		int readDeltaByteCounter( int oldValue ) const;
		int readDeltaShortCounter( int oldValue ) const;
		int readDeltaLongCounter( int oldValue ) const;
		bool readDeltaDict( StringDict &dict, const StringDict *base ) const;

		static int dirToBits( const Vector3 &dir, int numBits );
		static Vector3 bitsToDir( int bits, int numBits );

	private:
		byte_t* m_writeData;
		const byte_t* m_readData;
		int m_maxSize;
		int m_curSize;
		int m_writeBit;
		mutable int m_readCount;
		mutable int m_readBit;
		bool m_allowOverflow;
		bool m_overflowed;

	private:
		bool checkOverflow( int numBits );
		byte_t * getByteSpace( int length );
		void writeDelta( int oldValue, int newValue, int numBits );
		int readDelta( int oldValue, int numBits ) const;
	};

	inline void BitMsg::init( byte_t *data, int length ) {
		m_writeData = data;
		m_readData = data;
		m_maxSize = length;
	}

	inline void BitMsg::init( const byte_t *data, int length ) {
		m_writeData = NULL;
		m_readData = data;
		m_maxSize = length;
	}

	inline byte_t *BitMsg::getData( void ) {
		return m_writeData;
	}

	inline const byte_t *BitMsg::getData( void ) const {
		return m_readData;
	}

	inline int BitMsg::getMaxSize( void ) const {
		return m_maxSize;
	}

	inline void BitMsg::setAllowOverflow( bool set ) {
		m_allowOverflow = set;
	}

	inline bool BitMsg::isOverflowed( void ) const {
		return m_overflowed;
	}

	inline int BitMsg::getSize( void ) const {
		return m_curSize;
	}

	inline void BitMsg::setSize( int size ) {
		if ( size > m_maxSize ) {
			m_curSize = m_maxSize;
		} else {
			m_curSize = size;
		}
	}

	inline int BitMsg::getWriteBit( void ) const {
		return m_writeBit;
	}

	inline void BitMsg::setWriteBit( int bit ) {
		m_writeBit = bit & 7;
		if ( m_writeBit ) {
			m_writeData[m_curSize - 1] &= ( 1 << m_writeBit ) - 1;
		}
	}

	inline int BitMsg::getNumBitsWritten( void ) const {
		return ( ( m_curSize << 3 ) - ( ( 8 - m_writeBit ) & 7 ) );
	}

	inline int BitMsg::getRemainingWriteBits( void ) const {
		return ( m_maxSize << 3 ) - getNumBitsWritten();
	}

	inline void BitMsg::saveWriteState( int &s, int &b ) const {
		s = m_curSize;
		b = m_writeBit;
	}

	inline void BitMsg::restoreWriteState( int s, int b ) {
		m_curSize = s;
		m_writeBit = b & 7;
		if ( m_writeBit ) {
			m_writeData[m_curSize - 1] &= ( 1 << m_writeBit ) - 1;
		}
	}

	inline int BitMsg::getReadCount( void ) const {
		return m_readCount;
	}

	inline void BitMsg::setReadCount( int bytes ) {
		m_readCount = bytes;
	}

	inline int BitMsg::getReadBit( void ) const {
		return m_readBit;
	}

	inline void BitMsg::setReadBit( int bit ) {
		m_readBit = bit & 7;
	}

	inline int BitMsg::getNumBitsRead( void ) const {
		return ( ( m_readCount << 3 ) - ( ( 8 - m_readBit ) & 7 ) );
	}

	inline int BitMsg::getRemainingReadBits( void ) const {
		return ( m_curSize << 3 ) - getNumBitsRead();
	}

	inline void BitMsg::saveReadState( int &c, int &b ) const {
		c = m_readCount;
		b = m_readBit;
	}

	inline void BitMsg::restoreReadState( int c, int b ) {
		m_readCount = c;
		m_readBit = b & 7;
	}

	inline void BitMsg::beginWriting( void ) {
		m_curSize = 0;
		m_overflowed = false;
		m_writeBit = 0;
	}

	inline int BitMsg::getRemainingSpace( void ) const {
		return m_maxSize - m_curSize;
	}

	inline void BitMsg::writeByteAlign( void ) {
		m_writeBit = 0;
	}

	inline void BitMsg::writeChar( int c ) {
		writeBits( c, -8 );
	}

	inline void BitMsg::writeByte( int c ) {
		writeBits( c, 8 );
	}

	inline void BitMsg::writeShort( int c ) {
		writeBits( c, -16 );
	}

	inline void BitMsg::writeUShort( int c ) {
		writeBits( c, 16 );
	}

	inline void BitMsg::writeLong( int c ) {
		writeBits( c, 32 );
	}

	inline void BitMsg::writeFloat( float f ) {
		writeBits( *reinterpret_cast<int *>(&f), 32 );
	}

	inline void BitMsg::writeFloat( float f, int exponentBits, int mantissaBits ) {
		int bits = Math::floatToBits( f, exponentBits, mantissaBits );
		writeBits( bits, 1 + exponentBits + mantissaBits );
	}

	inline void BitMsg::writeAngle8( float f ) {
		writeByte( Math::angleToByte( f ) );
	}

	inline void BitMsg::writeAngle16( float f ) {
		writeShort( Math::angleToShort(f) );
	}

	inline void BitMsg::writeDir( const Vector3 &dir, int numBits ) {
		writeBits( dirToBits( dir, numBits ), numBits );
	}

	inline void BitMsg::writeDeltaChar( int oldValue, int newValue ) {
		writeDelta( oldValue, newValue, -8 );
	}

	inline void BitMsg::writeDeltaByte( int oldValue, int newValue ) {
		writeDelta( oldValue, newValue, 8 );
	}

	inline void BitMsg::writeDeltaShort( int oldValue, int newValue ) {
		writeDelta( oldValue, newValue, -16 );
	}

	inline void BitMsg::writeDeltaLong( int oldValue, int newValue ) {
		writeDelta( oldValue, newValue, 32 );
	}

	inline void BitMsg::writeDeltaFloat( float oldValue, float newValue ) {
		writeDelta( *reinterpret_cast<int *>(&oldValue), *reinterpret_cast<int *>(&newValue), 32 );
	}

	inline void BitMsg::writeDeltaFloat( float oldValue, float newValue, int exponentBits, int mantissaBits ) {
		int oldBits = Math::floatToBits( oldValue, exponentBits, mantissaBits );
		int newBits = Math::floatToBits( newValue, exponentBits, mantissaBits );
		writeDelta( oldBits, newBits, 1 + exponentBits + mantissaBits );
	}

	inline void BitMsg::beginReading( void ) const {
		m_readCount = 0;
		m_readBit = 0;
	}

	inline int BitMsg::getRemaingData( void ) const {
		return m_curSize - m_readCount;
	}

	inline void BitMsg::readByteAlign( void ) const {
		m_readBit = 0;
	}

	inline int BitMsg::readChar( void ) const {
		return (signed char)readBits( -8 );
	}

	inline int BitMsg::readByte( void ) const {
		return (unsigned char)readBits( 8 );
	}

	inline int BitMsg::readShort( void ) const {
		return (short)readBits( -16 );
	}

	inline int BitMsg::readUShort( void ) const {
		return (unsigned short)readBits( 16 );
	}

	inline int BitMsg::readLong( void ) const {
		return readBits( 32 );
	}

	inline float BitMsg::readFloat( void ) const {
		float value;
		*reinterpret_cast<int *>(&value) = readBits( 32 );
		return value;
	}

	inline float BitMsg::readFloat( int exponentBits, int mantissaBits ) const {
		int bits = readBits( 1 + exponentBits + mantissaBits );
		return Math::bitsToFloat( bits, exponentBits, mantissaBits );
	}

	inline float BitMsg::readAngle8( void ) const {
		return Math::byteToAngle( readByte() );
	}

	inline float BitMsg::readAngle16( void ) const {
		return Math::shortToAngle( readShort() );
	}

	inline Vector3 BitMsg::readDir( int numBits ) const {
		return bitsToDir( readBits( numBits ), numBits );
	}

	inline int BitMsg::readDeltaChar( int oldValue ) const {
		return (signed char)readDelta( oldValue, -8 );
	}

	inline int BitMsg::readDeltaByte( int oldValue ) const {
		return (unsigned char)readDelta( oldValue, 8 );
	}

	inline int BitMsg::readDeltaShort( int oldValue ) const {
		return (short)readDelta( oldValue, -16 );
	}

	inline int BitMsg::readDeltaLong( int oldValue ) const {
		return readDelta( oldValue, 32 );
	}

	inline float BitMsg::readDeltaFloat( float oldValue ) const {
		float value;
		*reinterpret_cast<int *>(&value) = readDelta( *reinterpret_cast<int *>(&oldValue), 32 );
		return value;
	}

	inline float BitMsg::readDeltaFloat( float oldValue, int exponentBits, int mantissaBits ) const {
		int oldBits = Math::floatToBits( oldValue, exponentBits, mantissaBits );
		int newBits = readDelta( oldBits, 1 + exponentBits + mantissaBits );
		return Math::bitsToFloat( newBits, exponentBits, mantissaBits );
	}

AX_END_NAMESPACE

#endif // end guardian

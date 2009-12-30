/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

	BitMsg::BitMsg() {
		m_writeData = NULL;
		m_readData = NULL;
		m_maxSize = 0;
		m_curSize = 0;
		m_writeBit = 0;
		m_readCount = 0;
		m_readBit = 0;
		m_allowOverflow = false;
		m_overflowed = false;
	}

	bool BitMsg::checkOverflow(int numBits) {
		assert(numBits >= 0);
		if (numBits > getRemainingWriteBits()) {
			if (!m_allowOverflow) {
				Errorf("BitMsg: overflow without allowOverflow set");
			}
			if (numBits > (m_maxSize << 3)) {
				Errorf("BitMsg: %i bits is > full message size", numBits);
			}
			Printf("BitMsg: overflow\n");
			beginWriting();
			m_overflowed = true;
			return true;
		}
		return false;
	}

	byte_t *BitMsg::getByteSpace(int length) {
		byte_t *ptr;

		if (!m_writeData) {
			Errorf("BitMsg::getByteSpace: cannot write to message");
		}

		// round up to the next byte_t
		writeByteAlign();

		// check for overflow
		checkOverflow(length << 3);

		ptr = m_writeData + m_curSize;
		m_curSize += length;
		return ptr;
	}

	void BitMsg::writeBits(int value, int numBits) {
		int put;
		int fraction;

		if (!m_writeData) {
			Errorf("BitMsg::WriteBits: cannot write to message");
		}

		// check if the number of bits is valid
		if (numBits == 0 || numBits < -31 || numBits > 32) {
			Errorf("BitMsg::WriteBits: bad numBits %i", numBits);
		}

		// check for value overflows
		// this should be an error really, as it can go unnoticed and cause either bandwidth or corrupted data transmitted
		if (numBits != 32) {
			if (numBits > 0) {
				if (value > (1 << numBits) - 1) {
					Debugf("BitMsg::WriteBits: value overflow %d %d", value, numBits);
				} else if (value < 0) {
					Debugf("BitMsg::WriteBits: value overflow %d %d", value, numBits);
				}
			} else {
				int r = 1 << (- 1 - numBits);
				if (value > r - 1) {
					Debugf("BitMsg::WriteBits: value overflow %d %d", value, numBits);
				} else if (value < -r) {
					Debugf("BitMsg::WriteBits: value overflow %d %d", value, numBits);
				}
			}
		}

		if (numBits < 0) {
			numBits = -numBits;
		}

		// check for msg overflow
		if (checkOverflow(numBits)) {
			return;
		}

		// write the bits
		while (numBits) {
			if (m_writeBit == 0) {
				m_writeData[m_curSize] = 0;
				m_curSize++;
			}
			put = 8 - m_writeBit;
			if (put > numBits) {
				put = numBits;
			}
			fraction = value & ((1 << put) - 1);
			m_writeData[m_curSize - 1] |= fraction << m_writeBit;
			numBits -= put;
			value >>= put;
			m_writeBit = (m_writeBit + put) & 7;
		}
	}

	void BitMsg::writeString(const char *s, int maxLength, bool make7Bit) {
		if (!s) {
			writeData("", 1);
		} else {
			int i, l;
			byte_t *dataPtr;
			const byte_t *bytePtr;

			l = strlen(s);
			if (maxLength >= 0 && l >= maxLength) {
				l = maxLength - 1;
			}
			dataPtr = getByteSpace(l + 1);
			bytePtr = reinterpret_cast<const byte_t *>(s);
			if (make7Bit) {
				for (i = 0; i < l; i++) {
					if (bytePtr[i] > 127) {
						dataPtr[i] = '.';
					} else {
						dataPtr[i] = bytePtr[i];
					}
				}
			} else {
				for (i = 0; i < l; i++) {
					dataPtr[i] = bytePtr[i];
				}
			}
			dataPtr[i] = '\0';
		}
	}

	void BitMsg::writeData(const void *data, int length) {
		memcpy(getByteSpace(length), data, length);
	}

	void BitMsg::writeNetadr(const NetAdr& adr) {
		byte_t *dataPtr;
		dataPtr = getByteSpace(4);
		memcpy(dataPtr, adr.ip, 4);
		writeUShort(adr.port);
	}

	void BitMsg::writeDelta(int oldValue, int newValue, int numBits) {
		if (oldValue == newValue) {
			writeBits(0, 1);
			return;
		}
		writeBits(1, 1);
		writeBits(newValue, numBits);
	}

	void BitMsg::writeDeltaByteCounter(int oldValue, int newValue) {
		int i, x;

		x = oldValue ^ newValue;
		for (i = 7; i > 0; i--) {
			if (x & (1 << i)) {
				i++;
				break;
			}
		}
		writeBits(i, 3);
		if (i) {
			writeBits(((1 << i) - 1) & newValue, i);
		}
	}

	void BitMsg::writeDeltaShortCounter(int oldValue, int newValue) {
		int i, x;

		x = oldValue ^ newValue;
		for (i = 15; i > 0; i--) {
			if (x & (1 << i)) {
				i++;
				break;
			}
		}
		writeBits(i, 4);
		if (i) {
			writeBits(((1 << i) - 1) & newValue, i);
		}
	}

	void BitMsg::writeDeltaLongCounter(int oldValue, int newValue) {
		int i, x;

		x = oldValue ^ newValue;
		for (i = 31; i > 0; i--) {
			if (x & (1 << i)) {
				i++;
				break;
			}
		}
		writeBits(i, 5);
		if (i) {
			writeBits(((1 << i) - 1) & newValue, i);
		}
	}

	bool BitMsg::writeDeltaDict(const StringDict &dict, const StringDict *base) {
		return true;
	}

	int BitMsg::readBits(int numBits) const {
		int value;
		int valueBits;
		int get;
		int fraction;
		bool sgn;

		if (!m_readData) {
			Errorf("BitMsg::readBits: cannot read from message");
		}

		// check if the number of bits is valid
		if (numBits == 0 || numBits < -31 || numBits > 32) {
			Errorf("BitMsg::readBits: bad numBits %i", numBits);
		}

		value = 0;
		valueBits = 0;

		if (numBits < 0) {
			numBits = -numBits;
			sgn = true;
		} else {
			sgn = false;
		}

		// check for overflow
		if (numBits > getRemainingReadBits()) {
			return -1;
		}

		while (valueBits < numBits) {
			if (m_readBit == 0) {
				m_readCount++;
			}
			get = 8 - m_readBit;
			if (get > (numBits - valueBits)) {
				get = numBits - valueBits;
			}
			fraction = m_readData[m_readCount - 1];
			fraction >>= m_readBit;
			fraction &= (1 << get) - 1;
			value |= fraction << valueBits;

			valueBits += get;
			m_readBit = (m_readBit + get) & 7;
		}

		if (sgn) {
			if (value & (1 << (numBits - 1))) {
				value |= -1 ^ ((1 << numBits) - 1);
			}
		}

		return value;
	}

	int BitMsg::readString(char *buffer, int bufferSize) const {
		int l, c;

		readByteAlign();
		l = 0;
		while(1) {
			c = readByte();
			if (c <= 0 || c >= 255) {
				break;
			}
			// translate all fmt spec to avoid crash bugs in string routines
			if (c == '%') {
				c = '.';
			}

			// we will read past any excessively long string, so
			// the following data can be read, but the string will
			// be truncated
			if (l < bufferSize - 1) {
				buffer[l] = c;
				l++;
			}
		}

		buffer[l] = 0;
		return l;
	}

	int BitMsg::readData(void *data, int length) const {
		int cnt;

		readByteAlign();
		cnt = m_readCount;

		if (m_readCount + length > m_curSize) {
			if (data) {
				memcpy(data, m_readData + m_readCount, getRemaingData());
			}
			m_readCount = m_curSize;
		} else {
			if (data) {
				memcpy(data, m_readData + m_readCount, length);
			}
			m_readCount += length;
		}

		return (m_readCount - cnt);
	}

	void BitMsg::readNetadr(NetAdr *adr) const {
		int i;

		for (i = 0; i < 4; i++) {
			adr->ip[ i ] = readByte();
		}
		adr->port = readUShort();
	}

	int BitMsg::readDelta(int oldValue, int numBits) const {
		if (readBits(1)) {
			return readBits(numBits);
		}
		return oldValue;
	}

	int BitMsg::readDeltaByteCounter(int oldValue) const {
		int i, newValue;

		i = readBits(3);
		if (!i) {
			return oldValue;
		}
		newValue = readBits(i);
		return (oldValue & ~((1 << i) - 1) | newValue);
	}

	int BitMsg::readDeltaShortCounter(int oldValue) const {
		int i, newValue;

		i = readBits(4);
		if (!i) {
			return oldValue;
		}
		newValue = readBits(i);
		return (oldValue & ~((1 << i) - 1) | newValue);
	}

	int BitMsg::readDeltaLongCounter(int oldValue) const {
		int i, newValue;

		i = readBits(5);
		if (!i) {
			return oldValue;
		}
		newValue = readBits(i);
		return (oldValue & ~((1 << i) - 1) | newValue);
	}

	bool BitMsg::readDeltaDict(StringDict &dict, const StringDict *base) const {
		return true;
	}

	int BitMsg::dirToBits(const Vector3 &dir, int numBits) {
		int max, bits;
		float bias;

		assert(numBits >= 6 && numBits <= 32);
		assert(dir.getLengthSquared() - 1.0f < 0.01f);

		numBits /= 3;
		max = (1 << (numBits - 1)) - 1;
		bias = 0.5f / max;

		bits = FLOATSIGNBITSET(dir.x) << (numBits * 3 - 1);
		bits |= (int((Math::abs(dir.x) + bias) * max)) << (numBits * 2);
		bits |= FLOATSIGNBITSET(dir.y) << (numBits * 2 - 1);
		bits |= (int((Math::abs(dir.y) + bias) * max)) << (numBits * 1);
		bits |= FLOATSIGNBITSET(dir.z) << (numBits * 1 - 1);
		bits |= (int((Math::abs(dir.z) + bias) * max)) << (numBits * 0);
		return bits;
	}

	Vector3 BitMsg::bitsToDir(int bits, int numBits) {
		static float sign[2] = { 1.0f, -1.0f };
		int max;
		float invMax;
		Vector3 dir;

		assert(numBits >= 6 && numBits <= 32);

		numBits /= 3;
		max = (1 << (numBits - 1)) - 1;
		invMax = 1.0f / max;

		dir.x = sign[(bits >> (numBits * 3 - 1)) & 1] * ((bits >> (numBits * 2)) & max) * invMax;
		dir.y = sign[(bits >> (numBits * 2 - 1)) & 1] * ((bits >> (numBits * 1)) & max) * invMax;
		dir.z = sign[(bits >> (numBits * 1 - 1)) & 1] * ((bits >> (numBits * 0)) & max) * invMax;
		dir.normalize();
		return dir;
	}

AX_END_NAMESPACE

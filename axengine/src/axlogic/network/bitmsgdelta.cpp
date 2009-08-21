/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

namespace Axon { namespace Network {

	const int MAX_DATA_BUFFER = 1024;

	void BitMsgDelta::writeBits(int value, int numBits) {
		if (m_newBase) {
			m_newBase->writeBits(value, numBits);
		}

		if (!m_base) {
			m_writeDelta->writeBits(value, numBits);
			m_changed = true;
		} else {
			int baseValue = m_base->readBits(numBits);
			if (baseValue == value) {
				m_writeDelta->writeBits(0, 1);
			} else {
				m_writeDelta->writeBits(1, 1);
				m_writeDelta->writeBits(value, numBits);
				m_changed = true;
			}
		}
	}

	void BitMsgDelta::writeDelta(int oldValue, int newValue, int numBits) {
		if (m_newBase) {
			m_newBase->writeBits(newValue, numBits);
		}

		if (!m_base) {
			if (oldValue == newValue) {
				m_writeDelta->writeBits(0, 1);
			} else {
				m_writeDelta->writeBits(1, 1);
				m_writeDelta->writeBits(newValue, numBits);
			}
			m_changed = true;
		} else {
			int baseValue = m_base->readBits(numBits);
			if (baseValue == newValue) {
				m_writeDelta->writeBits(0, 1);
			} else {
				m_writeDelta->writeBits(1, 1);
				if (oldValue == newValue) {
					m_writeDelta->writeBits(0, 1);
					m_changed = true;
				} else {
					m_writeDelta->writeBits(1, 1);
					m_writeDelta->writeBits(newValue, numBits);
					m_changed = true;
				}
			}
		}
	}

	int BitMsgDelta::readBits(int numBits) const {
		int value;

		if (!m_base) {
			value = m_readDelta->readBits(numBits);
			m_changed = true;
		} else {
			int baseValue = m_base->readBits(numBits);
			if (!m_readDelta || m_readDelta->readBits(1) == 0) {
				value = baseValue;
			} else {
				value = m_readDelta->readBits(numBits);
				m_changed = true;
			}
		}

		if (m_newBase) {
			m_newBase->writeBits(value, numBits);
		}
		return value;
	}

	int BitMsgDelta::readDelta(int oldValue, int numBits) const {
		int value;

		if (!m_base) {
			if (m_readDelta->readBits(1) == 0) {
				value = oldValue;
			} else {
				value = m_readDelta->readBits(numBits);
			}
			m_changed = true;
		} else {
			int baseValue = m_base->readBits(numBits);
			if (!m_readDelta || m_readDelta->readBits(1) == 0) {
				value = baseValue;
			} else if (m_readDelta->readBits(1) == 0) {
				value = oldValue;
				m_changed = true;
			} else {
				value = m_readDelta->readBits(numBits);
				m_changed = true;
			}
		}

		if (m_newBase) {
			m_newBase->writeBits(value, numBits);
		}
		return value;
	}

	void BitMsgDelta::writeString(const char *s, int maxLength) {
		if (m_newBase) {
			m_newBase->writeString(s, maxLength);
		}

		if (!m_base) {
			m_writeDelta->writeString(s, maxLength);
			m_changed = true;
		} else {
			char baseString[MAX_DATA_BUFFER];
			m_base->readString(baseString, sizeof(baseString));
			if (strcmp(s, baseString) == 0) {
				m_writeDelta->writeBits(0, 1);
			} else {
				m_writeDelta->writeBits(1, 1);
				m_writeDelta->writeString(s, maxLength);
				m_changed = true;
			}
		}
	}

	void BitMsgDelta::writeData(const void *data, int length) {
		if (m_newBase) {
			m_newBase->writeData(data, length);
		}

		if (!m_base) {
			m_writeDelta->writeData(data, length);
			m_changed = true;
		} else {
			byte_t baseData[MAX_DATA_BUFFER];
			assert(length < sizeof(baseData));
			m_base->readData(baseData, length);
			if (memcmp(data, baseData, length) == 0) {
				m_writeDelta->writeBits(0, 1);
			} else {
				m_writeDelta->writeBits(1, 1);
				m_writeDelta->writeData(data, length);
				m_changed = true;
			}
		}
	}

	void BitMsgDelta::writeDict(const StringDict &dict) {
		if (m_newBase) {
			m_newBase->writeDeltaDict(dict, NULL);
		}

		if (!m_base) {
			m_writeDelta->writeDeltaDict(dict, NULL);
			m_changed = true;
		} else {
			StringDict baseDict;
			m_base->readDeltaDict(baseDict, NULL);
			m_changed = m_writeDelta->writeDeltaDict(dict, &baseDict);
		}
	}

	void BitMsgDelta::writeDeltaByteCounter(int oldValue, int newValue) {
		if (m_newBase) {
			m_newBase->writeBits(newValue, 8);
		}

		if (!m_base) {
			m_writeDelta->writeDeltaByteCounter(oldValue, newValue);
			m_changed = true;
		} else {
			int baseValue = m_base->readBits(8);
			if (baseValue == newValue) {
				m_writeDelta->writeBits(0, 1);
			} else {
				m_writeDelta->writeBits(1, 1);
				m_writeDelta->writeDeltaByteCounter(oldValue, newValue);
				m_changed = true;
			}
		}
	}

	void BitMsgDelta::writeDeltaShortCounter(int oldValue, int newValue) {
		if (m_newBase) {
			m_newBase->writeBits(newValue, 16);
		}

		if (!m_base) {
			m_writeDelta->writeDeltaShortCounter(oldValue, newValue);
			m_changed = true;
		} else {
			int baseValue = m_base->readBits(16);
			if (baseValue == newValue) {
				m_writeDelta->writeBits(0, 1);
			} else {
				m_writeDelta->writeBits(1, 1);
				m_writeDelta->writeDeltaShortCounter(oldValue, newValue);
				m_changed = true;
			}
		}
	}

	void BitMsgDelta::writeDeltaLongCounter(int oldValue, int newValue) {
		if (m_newBase) {
			m_newBase->writeBits(newValue, 32);
		}

		if (!m_base) {
			m_writeDelta->writeDeltaLongCounter(oldValue, newValue);
			m_changed = true;
		} else {
			int baseValue = m_base->readBits(32);
			if (baseValue == newValue) {
				m_writeDelta->writeBits(0, 1);
			} else {
				m_writeDelta->writeBits(1, 1);
				m_writeDelta->writeDeltaLongCounter(oldValue, newValue);
				m_changed = true;
			}
		}
	}

	void BitMsgDelta::readString(char *buffer, int bufferSize) const {
		if (!m_base) {
			m_readDelta->readString(buffer, bufferSize);
			m_changed = true;
		} else {
			char baseString[MAX_DATA_BUFFER];
			m_base->readString(baseString, sizeof(baseString));
			if (!m_readDelta || m_readDelta->readBits(1) == 0) {
				StringUtil::strncpyz(buffer, baseString, bufferSize);
			} else {
				m_readDelta->readString(buffer, bufferSize);
				m_changed = true;
			}
		}

		if (m_newBase) {
			m_newBase->writeString(buffer);
		}
	}

	void BitMsgDelta::readData(void *data, int length) const {
		if (!m_base) {
			m_readDelta->readData(data, length);
			m_changed = true;
		} else {
			char baseData[MAX_DATA_BUFFER];
			assert(length < sizeof(baseData));
			m_base->readData(baseData, length);
			if (!m_readDelta || m_readDelta->readBits(1) == 0) {
				memcpy(data, baseData, length);
			} else {
				m_readDelta->readData(data, length);
				m_changed = true;
			}
		}

		if (m_newBase) {
			m_newBase->writeData(data, length);
		}
	}

	void BitMsgDelta::readDict(StringDict &dict) {
		if (!m_base) {
			m_readDelta->readDeltaDict(dict, NULL);
			m_changed = true;
		} else {
			StringDict baseDict;
			m_base->readDeltaDict(baseDict, NULL);
			if (!m_readDelta) {
				dict = baseDict;
			} else {
				m_changed = m_readDelta->readDeltaDict(dict, &baseDict);
			}
		}

		if (m_newBase) {
			m_newBase->writeDeltaDict(dict, NULL);
		}
	}

	int BitMsgDelta::readDeltaByteCounter(int oldValue) const {
		int value;

		if (!m_base) {
			value = m_readDelta->readDeltaByteCounter(oldValue);
			m_changed = true;
		} else {
			int baseValue = m_base->readBits(8);
			if (!m_readDelta || m_readDelta->readBits(1) == 0) {
				value = baseValue;
			} else {
				value = m_readDelta->readDeltaByteCounter(oldValue);
				m_changed = true;
			}
		}

		if (m_newBase) {
			m_newBase->writeBits(value, 8);
		}
		return value;
	}

	int BitMsgDelta::readDeltaShortCounter(int oldValue) const {
		int value;

		if (!m_base) {
			value = m_readDelta->readDeltaShortCounter(oldValue);
			m_changed = true;
		} else {
			int baseValue = m_base->readBits(16);
			if (!m_readDelta || m_readDelta->readBits(1) == 0) {
				value = baseValue;
			} else {
				value = m_readDelta->readDeltaShortCounter(oldValue);
				m_changed = true;
			}
		}

		if (m_newBase) {
			m_newBase->writeBits(value, 16);
		}
		return value;
	}

	int BitMsgDelta::readDeltaLongCounter(int oldValue) const {
		int value;

		if (!m_base) {
			value = m_readDelta->readDeltaLongCounter(oldValue);
			m_changed = true;
		} else {
			int baseValue = m_base->readBits(32);
			if (!m_readDelta || m_readDelta->readBits(1) == 0) {
				value = baseValue;
			} else {
				value = m_readDelta->readDeltaLongCounter(oldValue);
				m_changed = true;
			}
		}

		if (m_newBase) {
			m_newBase->writeBits(value, 32);
		}
		return value;
	}

}} // namespace Axon::Network

/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_STATISTIC_H 
#define AX_STATISTIC_H

namespace Axon {

	class AX_API Statistic {
	public:
		typedef Sequence<int>		IndexSeq;
		enum Group {
			NoneGroup,
			EngineGroup,
			RenderGroup,
			RenderDrvGroup,
			ClientGroup,
			SoundGroup,
			EditorGroup,
			ServerGroup,
			MiscGroup,
			MaxGroup
		};

		Statistic();
		~Statistic();

		void initialize();
		void finalize();

		int getIndex(Group group, const String& name, bool autoreset = false);
		void setValue(int index, int value);
		void resetValue(int index);
		void addValue(int index, int added);
		void subValue(int index, int subed);
		void incValue(int index);
		void decValue(int index);

		int getValue(int index);
		const IndexSeq& getIndexsForGroup(Group group) const;
		const String& getValueName(int index) const;

	private:
		enum { MAX_PERFORMERS = 4096 };

		typedef std::map<String, int>		PerfItems;
		typedef std::map<Group, PerfItems>	PerfGroups;

		int m_values[MAX_PERFORMERS];
		bool m_autoResets[MAX_PERFORMERS];
		String m_valueNames[MAX_PERFORMERS];
		IndexSeq m_indexsForGroup[MaxGroup];

		PerfGroups perfGroups;
		int m_numValues;
	};

	inline void Statistic::setValue(int index, int value) {
		m_values[index] = value;
	}

	inline void Statistic::resetValue(int index) {
		m_values[index] = 0;
	}

	inline void Statistic::addValue(int index, int added) {
		m_values[index] += added;
	}

	inline void Statistic::subValue(int index, int subed) {
		m_values[index] -= subed;
	}

	inline void Statistic::incValue(int index) {
		m_values[index]++;
	}

	inline void Statistic::decValue(int index) {
		m_values[index]--;
	}


} // namespace Axon

#endif // AX_STATISTIC_H

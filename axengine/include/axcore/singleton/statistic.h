/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_STATISTIC_H 
#define AX_STATISTIC_H

AX_BEGIN_NAMESPACE

class Statistic;
extern Statistic *g_statistic;

class AX_API Stat
{
	friend class Statistic;

public:
	enum Flag {
		F_Bool = 1,
		F_Int = 2,
		F_Double = 4,
		F_AutoReset = 8
	};

	Stat(const char* group, const char *name, int flags, const char *desc = 0);
	~Stat() {}

	const char *getGroup() const { return m_group; }
	const char *getName() const { return m_name; }
	const char *getDesc() const { return m_desc; }
	int getFlags() const { return m_flags; }

	bool isAutoReset() const { return (m_flags & F_AutoReset) != 0; }

	bool getBool() const { return getDouble() != 0; }
	int getInt() const { return getDouble(); }
	double getDouble() const { double ret = m_value; if (isAutoReset()) m_value = 0; return ret; }

	void setBool(bool val) { m_value = val; }
	void setInt(int val) { m_value = val; }
	void setDouble(double val) { m_value = val; }
	void inc() { m_value += 1; }
	void dec() { m_value -= 1; }
	void add(double d) { m_value += d; }
	void sub(double d) { m_value -= d; }

private:
	const char *m_group;
	const char *m_name;
	const char *m_desc;
	int m_flags;
	mutable double m_value;
	Stat *m_staticLink;

	static Stat *ms_linkEnd;
};

class AX_API Statistic
{
	friend class Stat;

public:
	typedef Sequence<int> IndexSeq;
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
#if 0
	int getIndex(Group group, const String &name, bool autoreset = false);
	void setValue(int index, int value);
	void resetValue(int index);
	void addValue(int index, int added);
	void subValue(int index, int subed);
	void incValue(int index);
	void decValue(int index);

	int getValue(int index);
	const IndexSeq &getIndexsForGroup(Group group) const;
	const String &getValueName(int index) const;
#endif
	const Sequence<Stat *> &getGroup(const char *groupname) const;

protected:
	void registerStat(Stat *stat);

private:
#if 0
	enum { MAX_PERFORMERS = 4096 };

	typedef std::map<String, int> PerfItems;
	typedef std::map<Group, PerfItems> PerfGroups;

	int m_values[MAX_PERFORMERS];
	bool m_autoResets[MAX_PERFORMERS];
	String m_valueNames[MAX_PERFORMERS];
	IndexSeq m_indexsForGroup[MaxGroup];

	PerfGroups perfGroups;
	int m_numValues;
#endif
	// new api
	typedef Sequence<Stat *> Stats;
	typedef Dict<const char *, Stats, hash_cstr, equal_cstr> StatGroup;

	StatGroup m_statGroup;
};

inline Stat::Stat(const char* group, const char *name, int flags, const char *desc)
	: m_group(group)
	, m_name(name)
	, m_desc(desc)
	, m_flags(flags)
	, m_value(0)
{
	if (ms_linkEnd != reinterpret_cast<Stat*>(-1)) {
		m_staticLink = ms_linkEnd;
		ms_linkEnd = this;
	} else {
		g_statistic->registerStat(this);
	}
}


#if 0
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
#endif

AX_END_NAMESPACE

#endif // AX_STATISTIC_H

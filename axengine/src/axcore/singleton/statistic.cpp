/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

Stat *Stat::ms_staticLink = 0;

Statistic::Statistic()
{
#if 0
	m_numValues = 0;
	memset(m_values, 0, sizeof(m_values));
#endif

	Stat *staticHead = Stat::ms_staticLink;

	while (staticHead) {
		registerStat(staticHead);
		staticHead = staticHead->m_staticNext;
	}
	Stat::ms_staticLink = reinterpret_cast<Stat*>(-1);
}

Statistic::~Statistic()
{}

void Statistic::initialize()
{
}

void Statistic::finalize()
{
}

#if 0
int Statistic::getIndex(Group group, const String &name, bool autoreset)
{
	if (group <= NoneGroup || group >= MaxGroup) {
		Errorf(_("Statistic::Register: group out of bound"));
		return 0;
	}

	// check if already registered
	PerfGroups::iterator it = perfGroups.find(group);
	if (it != perfGroups.end()) {
		PerfItems::iterator item = it->second.find(name);
		if (item != it->second.end()) {
			return item->second;
		}
	}

	if (m_numValues == MAX_PERFORMERS)
		Errorf(_("Statistic::Register: MAX_PERFORMERS exceeded"));

	m_valueNames[m_numValues] = name;
	m_autoResets[m_numValues] = autoreset;

	// add to hash
	perfGroups[group][name] = m_numValues;

	// add to vector
	m_indexsForGroup[group].push_back(m_numValues);

	return m_numValues++;
}

int Statistic::getValue(int index)
{
	int result = m_values[index];
	if (m_autoResets[index]) {
		m_values[index] = 0;
	}
	return result;
}

const Statistic::IndexSeq &Statistic::getIndexsForGroup(Group group) const
{
	if (group <= NoneGroup || group >= MaxGroup) {
		Errorf(_("Statistic::GetIndexsForGroup: group out of bound"));
	}

	return m_indexsForGroup[group];
}

const String &Statistic::getValueName(int index) const
{
	if (index >= MAX_PERFORMERS) {
		Errorf(_("Statistic::GetValueName: MAX_PERFORMERS exceeded"));
	}

	return m_valueNames[index];
}
#endif

void Statistic::registerStat(Stat *stat)
{
	m_statGroup[stat->getGroup()].push_front(stat);
}

const List<Stat *> & Statistic::getGroup(const char *groupname) const
{
	static List<Stat *> s_empty;

	StatGroup::const_iterator it = m_statGroup.find(groupname);
	if (it != m_statGroup.end())
		return it->second;

	return s_empty;
}

AX_END_NAMESPACE

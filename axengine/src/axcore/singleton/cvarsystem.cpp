/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"


AX_BEGIN_NAMESPACE

AX_BEGIN_COMMAND_MAP(CvarSystem)
	AX_COMMAND_ENTRY("cvarlist",	list_f)
	AX_COMMAND_ENTRY("toggleCvar",	toggleCvar_f)
	AX_COMMAND_ENTRY("set",			set_f)
AX_END_COMMAND_MAP()

//------------------------------------------------------------------------------
// class Cvar
//------------------------------------------------------------------------------

Cvar *Cvar::ms_staticLink = 0;

Cvar::Cvar(const std::string &name, const std::string &default_string, int flags)
	: m_name(name)
	, m_defaultString(default_string)
	, m_latchedString()
	, m_stringValue(default_string)
	, m_flags(flags)
{
	m_modified = true;
	m_modifiedCount = 1;
	m_floatValue = (float)atof(default_string.c_str());
	m_integerValue = atoi(default_string.c_str());

	if (ms_staticLink != reinterpret_cast<Cvar*>(-1)) {
		m_staticNext = ms_staticLink;
		ms_staticLink = this;
		return;
	}

	g_cvarSystem->registerCvar(this);
}

Cvar::~Cvar()
{
	g_cvarSystem->removeCvar(m_name);
}

void Cvar::setString(const std::string &sz, bool force)
{
	std::string value;

	if (sz.empty()) {
		value = m_defaultString;
	} else {
		value = sz;
	}

	if (m_stringValue == value) {
		return;
	}

	if (!force) {
		if (m_flags & Cvar::Readonly) {
			Printf(_("%s is read only\n"), m_name.c_str());
			return;
		}

		if (m_flags & Cvar::Init) {
			Printf(_("%s can only be set when initializing\n"), m_name.c_str());
			return;
		}

		if (m_flags & Cvar::Latch) {
			if (!m_latchedString.empty()) {
				if (m_latchedString == value) {
					return;
				}
			} else {
				if (m_stringValue == value) {
					return;
				}
			}
			Printf(_("%s will be changed when next time you run the game.\n"), m_name.c_str());
			m_latchedString = value;
			m_modifiedCount++;
			m_modified = true;
			return;
		}
	} else {
		m_latchedString.clear();
	}

	m_modifiedCount++;
	m_modified = true;
	m_stringValue = value;
	m_floatValue = (float)atof(m_stringValue.c_str());
	m_integerValue = atoi(m_stringValue.c_str());
}

void Cvar::forceSet(const std::string &sz_value)
{
	setString(sz_value, true);
}

void Cvar::forceSet(float f)
{
	char buf[32];

	if (f == (int) f) {
		StringUtil::snprintf(buf, ArraySize(buf), "%i", (int)f);
	} else {
		StringUtil::snprintf(buf, ArraySize(buf), "%f", f);
	}

	setString(buf, true);
}

void Cvar::forceSet(int i)
{
	char buf[32];

	StringUtil::snprintf(buf, ArraySize(buf), "%i", i);

	setString(buf, true);
}

void Cvar::setString(const std::string &sz_value)
{
	setString(sz_value, false);
}

void Cvar::setFloat(float f)
{
	char buf[32];

	if (f == (int) f) {
		StringUtil::snprintf(buf, ArraySize(buf), "%i", (int)f);
	} else {
		StringUtil::snprintf(buf, ArraySize(buf), "%f", f);
	}

	setString(buf, false);
}

void Cvar::setInt(int i)
{
	char buf[32];

	StringUtil::snprintf(buf, ArraySize(buf), "%i", i);

	setString(buf, false);
}

void Cvar::setBool(bool b)
{
	setInt(b);
}

#if 0
Cvar *Cvar::create(const String &name, const String &default_string, uint_t flags) {
	Cvar *cvar = new Cvar;

	cvar->m_name = name;
	cvar->m_defaultString = default_string;
	cvar->m_stringValue = default_string;
	cvar->m_flags = flags;
	cvar->m_modified = true;
	cvar->m_modifiedCount = 1;
	cvar->m_floatValue = (float)atof(default_string.c_str());
	cvar->m_integerValue = atoi(default_string.c_str());

	return cvar;
}
#endif



//------------------------------------------------------------------------------
// class CvarSystem
//------------------------------------------------------------------------------

CvarSystem::CvarSystem()
	: m_dirtyFlags(0)
{
}

CvarSystem::~CvarSystem() {
}


void CvarSystem::removeCvar(const std::string &name)
{
	return;
#if 0
	if (m_cvarDict.exist(name)) {
		m_cvarDict.erase(name);
	} else {
		Debugf(_("CvarSystem::RemoveCvar: cann't find console variable '%s'"), name.c_str());
	}
#endif
}

void CvarSystem::initialize()
{
	Printf(_("Initializing CvarSystem...\n"));

	// register console command handler
	g_cmdSystem->registerHandler(this);

	// register static cvars
	Cvar *cvar = Cvar::ms_staticLink;
	while (cvar) {
		registerCvar(cvar);
		cvar = cvar->m_staticNext;
	}
	Cvar::ms_staticLink = reinterpret_cast<Cvar*>(-1);

	// read cvars from configure file
	StringPairSeq spv = g_systemConfig->getItems("CvarSystem");
	StringPairSeq::const_iterator it;
	for (it = spv.begin(); it != spv.end(); ++it) {
#if 0
		createCvar(it->first, it->second, Cvar::Temp);
#else
		set(it->first, it->second);
#endif
	}

	Printf(_("Initialized CvarSystem\n"));

	return;
}

void CvarSystem::finalize()
{
	Printf(_("Finalizing CvarSystem...\n"));

	// write cvars to configure file
	CvarDict::const_iterator it = m_cvarDict.begin();

	for (; it != m_cvarDict.end(); ++it) {
		if (it->second->getFlags() & Cvar::Archive) {
			if ((it->second->getFlags() & Cvar::Latch)) {
				if (it->second->m_latchedString.empty()) {
					g_systemConfig->setValue("CvarSystem", it->first, it->second->getString());
				} else {
					g_systemConfig->setValue("CvarSystem", it->first, it->second->m_latchedString);
				}
			} else {
				g_systemConfig->setValue("CvarSystem", it->first, it->second->getString());
			}
		}
	}

	// remove console command handle
	g_cmdSystem->removeHandler(this);

	Printf(_("Finalized CvarSystem\n"));
}

#if 0
Cvar *CvarSystem::createCvar(const String &name, const String &defaultString, uint_t flags)
{
	Cvar *cvar = nullptr;

	if (name.empty() /*|| defaultString.empty()*/) {
		Errorf(_("CvarSystem::createCvar: null parameter"));
		return cvar;
	}

	if (name.empty() || name.find_first_of("\\/;") != String::npos) {
		Printf(_("invalid cvar name string: %s"), name.c_str());
		return cvar;
	}

	CvarDict::iterator it = m_cvarDict.find(name);
	if (it != m_cvarDict.end() && it->second) {
		cvar = it->second;

		cvar->m_defaultString = defaultString;
		cvar->m_flags |= flags;
	} else {
		cvar = Cvar::create(name, defaultString, flags);
		// add to hash
		m_cvarDict[name] = cvar;
	}

	return cvar;
}

Cvar *CvarSystem::createCvar(const String &name, const String &defaultString)
{
	return createCvar(name, defaultString, Cvar::Archive);
}
#endif

bool CvarSystem::executeCommand(const CmdArgs &params)
{
	CvarDict::iterator it = m_cvarDict.find(params.tokened[0]);
	if (it == m_cvarDict.end())
		return false;

	Cvar *cvar = m_cvarDict[ params.tokened[0] ];
	if (params.tokened.size() == 1) {
		Printf(_("\"%s\" is:\"%s\" default:\"%s\"\n"), cvar->m_name.c_str(), cvar->m_stringValue.c_str(), cvar->m_defaultString.c_str());
		if ((cvar->m_flags & Cvar::Latch) && !cvar->m_latchedString.empty()) {
			Printf(_("latched: %s\n"), cvar->m_latchedString.c_str());
		}
		return true;
	}

	cvar->setString(params.tokened[1].c_str());

	return true;
}


// console command
void CvarSystem::list_f(const CmdArgs &param)
{
	CvarDict::iterator it = m_cvarDict.begin();
	int count = 0;
	const char *arg;
	char flags[16];

	if (param.tokened.size() > 1) {
		arg = param.tokened[1].c_str();
	} else {
		arg = NULL;
	}

	for (; it!=m_cvarDict.end(); ++it) {
		if (arg && StringUtil::filterString(arg, it->second->m_name.c_str(), false)) {
			continue;
		}

		flags[0] = it->second->m_flags & Cvar::Archive ? 'A' : ' ';
		flags[1] = it->second->m_flags & Cvar::Init    ? 'I' : ' ';
		flags[2] = it->second->m_flags & Cvar::Latch   ? 'L' : ' ';
		flags[3] = it->second->m_flags & Cvar::Readonly ? 'R' : ' ';
		flags[4] = it->second->m_flags & Cvar::Temp    ? 'T' : ' ';
		flags[5] = it->second->m_flags & Cvar::Cheat   ? 'C' : ' ';
		flags[6] = 0;

		Printf("%s %s \"%s\"\n", flags, it->second->m_name.c_str(), it->second->m_stringValue.c_str());

		count++;
	}

	Printf(_("%i total console variables\n"), count);
}

void CvarSystem::toggleCvar_f(const CmdArgs &param)
{
}

void CvarSystem::set_f(const CmdArgs &params)
{
	if (params.tokened.size() < 3) {
		return;
	}

#if 0
	Cvar*& cvar = m_cvarDict[ params.tokened[1] ];

	cvar = createCvar(params.tokened[1], params.tokened[2], 0);
	cvar->set(params.tokened[2], true);
#else
	set(params.tokened[1], params.tokened[2]);
#endif
}

void CvarSystem::set(const std::string &name, const std::string& value)
{
	CvarDict::iterator it = m_cvarDict.find(name);

	if (it == m_cvarDict.end()) {
		m_penddingSets[name] = value;
		return;
	}

	it->second->setString(value, true);
}

void CvarSystem::registerCvar(Cvar *cvar)
{
	const std::string &name = cvar->getName();

	if (m_cvarDict.find(name) != m_cvarDict.end()) {
		Errorf("Cvar '%s' already registered", name.c_str());
	}

	m_cvarDict[name] = cvar;

	Dict<std::string, std::string>::iterator it = m_penddingSets.find(name);
	if (it != m_penddingSets.end()) {
		cvar->setString(it->second, true);
		m_penddingSets.erase(it);
	}
}

AX_END_NAMESPACE



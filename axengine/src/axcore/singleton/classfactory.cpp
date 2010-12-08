/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

namespace {

	const char *ClassFactoryReg = "ClassFactory";

	std::string GetModuleIdFromClassName(const std::string &class_name) {
		std::string moduleid = class_name;

		size_t pos = moduleid.find(".");
		if (pos != std::string::npos) {
			moduleid.resize(pos);
		} else {
			moduleid.clear();
		}

		return moduleid;
	}

	std::string GetClassIdFromClassName(const std::string &class_name) {
		size_t pos = class_name.find(".");
		if (pos != std::string::npos) {
			return class_name.c_str() + pos + 1;
		} else {
			return class_name;
		}
	}

}

Module::Module(const std::string &name)
	: m_name(name)
	, m_handle(NULL)
	, m_classEntries(NULL)
{
	m_handle = OsUtil::loadDll(name);

	if (!m_handle) {
		Errorf(_("Module::Module: cann't load dll %s"), name.c_str());
		return;
	}

	std::string symbol = name;
	symbol += "_entry";

	typedef ClassEntry* (*fnGetClassEntries)();

	fnGetClassEntries fn = (fnGetClassEntries)OsUtil::getProcEntry(m_handle, symbol);

	if (fn)
		m_classEntries = fn();
	else
		Errorf(_("Module::Module: cann't get class entry."));
}

Module::Module(const std::string &name, funcGetClassEntries func) : m_name(name), m_handle(NULL) {
	m_classEntries = func();
}

Module::~Module() {
	if (m_handle)
		OsUtil::freeDll(m_handle);
}

const ClassEntry *Module::findClassEntry(const std::string &name) const {
	const ClassEntry *entry;

	for (entry = m_classEntries; entry->factory; entry++) {
		if (entry->className == name)
			return entry;
	}

	return NULL;
}

const ClassEntry *Module::getClassEntries() const {
	return m_classEntries;
}


ClassFactory::ClassFactory() {
}

ClassFactory::~ClassFactory() {
	ModuleDict::iterator it = m_moduleDict.begin();

	// free all module have loaded
	for (; it != m_moduleDict.end(); ++it) {
		SafeDelete(it->second);
	}
}

void ClassFactory::initialize() {
}

void ClassFactory::finalize() {
}

void *ClassFactory::createInstance(const std::string &class_name) {
	std::string mid = GetModuleIdFromClassName(class_name);

	Module *module = findModule(mid);

	std::string cid = GetClassIdFromClassName(class_name);

	const ClassEntry *entry = module->findClassEntry(cid);

	if (!entry)
		Errorf(_("ClassFactory::CreateInstance: cann't find entry for class '%s'"), class_name.c_str());

	return entry->factory();
}

void *ClassFactory::createInstanceByAlias(const std::string &class_alias) {
	if (!g_systemConfig)
		Errorf(_("ClassFactory::CreateInstanceByAlias: system configure coundn't found"));

	std::string class_name = g_systemConfig->getKeyValue(ClassFactoryReg, class_alias);

	if (!class_name.empty())
		return createInstance(class_name);

	return createInstance(class_alias);
}

void ClassFactory::registerStaticModule(const std::string &name, funcGetClassEntries func) {
	Module *module = new Module(name, func);
	m_moduleDict[name] = module;
}


Module *ClassFactory::findModule(const std::string &module_name) {
	ModuleDict::iterator it = m_moduleDict.find(module_name);
	if (it != m_moduleDict.end())
		return it->second;

	Module *module = new Module(module_name);
	m_moduleDict[module_name] = module;

	return module;
}

AX_END_NAMESPACE

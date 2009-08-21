/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

namespace {

	using namespace Axon;
	
	const char* ClassFactoryReg = "ClassFactory";

	String GetModuleIdFromClassName(const String& class_name) {
		String moduleid = class_name;

		size_t pos = moduleid.find(".");
		if (pos != String::npos) {
			moduleid.resize(pos);
		} else {
			moduleid.clear();
		}

		return moduleid;
	}

	String GetClassIdFromClassName(const String& class_name) {
		size_t pos = class_name.find(".");
		if (pos != String::npos) {
			return class_name.c_str() + pos + 1;
		} else {
			return class_name;
		}
	}

}



namespace Axon {

	Module::Module(const String& name)
		: m_name(name)
		, m_handle(NULL)
		, m_classEntries(NULL)
	{
		m_handle = OsUtil::loadDll(name);

		if (!m_handle) {
			Errorf(_("Module::Module: cann't load dll %s"), name.c_str());
			return;
		}

		String symbol = name;
		symbol += "_entry";

		typedef ClassEntry* (*fnGetClassEntries)();

		fnGetClassEntries fn = (fnGetClassEntries)OsUtil::getProcEntry(m_handle, symbol);

		if (fn)
			m_classEntries = fn();
		else
			Errorf(_("Module::Module: cann't get class entry."));
	}

	Module::Module(const String& name, funcGetClassEntries func) : m_name(name), m_handle(NULL) {
		m_classEntries = func();
	}

	Module::~Module() {
		if (m_handle)
			OsUtil::freeDll(m_handle);
	}

	const ClassEntry* Module::findClassEntry(const String& name) const {
		const ClassEntry* entry;

		for (entry = m_classEntries; entry->factory; entry++) {
			if (entry->className == name)
				return entry;
		}

		return NULL;
	}

	const ClassEntry* Module::getClassEntries() const {
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

	void* ClassFactory::createInstance(const String& class_name) {
		String mid = GetModuleIdFromClassName(class_name);

		Module* module = findModule(mid);

		String cid = GetClassIdFromClassName(class_name);

		const ClassEntry* entry = module->findClassEntry(cid);

		if (!entry)
			Errorf(_("ClassFactory::CreateInstance: cann't find entry for class '%s'"), class_name.c_str());

		return entry->factory();
	}

	void* ClassFactory::createInstanceByAlias(const String& class_alias) {
		if (!g_systemConfig)
			Errorf(_("ClassFactory::CreateInstanceByAlias: system configure coundn't found"));

		String class_name = g_systemConfig->getKeyValue(ClassFactoryReg, class_alias);

		if (!class_name.empty())
			return createInstance(class_name);

		return createInstance(class_alias);
	}

	void ClassFactory::registerStaticModule(const String& name, funcGetClassEntries func) {
		Module* module = new Module(name, func);
		m_moduleDict[name] = module;
	}


	Module* ClassFactory::findModule(const String& module_name) {
		ModuleDict::iterator it = m_moduleDict.find(module_name);
		if (it != m_moduleDict.end())
			return it->second;

		Module* module = new Module(module_name);
		m_moduleDict[module_name] = module;

		return module;
	}

} // namespace Axon

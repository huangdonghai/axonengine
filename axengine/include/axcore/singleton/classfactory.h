/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CORE_CLASS_FACTORY_H
#define AX_CORE_CLASS_FACTORY_H

#define AX_BEGIN_CLASS_MAP(module)							\
	extern "C" AX_DLL_EXPORT const ClassEntry *module##_entry() {	\
		static const ClassEntry classentries[] = {

#define AX_CLASS_ENTRY(className, classSelf)					\
		{ className, classSelf::_CreateInstance },

#define AX_END_CLASS_MAP()										\
		{ NULL, NULL } }; return classentries; }

#define AX_DECLARE_FACTORY()									\
	static void *_CreateInstance();

#define AX_IMPLEMENT_FACTORY(class_name)						\
	void *class_name::_CreateInstance() {						\
		return new class_name();								\
	}

#define AX_DECLARE_MODULE(module) extern "C" const ClassEntry *module##_entry();
#define AX_REGISTER_MODULE(module) g_classFactory->registerStaticModule(#module, module##_entry);

AX_BEGIN_NAMESPACE

struct ClassEntry {
	const char *className;
	void* (*factory)();
};

typedef const ClassEntry* (*funcGetClassEntries)();

class Module {
public:
	Module(const std::string &name);
	Module(const std::string &name, funcGetClassEntries);
	~Module();

	const ClassEntry *findClassEntry(const std::string &name) const;
	const ClassEntry *getClassEntries() const;

private:
	std::string m_name;
	Handle m_handle;
	const ClassEntry *m_classEntries;
};

typedef Dict<std::string, Module*>	ModuleDict;


class AX_API ClassFactory {
public:
	ClassFactory();
	virtual ~ClassFactory();

	void initialize();
	void finalize();
	void *createInstance(const std::string &class_name);
	void *createInstanceByAlias(const std::string &class_alias);
	void registerStaticModule(const std::string &module, funcGetClassEntries);

protected:
	// find a module have loaded, if not load yet, load it
	Module *findModule(const std::string &module_name);

private:
	ModuleDict m_moduleDict;
};

AX_END_NAMESPACE

#endif // AX_CORE_CLASS_FACTORY_H

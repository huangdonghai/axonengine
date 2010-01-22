/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/




#ifndef AX_SCRIPTSYSTEM_H
#define AX_SCRIPTSYSTEM_H

#define AX_DECLARE_CLASS(classname, baseclass) public: \
	typedef classname ThisClass; \
	typedef baseclass BaseClass; \
	virtual ::Axon::MetaInfo *classname::getMetaInfo() const { \
		return classname::registerMetaInfo(); \
	} \
	static ::Axon::MetaInfo *classname::registerMetaInfo() { \
		static ::Axon::MetaInfo *typeinfo; \
		if (!typeinfo) { \
			typeinfo = new ::Axon::MetaInfo_<classname>(#classname, BaseClass::registerMetaInfo());

#define AX_CONSTPROP(name) typeinfo->addProperty(#name, &ThisClass::get_##name);
#define AX_PROP(name) typeinfo->addProperty(#name, &ThisClass::get_##name, &ThisClass::set_##name);
#define AX_SIMPLEPROP(name) typeinfo->addProperty(#name, &ThisClass::m_##name);

#define AX_METHOD(name) typeinfo->addMethod(#name, &ThisClass::name);

#define AX_END_CLASS() \
				g_scriptSystem->registerType(typeinfo); \
			} \
		return typeinfo; \
	}


#define AX_REGISTER_CLASS(cppname) cppname::registerMetaInfo();


AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class ScriptSystem
//--------------------------------------------------------------------------

class ScriptSystem;
extern AX_API ScriptSystem *g_scriptSystem;

class AX_API ScriptSystem
{
public:
	friend class Object;

	ScriptSystem();
	~ScriptSystem();

	void initialize();
	void finalize();

#if 0
	void executeString(const String &text);
	void executeString(const char *text);
	void executeFile(const String &filename);
#endif
	void executeLine(const char *text);

#if 0
	bool invokeLuaMethod(const char *methodName, VariantSeq &stack, int nResult);
	bool invokeLuaMethod(const char *method, Variant &arg1);
	bool invokeLuaScoped(const char *text,VariantSeq &stack, int nResult);
#endif
	Object *createObject(const char *classname);
	Object *cloneObject(const Object *obj);
#if 0
	Object *findObject(const String &objectname);
#endif
	// for automatic name gen
	int getNameIndex(const String &str) const;
	void updateNameIndex(const String &str);
	int nextNameIndex(const String &str);
	String generateObjectName(const String &str);

	void registerType(MetaInfo *metainfo);
#if 0
	void registerClass(const String &self, const String &base);
#endif
	void registerSqClass(const String &name);

	void getClassList(const char *prefix, bool sort, StringSeq &result) const;

#if 0
	//
	// read from lua state
	//
	void beginRead();
	Variant readField(const char *objname, const char *fieldname);
	Variant readField(const char *objname);
	void endRead();
#endif
	// connect signal and slot
	bool connect(Object *sender, const String &sig, Object *recevier, const String &slot);
	bool disconnect(Object *sender, const String &sig, Object *recevier, const String &slot);

#if 0
	// immediately read
	Variant readFieldImmediately(const char *objname, const char *fieldname);
#endif
	const String &getPackagePath() { return m_packagePath; }

	static ScriptValue createMetaClosure(Member *method);

protected:
	void linkMetaInfoToClassInfo(MetaInfo *ti);

private:
	typedef Dict<String, SqClass*> SqClassDict;
	SqClassDict m_sqClassReg;

#if 0
	typedef Dict<String,ClassInfo*>	ClassInfoDict;
	ClassInfoDict m_classInfoReg;
#endif

	typedef Dict<const char*, MetaInfo*, hash_cstr, equal_cstr> MetaInfoDict;
	MetaInfoDict m_typeInfoReg;

	typedef Dict<String,int> StringIntDict;
	StringIntDict m_objectNameGen;

	String m_packagePath;

	bool m_isReading;
	int m_readTop;
};


template< class T >
T object_cast(Object *obj) {
	if (obj->inherits(T(0)->registerMetaInfo()->getName())) {
		return (T)obj;
	}
	return nullptr;
}

AX_END_NAMESPACE

#endif // AX_SCRIPTSYSTEM_H


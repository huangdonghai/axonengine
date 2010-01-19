/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/




#if 0
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

template< class T >
struct variant_cast_helper {
	T doCast(const Variant &v) {
		return v;
	}
};

template<>
struct variant_cast_helper<Object*> {
	Object *doCast(const Variant &v) {
		return v.operator Object*();
	}
};

template<class T>
struct variant_cast_helper<T*> {
	T *doCast(const Variant &v) {
		Object *obj = variant_cast_helper<Object*>().doCast(v);
		return object_cast<T*>(obj);
	}
};

template<class T>
T variant_cast(const Variant &v) {
	return variant_cast_helper<T>().doCast(v);
}
#if 0
template<>
Object *variant_cast<Object*>(const Variant &v) {
	return v.operator Object*();
}

template<class T>
T *variant_cast<T*>(const Variant &v) {
	Object *obj = variant_cast<Object*>(v);
	return 0;
}
#endif


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

	void executeString(const String &text);
	void executeString(const char *text);
	void executeFile(const String &filename);
	bool invokeLuaMethod(const char *methodName, VariantSeq &stack, int nResult);
	bool invokeLuaMethod(const char *method, Variant &arg1);
	bool invokeLuaScoped(const char *text,VariantSeq &stack, int nResult);
	String generateLuaString(const String &text);

	Object *createObject(const char *classname);
	Object *cloneObject(const Object *obj);
	Object *findObject(const String &objectname);

	// for automatic name gen
	int getNameIndex(const String &str) const;
	void updateNameIndex(const String &str);
	int nextNameIndex(const String &str);
	String generateObjectName(const String &str);

	void registerType(MetaInfo *metainfo);
	void registerClass(const String &self, const String &base);

	void getClassList(const char *prefix, bool sort, StringSeq &result) const;

	//
	// read from lua state
	//
	void beginRead();
	Variant readField(const char *objname, const char *fieldname);
	Variant readField(const char *objname);
	void endRead();

	// connect signal and slot
	bool connect(Object *sender, const String &sig, Object *recevier, const String &slot);
	bool disconnect(Object *sender, const String &sig, Object *recevier, const String &slot);

	// immediately read
	Variant readFieldImmediately(const char *objname, const char *fieldname);

	const String &getPackagePath() { return m_packagePath; }

protected:
	void linkMetaInfoToClassInfo(MetaInfo *ti);

private:
	typedef Dict<String,ClassInfo*>	ClassInfoDict;
	ClassInfoDict m_classInfoReg;

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


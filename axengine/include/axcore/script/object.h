#ifndef AX_OBJECT_H
#define AX_OBJECT_H

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class Object
//--------------------------------------------------------------------------

class File;
class Object;
typedef Object *ObjectStar;

class AX_API Object {
public:
	friend class ScriptSystem;

	Object();
	virtual ~Object();

	virtual MetaInfo *getMetaInfo() const;
	virtual String getNamespace() const { return String(); }
	static MetaInfo *registerMetaInfo();
#if 0
	const ClassInfo *getClassInfo() const { return m_classInfo; }
#else
	const SqClass *getScriptClass() const { return m_sqClass; }
#endif
	Member *findMember(const char *name) const;
	bool getProperty(const char *name, Variant &ret) const;
	bool setProperty(const char *name, const Variant &value);
	bool setProperty(const char *name, const char *value);

	// properties
	void set_objectName(const String &name);
	String get_objectName() const;

	bool inherits(const char *cls) const;
	bool isClass(const char *classname) const;

	// read and write properties
	void writeProperties(File *f, int indent=0) const;
	void readProperties(const TiXmlElement *node);
	void copyPropertiesFrom(const Object *rhs);

	// runtime
	void setRuntime(const char *name, const Variant &val);
	Variant getRuntime(const char *name);

	void doPropertyChanged();

	bool invokeMethod(const char *methodName, const Ref &ret, const ConstRef &arg0, const ConstRef &arg1, const ConstRef &arg2, const ConstRef &arg3, const ConstRef &arg4);

	// invoke helper
	bool invokeMethod_(const char *methodName);
	template <class A0>
	bool invokeMethod_(const char *methodName, const A0 &a0);
	template <class A0, class A1>
	bool invokeMethod_(const char *methodName, const A0 &a0, const A1 &a1);
	template <class A0, class A1, class A2>
	bool invokeMethod_(const char *methodName, const A0 &a0, const A1 &a1, const A2 &a2);
	template <class A0, class A1, class A2, class A3>
	bool invokeMethod_(const char *methodName, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3);
	template <class A0, class A1, class A2, class A3, class A4>
	bool invokeMethod_(const char *methodName, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4);

	template <class Rt>
	bool invokeMethodRt_(const char *methodName, Rt &ret);
	template <class Rt, class A0>
	bool invokeMethodRt_(const char *methodName, Rt &ret, const A0 &a0);
	template <class Rt, class A0, class A1>
	bool invokeMethodRt_(const char *methodName, Rt &ret, const A0 &a0, const A1 &a1);
	template <class Rt, class A0, class A1, class A2>
	bool invokeMethodRt_(const char *methodName, Rt &ret, const A0 &a0, const A1 &a1, const A2 &a2);
	template <class Rt, class A0, class A1, class A2, class A3>
	bool invokeMethodRt_(const char *methodName, Rt &ret, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3);
	template <class Rt, class A0, class A1, class A2, class A3, class A4>
	bool invokeMethodRt_(const char *methodName, Rt &ret, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4);

protected:
	virtual void onPropertyChanged();

	void invoke_onInit();
	void invoke_onFinalize();
	void invoke_onPropertyChanged();

	void invokeCallback(const String &callback);
	void invokeCallback(const String &callback, const Variant &param);

	void setObjectName(const String &name, bool force);
	void resetObjectName();

	void raiseSignal(const String &signal, const Variant &param);
#if 0
	void initClassInfo(const ClassInfo *ci);
#else
	void initScriptClass(const SqClass *sqclass);
#endif

private:
#if 0
	const ClassInfo *m_classInfo;
#else
	const SqClass *m_sqClass;
#endif
	String m_objectNamespace;
	String m_objectName;
};

inline bool Object::invokeMethod_(const char *methodName)
{
	return invokeMethod(methodName, Ref(), ConstRef(), ConstRef(), ConstRef(), ConstRef(), ConstRef());
}

template <class A0>
bool Object::invokeMethod_(const char *methodName, const A0 &a0)
{
	return invokeMethod(methodName, Ref(), AX_ARG(a0), ConstRef(), ConstRef(), ConstRef(), ConstRef());
}

template <class A0, class A1>
bool Object::invokeMethod_(const char *methodName, const A0 &a0, const A1 &a1)
{
	return invokeMethod(methodName, Ref(), AX_ARG(a0), AX_ARG(a1), ConstRef(), ConstRef(), ConstRef());
}

template <class A0, class A1, class A2>
bool Object::invokeMethod_(const char *methodName, const A0 &a0, const A1 &a1, const A2 &a2)
{
	return invokeMethod(methodName, Ref(), AX_ARG(a0), AX_ARG(a1), AX_ARG(a2), ConstRef(), ConstRef());
}

template <class A0, class A1, class A2, class A3>
bool Object::invokeMethod_(const char *methodName, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3)
{
	return invokeMethod(methodName, Ref(), AX_ARG(a0), AX_ARG(a1), AX_ARG(a2), AX_ARG(a3), ConstRef());
}

template <class A0, class A1, class A2, class A3, class A4>
bool Object::invokeMethod_(const char *methodName, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{
	return invokeMethod(methodName, Ref(), AX_ARG(a0), AX_ARG(a1), AX_ARG(a2), AX_ARG(a3), AX_ARG(a4));
}

template <class Rt>
bool Object::invokeMethodRt_(const char *methodName, Rt &ret)
{
	return invokeMethod(methodName, AX_RETURN_ARG(ret), ConstRef(), ConstRef(), ConstRef(), ConstRef(), ConstRef());
}

template <class Rt, class A0>
bool Object::invokeMethodRt_(const char *methodName, Rt &ret, const A0 &a0)
{
	return invokeMethod(methodName, AX_RETURN_ARG(ret), AX_ARG(a0), ConstRef(), ConstRef(), ConstRef(), ConstRef());
}

template <class Rt, class A0, class A1>
bool Object::invokeMethodRt_(const char *methodName, Rt &ret, const A0 &a0, const A1 &a1)
{
	return invokeMethod(methodName, AX_RETURN_ARG(ret), AX_ARG(a0), AX_ARG(a1), ConstRef(), ConstRef(), ConstRef());
}

template <class Rt, class A0, class A1, class A2>
bool Object::invokeMethodRt_(const char *methodName, Rt &ret, const A0 &a0, const A1 &a1, const A2 &a2)
{
	return invokeMethod(methodName, AX_RETURN_ARG(ret), AX_ARG(a0), AX_ARG(a1), AX_ARG(a2), ConstRef(), ConstRef());
}

template <class Rt, class A0, class A1, class A2, class A3>
bool Object::invokeMethodRt_(const char *methodName, Rt &ret, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3)
{
	return invokeMethod(methodName, AX_RETURN_ARG(ret), AX_ARG(a0), AX_ARG(a1), AX_ARG(a2), AX_ARG(a3), ConstRef());
}

template <class Rt, class A0, class A1, class A2, class A3, class A4>
bool Object::invokeMethodRt_(const char *methodName, Rt &ret, const A0 &a0, const A1 &a1, const A2 &a2, const A3 &a3, const A4 &a4)
{
	return invokeMethod(methodName, AX_RETURN_ARG(ret), AX_ARG(a0), AX_ARG(a1), AX_ARG(a2), AX_ARG(a3), AX_ARG(a4));
}



AX_END_NAMESPACE

#endif // AX_OBJECT_H
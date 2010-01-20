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
	static MetaInfo *m_metaInfo;

	const ClassInfo *getClassInfo() const { return m_classInfo; }

	Member *findMember(const char *name) const;
	Variant getProperty(const char *name) const;
	bool setProperty(const char *name, const Variant &value);
	bool setProperty(const char *name, const char *value);
	void invokeMethod(const char *name, const Variant &arg1);
	void invokeMethod(const char *name, const Variant &arg1, const Variant &arg2);

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

	void initClassInfo(const ClassInfo *ci);

private:
	const ClassInfo *m_classInfo;
	String m_objectNamespace;
	String m_objectName;
};

AX_END_NAMESPACE

#endif // AX_OBJECT_H
#ifndef AX_METAINFO_H
#define AX_METAINFO_H

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class Member
//--------------------------------------------------------------------------

class AX_API Member
{
public:
	enum Type {
		kPropertyType, kMethodType
	};

	enum Kind {
		kEnum = Variant::kMaxType, kFlag, kTexture, kModel, kMaterial, kAnimation, kSpeedTree, kSound, kGroup
	};

	typedef std::pair<String,int> EnumItem;
	typedef Sequence<EnumItem> EnumItems;

	Member(const char *name, Type t) : m_name(name), m_type(t), m_propKind(Variant::kEmpty) {}

	bool isProperty() const { return m_type == kPropertyType; }
	bool isMethod() const { return m_type == kMethodType; }
	Type getType() const { return m_type; }
	const char *getName() const { return m_name; }
	int getNumParams() const { return m_numParams; }
	Variant::Type getPropType() const { return m_propType; }
	int getPropKind() const { return m_propKind; }
	const EnumItems &getEnumItems() const { return m_enumItems; }

	// method
	virtual int invoke(Object *obj, VariantSeq &stack) { AX_ASSERT(0);return 0;}

	// property
	virtual void setProperty(Object *obj, const Variant &val) { AX_ASSERT(0); }
	virtual Variant getProperty(const Object *obj) { AX_ASSERT(0); return Variant(); }
	virtual void *getPropertyPointer(const Object *obj) { AX_ASSERT(0); return 0; }
	virtual bool isConst() const { return false; }
	virtual bool isAnimatable() const { return false; }

	static Variant::Type kindToType(int k)
	{
		if (k < Variant::kMaxType) {
			return Variant::Type(k);
		}
		switch (k) {
case kEnum: case kFlag: return Variant::kInt;
case kTexture: case kModel: case kMaterial: case kAnimation: case kSpeedTree: case kSound: return Variant::kString;
default: return Variant::kEmpty;
		}
	}

protected:
	const char *m_name;
	Type m_type;
	int m_numParams;
	Variant::Type m_propType;
	int m_propKind;
	EnumItems m_enumItems;
};

typedef Sequence<Member*> MemberSeq;
typedef Dict<const char*, Member*, hash_cstr, equal_cstr> MemberDict;

//--------------------------------------------------------------------------
// template SimpleProp_
//--------------------------------------------------------------------------

template< class T, class M >
class SimpleProp_ : public Member {
public:
	typedef M (T::*DataType);

	SimpleProp_(const char *name, DataType d);

	virtual void setProperty(Object *obj, const Variant &val);
	virtual Variant getProperty(const Object *obj);
	virtual void *getPropertyPointer(const Object *obj);
	virtual bool isConst() const { return false; }
	virtual bool isAnimatable() const { return true; }

private:
	DataType m_d;
};

template< class T, class M >
SimpleProp_<T,M>::SimpleProp_(const char *name, DataType d) : Member(name, Member::kPropertyType) {
	m_d = d;
	m_propType = GetVariantType_<M>();
}

template< class T, class M >
void SimpleProp_<T,M>::setProperty(Object *obj, const Variant &val) {
	T *t = (T*)obj;
	t->*m_d = val;
}

template< class T, class M >
Variant SimpleProp_<T,M>::getProperty(const Object *obj) {
	T *t = (T*)obj;
	return t->*m_d;
}

template< class T, class M >
void *SimpleProp_<T,M>::getPropertyPointer(const Object *obj) {
	T *t = (T*)obj;
	return &(t->*m_d);
}

//--------------------------------------------------------------------------
// template Property_
//--------------------------------------------------------------------------

template< typename T, typename GetType, typename SetType >
class Property_ : public Member {
public:
	typedef GetType (T::*GetFunc)() const;
	typedef void (T::*SetFunc)(SetType);

	Property_(const char *name, GetFunc getfunc, SetFunc setfunc);
	Property_(const char *name, GetFunc getfunc);

	virtual void setProperty(Object *obj, const Variant &val);
	virtual Variant getProperty(const Object *obj);
	virtual bool isConst() const { return m_setFunc == nullptr; }

private:
	GetFunc m_getFunc;
	SetFunc m_setFunc;
};

template< typename T, typename GetType, typename SetType >
Property_<T,GetType,SetType>::Property_(const char *name, GetFunc getfunc, SetFunc setfunc)
: Member(name, Member::kPropertyType)
, m_getFunc(getfunc)
, m_setFunc(setfunc)
{
	m_propType = GetVariantType_<GetType>();
}

template< typename T, typename GetType, typename SetType >
Property_<T,GetType,SetType>::Property_(const char *name, GetFunc getfunc)
: Member(name, Member::kPropertyType)
, m_getFunc(getfunc)
, m_setFunc(nullptr)
{
	m_propType = GetVariantType_<GetType>();
}

template< typename T, typename GetType, typename SetType >
void Property_<T,GetType,SetType>::setProperty(Object *obj, const Variant &val) {
	if (!m_setFunc) return;
	T *t = (T*)obj;
	(t->*m_setFunc)(val);
}

template< typename T, typename GetType, typename SetType >
Variant Property_<T,GetType,SetType>::getProperty(const Object *obj) {
	T *t = (T*)obj;
	return (t->*m_getFunc)();
}


//--------------------------------------------------------------------------
// template ReturnSpecialization
//--------------------------------------------------------------------------

#define P(x) stack[x].cast<remove_const_reference<P##x>::type>()

template< typename Rt >
struct ReturnSpecialization {
	template< typename T >
	static int call(T *object, Rt (T::*func)(), VariantSeq &stack) {
		Rt ret = (object->*func)();
		Variant val(ret);
		stack.push_back(val);
		return 1;
	}

	template< typename T, typename P0 >
	static int call(T *object, Rt (T::*func)(P0), VariantSeq &stack) {
		Rt ret = (object->*func)(P(0));
		Variant val(ret);
		stack.push_back(val);
		return 1;
	}

	template< typename T, typename P0, typename P1 >
	static int call(T *object, Rt (T::*func)(P0,P1), VariantSeq &stack) {
		Rt ret = (object->*func)(P(0),P(1));
		Variant val(ret);
		stack.push_back(val);
		return 1;
	}

	template< typename T, typename P0, typename P1, typename P2 >
	static int call(T *object, Rt (T::*func)(P0,P1,P2), VariantSeq &stack) {
		Rt ret = (object->*func)(P(0),P(1),P(2));
		Variant val(ret);
		stack.push_back(val);
		return 1;
	}

	template< typename T, typename P0, typename P1, typename P2, typename P3 >
	static int call(T *object, Rt (T::*func)(P0,P1,P2,P3), VariantSeq &stack) {
		Rt ret = (object->*func)(stack(0),stack(1),stack(2),stack(3));
		Variant val(ret);
		stack.push_back(val);
		return 1;
	}

	template< typename T, typename P0, typename P1, typename P2, typename P3, typename P4 >
	static int call(T *object, Rt (T::*func)(P0,P1,P2,P3,P4), VariantSeq &stack) {
		Rt ret = (object->*func)(P(0),P(1),P(2),P(3),P(4));
		Variant val(ret);
		stack.push_back(val);
		return 1;
	}

};

template<>
struct ReturnSpecialization< void > {
	template< typename T >
	static int call(T *object, void (T::*func)(), VariantSeq &stack) {
		(object->*func)();
		return 0;
	}

	template< typename T, typename P0 >
	static int call(T *object, void (T::*func)(P0), VariantSeq &stack) {
		(object->*func)(P(0));
		return 0;
	}

	template< typename T, typename P0, typename P1 >
	static int call(T *object, void (T::*func)(P0,P1), VariantSeq &stack) {
		(object->*func)(P(0),P(1));
		return 0;
	}

	template< typename T, typename P0, typename P1, typename P2 >
	static int call(T *object, void (T::*func)(P0,P1,P2), VariantSeq &stack) {
		(object->*func)(P(0),P(1),P(2));
		return 0;
	}

	template< typename T, typename P0, typename P1, typename P2, typename P3 >
	static int call(T *object, void (T::*func)(P0,P1,P2,P3), VariantSeq &stack) {
		(object->*func)(P(0),P(1),P(2),P(3));
		return 0;
	}

	template< typename T, typename P0, typename P1, typename P2, typename P3, typename P4 >
	static int call(T *object, void (T::*func)(P0,P1,P2,P3,P4), VariantSeq &stack) {
		(object->*func)(P(0),P(1),P(2),P(3),P(4));
		return 0;
	}

};
#undef P

//--------------------------------------------------------------------------
// template Method_
//--------------------------------------------------------------------------

template< typename Signature >
class Method_ : public Member {
public:
	AX_STATIC_ASSERT(0);
private:
};

template< typename Rt, typename T >
class Method_<Rt (T::*)()> : public Member {
public:
	typedef Rt (T::*FunctionType)();

	Method_(const char *name, FunctionType m)
		: Member(name, Member::kMethodType)
		, m_m(m)
	{
		m_numParams = 0;
	}

	virtual int invoke(Object *obj, VariantSeq &stack) {
		AX_ASSERT(stack.size() == getNumParams());
		T *realobj = dynamic_cast<T*>(obj);
		AX_ASSERT(realobj);
		return ReturnSpecialization<Rt>::call(realobj, m_m, stack);
	}

private:
	String m_name;
	FunctionType m_m;
};

template< typename Rt, typename T, typename Arg1 >
class Method_<Rt (T::*)(Arg1)> : public Member {
public:
	typedef Rt (T::*FunctionType)(Arg1);

	Method_(const char *name, FunctionType m)
		: Member(name, Member::kMethodType)
		, m_m(m)
	{
		m_numParams = 1;
	}

	virtual int invoke(Object *obj, VariantSeq &stack) {
		AX_ASSERT(stack.size() == getNumParams());
		T *realobj = dynamic_cast<T*>(obj);
		AX_ASSERT(realobj);
		return ReturnSpecialization<Rt>::call(realobj, m_m, stack);
	}

private:
	String m_name;
	FunctionType m_m;
};

template< typename Rt, typename T, typename Arg1, typename Arg2 >
class Method_<Rt (T::*)(Arg1,Arg2)> : public Member {
public:
	typedef Rt (T::*FunctionType)(Arg1,Arg2);

	Method_(const char *name, FunctionType m)
		: Member(name, Member::kMethodType)
		, m_m(m)
	{
		m_numParams = 2;
	}

	virtual int invoke(Object *obj, VariantSeq &stack) {
		AX_ASSERT(stack.size() == getNumParams());
		T *realobj = dynamic_cast<T*>(obj);
		AX_ASSERT(realobj);
		return ReturnSpecialization<Rt>::call(realobj, m_m, stack);
	}

private:
	String m_name;
	FunctionType m_m;
};

template< typename Rt, typename T, typename Arg1, typename Arg2, typename Arg3 >
class Method_<Rt (T::*)(Arg1,Arg2,Arg3)> : public Member {
public:
	typedef Rt (T::*FunctionType)(Arg1,Arg2,Arg3);

	Method_(const char *name, FunctionType m)
		: Member(name, Member::kMethodType)
		, m_m(m)
	{
		m_numParams = 3;
	}

	virtual int invoke(Object *obj, VariantSeq &stack) {
		AX_ASSERT(stack.size() == getNumParams());
		T *realobj = dynamic_cast<T*>(obj);
		AX_ASSERT(realobj);
		return ReturnSpecialization<Rt>::call(realobj, m_m, stack);
	}

private:
	String m_name;
	FunctionType m_m;
};

template< typename Rt, typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4 >
class Method_<Rt (T::*)(Arg1,Arg2,Arg3,Arg4)> : public Member {
public:
	typedef Rt (T::*FunctionType)(Arg1,Arg2,Arg3,Arg4);

	Method_(const char *name, FunctionType m)
		: Member(name, Member::kMethodType)
		, m_m(m)
	{
		m_numParams = 4;
	}

	virtual int invoke(Object *obj, VariantSeq &stack) {
		AX_ASSERT(stack.size() == getNumParams());
		T *realobj = dynamic_cast<T*>(obj);
		AX_ASSERT(realobj);
		return ReturnSpecialization<Rt>::call(realobj, m_m, stack);
	}

private:
	String m_name;
	FunctionType m_m;
};

template< typename Rt, typename T, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5 >
class Method_<Rt (T::*)(Arg1,Arg2,Arg3,Arg4,Arg5)> : public Member {
public:
	typedef Rt (T::*FunctionType)(Arg1,Arg2,Arg3,Arg4,Arg5);

	Method_(const char *name, FunctionType m)
		: Member(name, Member::kMethodType)
		, m_m(m)
	{
		m_numParams = 5;
	}

	virtual int invoke(Object *obj, VariantSeq &stack) {
		AX_ASSERT(stack.size() == getNumParams());
		T *realobj = dynamic_cast<T*>(obj);
		AX_ASSERT(realobj);
		return ReturnSpecialization<Rt>::call(realobj, m_m, stack);
	}

private:
	String m_name;
	FunctionType m_m;
};


//--------------------------------------------------------------------------
// class MetaInfo
//--------------------------------------------------------------------------
class ClassInfo;
class AX_API MetaInfo
{
	friend class ScriptSystem;

public:
	MetaInfo(const char *classname, MetaInfo *base);
	virtual ~MetaInfo();

	template< typename T, typename GetType, typename SetType >
	MetaInfo &addProperty(const char *name, GetType (T::*get)() const, void (T::*set)(SetType));

	template< typename T, typename GetType >
	MetaInfo &addProperty(const char *name, GetType (T::*get)() const);

	template< typename T, typename M >
	MetaInfo &addProperty(const char *name, M (T::*d));

	template< typename Signature >
	MetaInfo &addMethod(const char *name, Signature m);

	Member *findMember(const char *name) const;
	MetaInfo *getBase() const;

	const char *getName() const;
	const MemberSeq &getMembers() const;

	virtual Object *createObject() = 0;

protected:
	void addMember(Member *member);

private:
	const char *m_name;
	MetaInfo *m_base;
	ClassInfo *m_classInfo;
	MemberSeq m_members;
	MemberDict m_memberDict;
};

inline MetaInfo::MetaInfo(const char *classname, MetaInfo *base)
: m_name(classname)
, m_base(base)
, m_classInfo(nullptr)
{}

inline MetaInfo::~MetaInfo() {}

template< typename T, typename GetType, typename SetType >
MetaInfo &MetaInfo::addProperty(const char *name, GetType (T::*get)() const, void (T::*set)(SetType)) {
	Member *member = new Property_<T,GetType,SetType>(name, get, set);
	addMember(member);
	return *this;
}

template< typename T, typename GetType >
MetaInfo &MetaInfo::addProperty(const char *name, GetType (T::*get)() const) {
	Member *member = new Property_<T,GetType,GetType>(name, get);
	addMember(member);
	return *this;
}

template< typename T, typename M >
MetaInfo &MetaInfo::addProperty(const char *name, M (T::*d)) {
	Member *member = new SimpleProp_<T,M>(name, d);
	addMember(member);
	return *this;
}


template< typename Signature >
MetaInfo &MetaInfo::addMethod(const char *name, Signature m) {
	Member *member = new Method_<Signature>(name, m);
	addMember(member);
	return *this;
}

inline void MetaInfo::addMember(Member *member) {
	m_members.push_back(member);
	m_memberDict[member->getName()] = member;
}

inline Member *MetaInfo::findMember(const char *name) const {
	MemberDict::const_iterator it = m_memberDict.find(name);

	if (it != m_memberDict.end())
		return it->second;
	return nullptr;
}

inline MetaInfo *MetaInfo::getBase() const {
	return m_base;
}

inline const char *MetaInfo::getName() const {
	return m_name;
}

inline const MemberSeq &MetaInfo::getMembers() const {
	return m_members;
}

//--------------------------------------------------------------------------
// class MetaInfo_
//--------------------------------------------------------------------------
template< class T >
class MetaInfo_ : public MetaInfo {
public:
	MetaInfo_(const char *classname, MetaInfo *base)
		: MetaInfo(classname, base)
	{}
	virtual ~MetaInfo_() {}

	virtual Object *createObject() {
		return new T;
	}
};

//--------------------------------------------------------------------------
// class ScriptProp
//--------------------------------------------------------------------------

class AX_API ScriptProp : public Member {
public:
	friend class ClassInfo;
	ScriptProp();

	virtual void setProperty(Object *obj, const Variant &val);
	virtual Variant getProperty(const Object *obj);
	bool grouped() const { return m_group || m_propKind == kGroup; }

protected:
	void init();
	void initEnumItems();
	int checkNameKind();
	int checkTableKind(Variant &result);
	bool isStringType(int kind);

public:
	String m_realName;
	String m_showName;
	Variant m_defaultValue;
	ScriptProp *m_group;
};
typedef Sequence<ScriptProp*> ScriptPropSeq;
typedef Dict<String,ScriptProp*> ScriptPropDict;


//--------------------------------------------------------------------------
// class ClassInfo
//--------------------------------------------------------------------------

class AX_API ClassInfo {
public:
	ClassInfo() {}

	Variant getField(const String &field) const;
	void initScriptProps();

	String m_className;
	String m_metaName;
	MetaInfo *m_typeInfo;
	ScriptPropSeq m_scriptPropSeq;
	ScriptPropDict m_scriptProps;
};

AX_END_NAMESPACE

#endif // AX_METAINFO_H
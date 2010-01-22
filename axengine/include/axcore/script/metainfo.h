#ifndef AX_METAINFO_H
#define AX_METAINFO_H

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class Member
//--------------------------------------------------------------------------

class AX_API Member
{
public:
	enum {
		MaxArgs = 5
	};

	typedef int (*ScriptFunc)(void *vm);

	enum Type {
		kPropertyType, kMethodType
	};

	enum Kind {
		kEnum = Variant::kMaxType, kFlag, kTexture, kModel, kMaterial, kAnimation, kSpeedTree, kSound, kGroup
	};

	typedef std::pair<String,int> EnumItem;
	typedef Sequence<EnumItem> EnumItems;

	Member(const char *name, Type t) : m_name(name), m_type(t), m_propKind(Variant::kVoid) {}

	bool isProperty() const { return m_type == kPropertyType; }
	bool isMethod() const { return m_type == kMethodType; }
	Type getType() const { return m_type; }
	const char *getName() const { return m_name; }
	int argc() const { return m_argc; }
	Variant::TypeId getPropType() const { return m_propType; }
	int getPropKind() const { return m_propKind; }
	const EnumItems &getEnumItems() const { return m_enumItems; }
	const Variant::TypeId *getArgsType() const { return m_argsType; }
	Variant::TypeId getReturnType() const { return m_returnType; }
	ScriptValue &getScriptClousure() { return m_scriptClosure; }

	Variant getPropertyNoCheck(const Object *obj)
	{
		AX_ASSERT(isProperty());
		Variant ret;
		bool v = getProperty(obj, ret);
		AX_ASSERT(v);
		return ret;
	}

	// method
	virtual int invoke(Object *obj, void *ret, const void **argv) { return 0; }

	// property
	virtual bool isConst() const { return false; }
	virtual bool isAnimatable() const { return false; }
	virtual bool getProperty(const Object *obj, Variant &ret) { return false; }
	virtual bool setProperty(Object *obj, const ConstRef &arg) { return false; }
	virtual bool resetProperty(Object *obj) { return false; }

	static Variant::TypeId kindToType(int k)
	{
		if (k < Variant::kMaxType) {
			return Variant::TypeId(k);
		}
		switch (k) {
		case kEnum: case kFlag: return Variant::kInt;
		case kTexture: case kModel: case kMaterial: case kAnimation: case kSpeedTree: case kSound: return Variant::kString;
		default: return Variant::kVoid;
		}
	}

protected:
	const char *m_name;
	Type m_type;

	// for method
	int m_argc;
	Variant::TypeId m_returnType;
	Variant::TypeId m_argsType[MaxArgs];
	ScriptValue m_scriptClosure;

	// for property
	Variant::TypeId m_propType;
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

	SimpleProp_(const char *name, DataType d)
		: Member(name, Member::kPropertyType)
	{
		m_d = d;
		m_propType = GetVariantType_<M>();
	}

	virtual bool isConst() const
	{ return false; }

	virtual bool isAnimatable() const
	{ return true; }

	virtual bool getProperty(const Object *obj, Variant &ret)
	{
		T *t = (T*)obj;
		ret.init(m_propType, &(t->*m_d), Variant::InitRef);
		return true;
	}

	virtual bool setProperty(Object *obj, const ConstRef &arg)
	{
		T *t = (T*)obj;
		return arg.castTo(m_propType, &(t->*m_d));
	}

private:
	DataType m_d;
};

//--------------------------------------------------------------------------
// template Property_
//--------------------------------------------------------------------------

template <typename T, typename GetType, typename SetType>
class Property_ : public Member {
public:
	typedef GetType (T::*GetFunc)() const;
	typedef void (T::*SetFunc)(SetType);

	Property_(const char *name, GetFunc getfunc, SetFunc setfunc)
		: Member(name, Member::kPropertyType)
		, m_getFunc(getfunc)
		, m_setFunc(setfunc)
	{
		Variant::TypeId getTypeId = GetVariantType_<remove_const_reference<GetType>::type>();
		Variant::TypeId setTypeId = GetVariantType_<remove_const_reference<SetType>::type>();
		AX_ASSERT(getTypeId == setTypeId);
		m_propType = getTypeId;
	}

	Property_(const char *name, GetFunc getfunc)
		: Member(name, Member::kPropertyType)
		, m_getFunc(getfunc)
		, m_setFunc(nullptr)
	{
		m_propType = GetVariantType_<GetType>();
	}

	virtual bool isConst() const
	{ return m_setFunc == nullptr; }

	virtual bool getProperty(const Object *obj, Variant &ret)
	{
		T *t = (T*)obj;
		ret = (t->*m_getFunc)();
		return true;
	}

	virtual bool setProperty(Object *obj, const ConstRef& arg)
	{
		if (!m_setFunc)
			return false;

		T *t = (T*)obj;
		if (arg.getTypeId() == m_propType) {
			(t->*m_setFunc)(*reinterpret_cast<const GetType*>(arg.getPointer()));
			return true;
		}

		AX_DECL_STACK_VARIANT(casted, m_propType);
		bool v = arg.castTo(casted);
		if (!v) return false;

		(t->*m_setFunc)(*reinterpret_cast<const GetType*>(casted.getPointer()));
		return true;
	}

private:
	GetFunc m_getFunc;
	SetFunc m_setFunc;
};

	

//--------------------------------------------------------------------------
// template ReturnSpecialization
//--------------------------------------------------------------------------

#define P(x) stack[x].cast<remove_const_reference<P##x>::type>()
#define ARG(x) *reinterpret_cast<std::tr1::add_pointer<std::tr1::add_const<remove_const_reference<Arg##x>::type>::type>::type>(argv[x])
#define RET *reinterpret_cast<std::tr1::add_pointer<Rt>::type>(ret)

template <typename Rt>
struct ReturnSpecialization {
	template <typename T>
	static int xcall(T *object, Rt (T::*func)(), void *ret, const void **argv)
	{
		if (ret) {
			RET = (object->*func)();
			return 1;
		} else {
			(object->*func)();
			return 0;
		}
	}

	template< typename T, typename Arg0 >
	static int xcall(T *object, Rt (T::*func)(Arg0), void *ret, const void **argv)
	{
		if (ret) {
			RET = (object->*func)(ARG(0));
			return 1;
		} else {
			(object->*func)(ARG(0));
			return 0;
		}
	}

	template< typename T, typename Arg0, typename Arg1 >
	static int xcall(T *object, Rt (T::*func)(Arg0, Arg1), void *ret, const void **argv)
	{
		if (ret) {
			RET = (object->*func)(ARG(0), ARG(1));
			return 1;
		} else {
			(object->*func)(ARG(0), ARG(1));
			return 0;
		}
	}

	template< typename T, typename Arg0, typename Arg1, typename Arg2 >
	static int xcall(T *object, Rt (T::*func)(Arg0, Arg1, Arg2), void *ret, const void **argv)
	{
		if (ret) {
			RET = (object->*func)(ARG(0), ARG(1), ARG(2));
			return 1;
		} else {
			(object->*func)(ARG(0), ARG(1), ARG(2));
			return 0;
		}
	}

	template< typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3 >
	static int xcall(T *object, Rt (T::*func)(Arg0, Arg1, Arg2, Arg3), void *ret, const void **argv)
	{
		if (ret) {
			RET = (object->*func)(ARG(0), ARG(1), ARG(2), ARG(3));
			return 1;
		} else {
			(object->*func)(ARG(0), ARG(1), ARG(2), ARG(3));
			return 0;
		}
	}

	template< typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4 >
	static int xcall(T *object, Rt (T::*func)(Arg0, Arg1, Arg2, Arg3, Arg4), void *ret, const void **argv)
	{
		if (ret) {
			RET = (object->*func)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4));
			return 1;
		} else {
			(object->*func)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4));
			return 0;
		}
	}
};

template <>
struct ReturnSpecialization<void> {
	template <typename T>
	static int xcall(T *object, void (T::*func)(), void *ret, const void **argv)
	{
		(object->*func)();
		return 0;
	}

	template<typename T, typename Arg0>
	static int xcall(T *object, void (T::*func)(Arg0), void *ret, const void **argv)
	{
		(object->*func)(ARG(0));
		return 0;
	}

	template<typename T, typename Arg0, typename Arg1>
	static int xcall(T *object, void (T::*func)(Arg0, Arg1), void *ret, const void **argv)
	{
		(object->*func)(ARG(0), ARG(1));
		return 0;
	}

	template<typename T, typename Arg0, typename Arg1, typename Arg2>
	static int xcall(T *object, void (T::*func)(Arg0, Arg1, Arg2), void *ret, const void **argv)
	{
		(object->*func)(ARG(0), ARG(1), ARG(2));
		return 0;
	}

	template<typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
	static int xcall(T *object, void (T::*func)(Arg0, Arg1, Arg2, Arg3), void *ret, const void **argv)
	{
		(object->*func)(ARG(0), ARG(1), ARG(2), ARG(3));
		return 0;
	}

	template<typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	static int xcall(T *object, void (T::*func)(Arg0, Arg1, Arg2, Arg3, Arg4), void *ret, const void **argv)
	{
		(object->*func)(ARG(0), ARG(1), ARG(2), ARG(3), ARG(4));
		return 0;
	}
};
#undef P
#undef ARG
#undef RET

//--------------------------------------------------------------------------
// template Method_
//--------------------------------------------------------------------------

template< typename Signature >
class Method_ : public Member {
public:
	AX_STATIC_ASSERT(0);
};

template<typename Rt, typename T>
class Method_<Rt (T::*)()> : public Member {
public:
	typedef Rt (T::*FunctionType)();

	Method_(const char *name, FunctionType m)
		: Member(name, Member::kMethodType)
		, m_m(m)
	{
		m_argc = 0;
		m_returnType = GetVariantType_<Rt>();
		m_scriptClosure = ScriptSystem::createMetaClosure(this);
	}

	virtual int invoke(Object *obj, void *ret, const void **argv)
	{
		T *realobj = dynamic_cast<T*>(obj);
		AX_ASSERT(realobj);

		return ReturnSpecialization<Rt>::xcall(realobj, m_m, ret, argv);
	}

private:
	String m_name;
	FunctionType m_m;
};

template< typename Rt, typename T, typename Arg0 >
class Method_<Rt (T::*)(Arg0)> : public Member {
public:
	typedef Rt (T::*FunctionType)(Arg0);

	Method_(const char *name, FunctionType m)
		: Member(name, Member::kMethodType)
		, m_m(m)
	{
		m_argc = 1;
		m_returnType = GetVariantType_<Rt>();
		m_argsType[0] = GetVariantType_<remove_const_reference<Arg0>::type>();
		m_scriptClosure = ScriptSystem::createMetaClosure(this);
	}

	virtual int invoke(Object *obj, void *ret, const void **argv)
	{
		T *realobj = dynamic_cast<T*>(obj);
		AX_ASSERT(realobj);

		return ReturnSpecialization<Rt>::xcall(realobj, m_m, ret, argv);
	}

private:
	String m_name;
	FunctionType m_m;
};

template< typename Rt, typename T, typename Arg0, typename Arg1 >
class Method_<Rt (T::*)(Arg0,Arg1)> : public Member {
public:
	typedef Rt (T::*FunctionType)(Arg0,Arg1);

	Method_(const char *name, FunctionType m)
		: Member(name, Member::kMethodType)
		, m_m(m)
	{
		m_argc = 2;
		m_returnType = GetVariantType_<Rt>();
		m_argsType[0] = GetVariantType_<remove_const_reference<Arg0>::type>();
		m_argsType[1] = GetVariantType_<remove_const_reference<Arg1>::type>();
		m_scriptClosure = ScriptSystem::createMetaClosure(this);
	}

	virtual int invoke(Object *obj, void *ret, const void **argv)
	{
		T *realobj = dynamic_cast<T*>(obj);
		AX_ASSERT(realobj);

		return ReturnSpecialization<Rt>::xcall(realobj, m_m, ret, argv);
	}

private:
	String m_name;
	FunctionType m_m;
};

template< typename Rt, typename T, typename Arg0, typename Arg1, typename Arg2 >
class Method_<Rt (T::*)(Arg0,Arg1,Arg2)> : public Member {
public:
	typedef Rt (T::*FunctionType)(Arg0,Arg1,Arg2);

	Method_(const char *name, FunctionType m)
		: Member(name, Member::kMethodType)
		, m_m(m)
	{
		m_argc = 3;
		m_returnType = GetVariantType_<Rt>();
		m_argsType[0] = GetVariantType_<remove_const_reference<Arg0>::type>();
		m_argsType[1] = GetVariantType_<remove_const_reference<Arg1>::type>();
		m_argsType[2] = GetVariantType_<remove_const_reference<Arg2>::type>();
		m_scriptClosure = ScriptSystem::createMetaClosure(this);
	}

	virtual int invoke(Object *obj, void *ret, const void **argv)
	{
		T *realobj = dynamic_cast<T*>(obj);
		AX_ASSERT(realobj);

		return ReturnSpecialization<Rt>::xcall(realobj, m_m, ret, argv);
	}

private:
	String m_name;
	FunctionType m_m;
};

template< typename Rt, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3 >
class Method_<Rt (T::*)(Arg0,Arg1,Arg2,Arg3)> : public Member {
public:
	typedef Rt (T::*FunctionType)(Arg0,Arg1,Arg2,Arg3);

	Method_(const char *name, FunctionType m)
		: Member(name, Member::kMethodType)
		, m_m(m)
	{
		m_argc = 4;
		m_returnType = GetVariantType_<Rt>();
		m_argsType[0] = GetVariantType_<remove_const_reference<Arg0>::type>();
		m_argsType[1] = GetVariantType_<remove_const_reference<Arg1>::type>();
		m_argsType[2] = GetVariantType_<remove_const_reference<Arg2>::type>();
		m_argsType[3] = GetVariantType_<remove_const_reference<Arg3>::type>();
		m_scriptClosure = ScriptSystem::createMetaClosure(this);
	}

	virtual int invoke(Object *obj, void *ret, const void **argv)
	{
		T *realobj = dynamic_cast<T*>(obj);
		AX_ASSERT(realobj);

		return ReturnSpecialization<Rt>::xcall(realobj, m_m, ret, argv);
	}

private:
	String m_name;
	FunctionType m_m;
};

template< typename Rt, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4 >
class Method_<Rt (T::*)(Arg0,Arg1,Arg2,Arg3,Arg4)> : public Member {
public:
	typedef Rt (T::*FunctionType)(Arg0,Arg1,Arg2,Arg3,Arg4);

	Method_(const char *name, FunctionType m)
		: Member(name, Member::kMethodType)
		, m_m(m)
	{
		m_argc = 5;
		m_returnType = GetVariantType_<Rt>();
		m_argsType[0] = GetVariantType_<remove_const_reference<Arg0>::type>();
		m_argsType[1] = GetVariantType_<remove_const_reference<Arg1>::type>();
		m_argsType[2] = GetVariantType_<remove_const_reference<Arg2>::type>();
		m_argsType[3] = GetVariantType_<remove_const_reference<Arg3>::type>();
		m_argsType[4] = GetVariantType_<remove_const_reference<Arg4>::type>();
		m_scriptClosure = ScriptSystem::createMetaClosure(this);
	}

	virtual int invoke(Object *obj, void *ret, const void **argv)
	{
		T *realobj = dynamic_cast<T*>(obj);
		AX_ASSERT(realobj);

		return ReturnSpecialization<Rt>::xcall(realobj, m_m, ret, argv);
	}

private:
	String m_name;
	FunctionType m_m;
};
#undef ARG

//--------------------------------------------------------------------------
// class MetaInfo
//--------------------------------------------------------------------------
#if 0
class ClassInfo;
#else
class SqClass;
#endif
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
#if 0
	ClassInfo *m_classInfo;
#else
	SqClass *m_sqClass;
#endif
	MemberSeq m_members;
	MemberDict m_memberDict;
};

inline MetaInfo::MetaInfo(const char *classname, MetaInfo *base)
	: m_name(classname)
	, m_base(base)
#if 0
	, m_classInfo(nullptr)
#else
	, m_sqClass(0)
#endif
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

#if 0
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
#endif

//--------------------------------------------------------------------------
class SqProperty;
class SqPropGroup;
class SqClass;

class AX_API SqProperty : public Member
{
	friend class SqClass;

public:
	SqProperty(const sqObject &key, const sqObject &val, const sqObject &attr);
	SqProperty(const char *name, Kind kind);

	const String &getRealName() const { return m_realName; }
	String getGroupName() const { if (m_group) return m_group->getRealName(); else return String(); }

private:
	String m_realName;
	Variant m_default;
	SqProperty *m_group;
};

typedef Sequence<SqProperty*> SqProperties;
typedef Dict<String,SqProperty*> SqPropertyDict;

//--------------------------------------------------------------------------
class SqPropGroup {
	String m_name;
	SqProperties m_properties;
};
typedef Sequence<SqPropGroup> SqPropGroups;

//--------------------------------------------------------------------------
class AX_API SqClass
{
	friend class ScriptSystem;

public:
	SqClass(const String &name);

	String getName() const { return m_name; }
	Member *findMember(const char *name) const {
		SqPropertyDict::const_iterator it = m_propDict.find(name);

		if (it != m_propDict.end())
			return it->second;

		return 0;
	}

	const SqProperties& getMembers() const { return m_properties; }

protected:
	void addProperty(const sqObject &key, const sqObject &val, const sqObject &attr);

private:
	String m_name;
	String m_cppName;
	MetaInfo *m_metaInfo;

	SqProperties m_properties;
	SqPropGroups m_groups;
	SqPropertyDict m_propDict;
};

AX_END_NAMESPACE

#endif // AX_METAINFO_H
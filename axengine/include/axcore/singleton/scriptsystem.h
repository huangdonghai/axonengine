/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/




#ifndef AX_SCRIPTSYSTEM_H
#define AX_SCRIPTSYSTEM_H

#define AX_DECLARE_CLASS(classname, baseclass, scriptname) public:			\
	typedef classname ThisClass;											\
	typedef baseclass BaseClass;											\
	virtual ::Axon::TypeInfo* classname::getTypeInfo() const {				\
		return classname::registerTypeInfo();								\
	}																		\
	static ::Axon::TypeInfo* classname::registerTypeInfo() {				\
		static ::Axon::TypeInfo* typeinfo;									\
		if (!typeinfo) {													\
			typeinfo = new ::Axon::TypeInfo_<classname>(scriptname, BaseClass::registerTypeInfo()); \

#define AX_CONSTPROP(name) typeinfo->addProperty(#name, &ThisClass::get_##name);
#define AX_PROP(name) typeinfo->addProperty(#name, &ThisClass::get_##name, &ThisClass::set_##name);
#define AX_SIMPLEPROP(name) typeinfo->addProperty(#name, &ThisClass::m_##name);

#define AX_METHOD(name) typeinfo->addMethod(#name, &ThisClass::name);

#define AX_END_CLASS()													\
				g_scriptSystem->registerType(typeinfo);					\
			}															\
		return typeinfo;												\
	}																	\


#define AX_REGISTER_CLASS(cppname) cppname::registerTypeInfo();


namespace Axon {

	class Object;
	class Variant;

	//--------------------------------------------------------------------------
	// class LuaTable
	//--------------------------------------------------------------------------

	struct AX_API LuaTable
	{
	public:
		LuaTable(int index);

		void beginRead() const;
		Variant get(const String& n) const;
		void set(const String& n, const Variant& v);
		int getLength() const;
		// n is start from 0
		Variant get(int n) const;
		void endRead() const;

		void beginIterator() const;
		bool nextIterator(Variant& k, Variant& v) const;
		void endIterator() const;

		Vector3 toVector3() const;
		Rgb toColor() const;
		Point toPoint() const;
		Rect toRect() const;
		Object* toObject() const;

	public:
		int m_index;
		mutable bool m_isReading;
		mutable bool m_isIteratoring;
		mutable int m_stackTop;
	};

	//--------------------------------------------------------------------------
	// class Variant
	//--------------------------------------------------------------------------

	class AX_API Variant
	{
	public:
		enum Type {
			kEmpty, kBool, kInt, kFloat, kString, kObject, kTable, kVector3, kColor, kPoint, kRect, kAffineMat, kMaxType
		};

		enum {
			MINIBUF_SIZE = 4 * sizeof(float)
		};

		Type getType() const { return type; }

		// constructor
		Variant();
		Variant(bool v);
		Variant(int v);
		Variant(double v);
		Variant(const String& v);
		Variant(const char* v);
		Variant(Object* v);
		Variant(const Vector3& v);
		Variant(const Point& v);
		Variant(const Rect& v);
		Variant(const Rgb& v);
		Variant(const Variant& v);
		Variant(const LuaTable& table);
		Variant(const AffineMat& matrix);
		~Variant();

		void clear();
		operator bool() const;
		operator int() const;
		operator float() const;
		operator double() const;
		operator String() const;
		operator Object*() const;
		operator Vector3() const;
		operator Point() const;
		operator Rect() const;
		operator Rgb() const;
		operator LuaTable() const;
		Variant& operator=(const Variant& v);
		operator AffineMat() const;

		void set(int v);
		void set(float v);
		void set(double v);
		void set(const char* v);
		void set(const String& v);
		void set(const Variant& v);
		void set(Object* v);

		bool toBool() const { return operator bool(); }
		int toInt() const { return operator int(); }
		float toFloat() const { return operator float(); }

		String toScript() const;

		String toString() const;
		void fromString(Type t, const char* str);

		template<class Q>
		Q cast() {
			return variant_cast<Q>(*this);
		}

		// member variable
		Type type;
		union {
			bool boolval;
			int intval;
			double realval;
			Object* obj;
			String* str;
			AffineMat* mtr;
			byte_t minibuf[MINIBUF_SIZE];
		};
	};

	inline Variant::Variant() : type(kEmpty) {}

	inline Variant::Variant(bool v) : type(kBool), boolval(v) {}

	inline Variant::Variant(int v) : type(kInt), intval(v) {}

	inline Variant::Variant(double v) : type(kFloat), realval(v) {}

	inline Variant::Variant(const String& v) : type(kString), str(new String(v)) {}

	inline Variant::Variant(const char* v) : type(kString), str(new String(v)) {}

	inline Variant::Variant(Object* v) : type(kObject), obj(v) {}

	inline Variant::Variant(const Vector3& v) : type(kVector3) { new (minibuf) Vector3(v); }

	inline Variant::Variant(const Point& v) : type(kPoint) { new (minibuf) Point(v); }

	inline Variant::Variant(const Rect& v) : type(kRect) { new (minibuf) Rect(v); }
	
	inline Variant::Variant(const Rgb& v) : type(kColor) { new (minibuf) Rgb(v); }

	inline Variant::Variant(const AffineMat& v) : type(kAffineMat), mtr(new AffineMat(v)) {}

	inline Variant::Variant(const Variant& v) : type(v.type), realval(v.realval) {
		if (type == kString) {
			str = new String(*(String*)v.str);
			return;
		}

		if (type == kAffineMat) {
			mtr = new AffineMat(*(AffineMat*)v.mtr);
			return;
		}

		::memcpy(minibuf, v.minibuf, MINIBUF_SIZE);
	}

	inline Variant::Variant(const LuaTable& rhs) : type(kTable) {
		new(minibuf) LuaTable(rhs);
	}

	inline Variant::~Variant() { clear(); }

	inline void Variant::clear() {
		if (type == kString) {
			delete str;
		} else if (type == kAffineMat) {
			delete mtr;
		}
		type = kEmpty;
	}

	inline Variant::operator bool() const {
		switch (type) {
			case kBool:
				return boolval;
			case kInt:
				return intval != 0;
			case kFloat:
				return realval != 0;
			default:
				return 0;
		}
	}

	inline Variant::operator int() const {
		switch (type) {
			case kBool:
				return boolval;
			case kInt:
				return intval;
			case kFloat:
				return (int)realval;
			default:
				return 0;
		}
	}

	inline Variant::operator float() const {
		switch (type) {
			case kBool:
				return boolval;
			case kInt:
				return (float)intval;
			case kFloat:
				return (float)realval;
			default:
				return 0;
		}
		return 0;
	}

	inline Variant::operator double() const {
		switch (type) {
			case kBool:
				return boolval;
			case kInt:
				return intval;
			case kFloat:
				return realval;
			default:
				return 0;
		}
		return 0;
	}

	inline Variant::operator Vector3() const {
		switch (type) {
		case kTable:
			return ((LuaTable*)minibuf)->toVector3();
			break;
		case kVector3:
			return *(Vector3*)minibuf;
			break;
		}

		return Vector3();
	}

	inline Variant::operator Point() const {
		switch (type) {
		case kTable:
			return ((LuaTable*)minibuf)->toPoint();
			break;
		case kPoint:
			return *(Point*)minibuf;
			break;
		}

		return Point();
	}

	inline Variant::operator Rect() const {
		switch (type) {
		case kTable:
			return ((LuaTable*)minibuf)->toRect();
			break;
		case kRect:
			return *(Rect*)minibuf;
			break;
		}

		return Rect();
	}

	inline Variant::operator AffineMat() const {
		switch (type) {
		case kTable:
			break;
		case kRect:
			return *mtr;
			break;
		}

		return AffineMat();
	}

	inline Variant::operator Rgb() const {
		switch (type) {
		case kTable:
			return ((LuaTable*)minibuf)->toColor();
			break;
		case kColor:
			return *(Rgb*)minibuf;
			break;
		}

		return Vector3();
	}

	inline Variant::operator LuaTable() const {
		if (type != kTable) {
			Errorf("can't convert other type to table");
		}

		return *((LuaTable*)minibuf);
	}

	inline Variant::operator Object*() const{
		switch (type) {
			case kTable:
				return ((LuaTable*)minibuf)->toObject();
			case kObject:
				return obj;
			default:
				return 0;
		}
	}

	inline Variant& Variant::operator=(const Variant& v) {
		set(v);
		return *this;
	}


	inline Variant::operator String() const {
		if (type==kString) {
			return *str;
		}
		return "";
	}

	inline void Variant::set(int v) {
		clear();
		type = kInt;
		intval = v;
	}

	inline void Variant::set(float v) {
		clear();
		type = kFloat;
		realval = (double)v;
	}

	inline void Variant::set(double v) {
		clear();
		type = kFloat;
		realval = v;
	}

	inline void Variant::set(const char* v) {
		clear();
		type = kString;
		str = new String(v);
	}

	inline void Variant::set(const String& v) {
		clear();
		type = kString;
		str = new String(v);
	}

	inline void Variant::set(const Variant& v) {
		clear();
		type = v.type;
		::memcpy(minibuf, v.minibuf, MINIBUF_SIZE);
		if (type == kString) {
			str = new String(*(String*)v.str);
		}
	}

	inline void Variant::set(Object *v) {
		clear();
		type = kObject;
		obj = v;
	}
	inline String Variant::toScript() const {
		String script;
		if (type == kInt) {
			StringUtil::sprintf(script, "%d", intval);
		} else if (type == kFloat) {
			StringUtil::sprintf(script, "%f", realval);
		} else if (type == kObject) {
			Errorf("");
		} else if (type == kString) {
			script = "\"";
			script += *str;
			script += "\"";
		}
		return script;
	}

	inline String Variant::toString() const {
		String result;
		switch (type) {
		case kEmpty:
			break;
		case kBool:
			StringUtil::sprintf(result, "%d", boolval);
			break;
		case kInt:
			StringUtil::sprintf(result, "%d", intval);
			break;
		case kFloat:
			StringUtil::sprintf(result, "%f", realval);
			break;
		case kString:
			result = *str;
			break;
		case kObject:
			break;
		case kTable:
			break;
		case kVector3:
			{
				Vector3 v = *this;
				StringUtil::sprintf(result, "%f %f %f", v.x, v.y, v.z);
				break;
			}
		case kColor:
			{
				Rgb v = *this;
				StringUtil::sprintf(result, "%d %d %d", v.r, v.g, v.b);
				break;
			}
		case kPoint:
			{
				Point v = *this;
				StringUtil::sprintf(result, "%d %d", v.x, v.y);
				break;
			}
		case kRect:
			{
				Rect v = *this;
				StringUtil::sprintf(result, "%d %d %d %d", v.x, v.y, v.width, v.height);
				break;
			}
		case kAffineMat:
			{
				return mtr->toString();
			}
		default:
			AX_NO_DEFAULT;
		}
		return result;
	}

	inline void Variant::fromString(Type t, const char* str) {
		clear();

		switch (t) {
		case kEmpty:
			break;
		case kBool:
			this->set(atoi(str) ? true : false);
			break;
		case kInt:
			this->set(atoi(str));
			break;
		case kFloat:
			this->set(atof(str));
			break;
		case kString:
			this->set(str);
			break;
		case kObject:
			break;
		case kTable:
			break;
		case kVector3:
			{
				Vector3 v;
				v.fromString(str);
				this->set(v);
				break;
			}
		case kColor:
			{
				Rgb v;
				v.fromString(str);
				this->set(v);
				break;
			}
		case kPoint:
			{
				Point v;
				v.fromString(str);
				this->set(v);
				break;
			}
		case kRect:
			{
				Rect v; v.fromString(str); this->set(v);
				break;
			}
		case kAffineMat:
			{
				AffineMat v;
				v.fromString(str);
				this->set(v);
				break;
			}
		default:
			AX_NO_DEFAULT;
		}
	}


	typedef Sequence<Variant> VariantSeq;

	template< typename T >
	inline Variant::Type GetVariantType_() {
		// must be specialized, or raise a static error
		AX_STATIC_ASSERT(0);
	}

	template<>
	inline Variant::Type GetVariantType_<int>() {
		return Variant::kInt;
	}

	template<>
	inline Variant::Type GetVariantType_<bool>() {
		return Variant::kBool;
	}

	template<>
	inline Variant::Type GetVariantType_<float>() {
		return Variant::kFloat;
	}

	template<>
	inline Variant::Type GetVariantType_<String>() {
		return Variant::kString;
	}

	template<>
	inline Variant::Type GetVariantType_<Vector3>() {
		return Variant::kVector3;
	}

	template<>
	inline Variant::Type GetVariantType_<Rgb>() {
		return Variant::kColor;
	}

	template<>
	inline Variant::Type GetVariantType_<Point>() {
		return Variant::kPoint;
	}

	template<>
	inline Variant::Type GetVariantType_<Rect>() {
		return Variant::kRect;
	}

	template<>
	inline Variant::Type GetVariantType_<Object*>() {
		return Variant::kObject;
	}

	template<>
	inline Variant::Type GetVariantType_<AffineMat>() {
		return Variant::kAffineMat;
	}

	template< class T >
	struct variant_cast_helper {
		T doCast(const Variant& v) {
			return v;
		}
	};

	template<>
	struct variant_cast_helper<Object*> {
		Object* doCast(const Variant& v) {
			return v.operator Object*();
		}
	};

	template<class T>
	struct variant_cast_helper<T*> {
		T* doCast(const Variant& v) {
			Object* obj = variant_cast_helper<Object*>().doCast(v);
			return object_cast<T*>(obj);
		}
	};

	template<class T>
	T variant_cast(const Variant& v) {
		return variant_cast_helper<T>().doCast(v);
	}
#if 0
	template<>
	Object* variant_cast<Object*>(const Variant& v) {
		return v.operator Object*();
	}

	template<class T>
	T* variant_cast<T*>(const Variant& v) {
		Object* obj = variant_cast<Object*>(v);
		return 0;
	}
#endif
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

		typedef std::pair<String,int>	EnumItem;
		typedef Sequence<EnumItem>		EnumItems;

		Member(const char* name, Type t) : m_name(name), m_type(t), m_propKind(Variant::kEmpty) {}

		bool isProperty() const { return m_type == kPropertyType; }
		bool isMethod() const { return m_type == kMethodType; }
		Type getType() const { return m_type; }
		const char* getName() const { return m_name; }
		int getNumParams() const { return m_numParams; }
		Variant::Type getPropType() const { return m_propType; }
		int getPropKind() const { return m_propKind; }
		const EnumItems& getEnumItems() const { return m_enumItems; }

		// method
		virtual int invoke(Object* obj, VariantSeq& stack) { AX_ASSERT(0);return 0;}

		// property
		virtual void setProperty(Object* obj, const Variant& val) { AX_ASSERT(0); }
		virtual Variant getProperty(const Object* obj) { AX_ASSERT(0); return Variant(); }
		virtual void *getPropertyPointer(const Object* obj) { AX_ASSERT(0); return 0; }
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
		const char* m_name;
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

		SimpleProp_(const char* name, DataType d);

		virtual void setProperty(Object* obj, const Variant& val);
		virtual Variant getProperty(const Object* obj);
		virtual void *getPropertyPointer(const Object* obj);
		virtual bool isConst() const { return false; }
		virtual bool isAnimatable() const { return true; }

	private:
		DataType m_d;
	};

	template< class T, class M >
	SimpleProp_<T,M>::SimpleProp_(const char* name, DataType d) : Member(name, Member::kPropertyType) {
		m_d = d;
		m_propType = GetVariantType_<M>();
	}

	template< class T, class M >
	void SimpleProp_<T,M>::setProperty(Object* obj, const Variant& val) {
		T* t = (T*)obj;
		t->*m_d = val;
	}

	template< class T, class M >
	Variant SimpleProp_<T,M>::getProperty(const Object* obj) {
		T* t = (T*)obj;
		return t->*m_d;
	}

	template< class T, class M >
	void* SimpleProp_<T,M>::getPropertyPointer(const Object* obj) {
		T* t = (T*)obj;
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

		Property_(const char* name, GetFunc getfunc, SetFunc setfunc);
		Property_(const char* name, GetFunc getfunc);

		virtual void setProperty(Object* obj, const Variant& val);
		virtual Variant getProperty(const Object* obj);
		virtual bool isConst() const { return m_setFunc == nullptr; }

	private:
		GetFunc m_getFunc;
		SetFunc m_setFunc;
	};

	template< typename T, typename GetType, typename SetType >
	Property_<T,GetType,SetType>::Property_(const char* name, GetFunc getfunc, SetFunc setfunc)
		: Member(name, Member::kPropertyType)
		, m_getFunc(getfunc)
		, m_setFunc(setfunc)
	{
		m_propType = GetVariantType_<GetType>();
	}

	template< typename T, typename GetType, typename SetType >
	Property_<T,GetType,SetType>::Property_(const char* name, GetFunc getfunc)
		: Member(name, Member::kPropertyType)
		, m_getFunc(getfunc)
		, m_setFunc(nullptr)
	{
		m_propType = GetVariantType_<GetType>();
	}

	template< typename T, typename GetType, typename SetType >
	void Property_<T,GetType,SetType>::setProperty(Object* obj, const Variant& val) {
		if (!m_setFunc) return;
		T* t = (T*)obj;
		(t->*m_setFunc)(val);
	}

	template< typename T, typename GetType, typename SetType >
	Variant Property_<T,GetType,SetType>::getProperty(const Object* obj) {
		T* t = (T*)obj;
		return (t->*m_getFunc)();
	}


	//--------------------------------------------------------------------------
	// template ReturnSpecialization
	//--------------------------------------------------------------------------

#define P(x) stack[x].cast<remove_const_reference<P##x>::type>()

	template< typename Rt >
	struct ReturnSpecialization {
		template< typename T >
		static int call(T* object, Rt (T::*func)(), VariantSeq& stack) {
			Rt ret = (object->*func)();
			Variant val(ret);
			stack.push_back(val);
			return 1;
		}

		template< typename T, typename P0 >
		static int call(T* object, Rt (T::*func)(P0), VariantSeq& stack) {
			Rt ret = (object->*func)(P(0));
			Variant val(ret);
			stack.push_back(val);
			return 1;
		}

		template< typename T, typename P0, typename P1 >
		static int call(T* object, Rt (T::*func)(P0,P1), VariantSeq& stack) {
			Rt ret = (object->*func)(P(0),P(1));
			Variant val(ret);
			stack.push_back(val);
			return 1;
		}

		template< typename T, typename P0, typename P1, typename P2 >
		static int call(T* object, Rt (T::*func)(P0,P1,P2), VariantSeq& stack) {
			Rt ret = (object->*func)(P(0),P(1),P(2));
			Variant val(ret);
			stack.push_back(val);
			return 1;
		}

		template< typename T, typename P0, typename P1, typename P2, typename P3 >
		static int call(T* object, Rt (T::*func)(P0,P1,P2,P3), VariantSeq& stack) {
			Rt ret = (object->*func)(stack(0),stack(1),stack(2),stack(3));
			Variant val(ret);
			stack.push_back(val);
			return 1;
		}

		template< typename T, typename P0, typename P1, typename P2, typename P3, typename P4 >
		static int call(T* object, Rt (T::*func)(P0,P1,P2,P3,P4), VariantSeq& stack) {
			Rt ret = (object->*func)(P(0),P(1),P(2),P(3),P(4));
			Variant val(ret);
			stack.push_back(val);
			return 1;
		}

	};

	template<>
	struct ReturnSpecialization< void > {
		template< typename T >
		static int call(T* object, void (T::*func)(), VariantSeq& stack) {
			(object->*func)();
			return 0;
		}

		template< typename T, typename P0 >
		static int call(T* object, void (T::*func)(P0), VariantSeq& stack) {
			(object->*func)(P(0));
			return 0;
		}

		template< typename T, typename P0, typename P1 >
		static int call(T* object, void (T::*func)(P0,P1), VariantSeq& stack) {
			(object->*func)(P(0),P(1));
			return 0;
		}

		template< typename T, typename P0, typename P1, typename P2 >
		static int call(T* object, void (T::*func)(P0,P1,P2), VariantSeq& stack) {
			(object->*func)(P(0),P(1),P(2));
			return 0;
		}

		template< typename T, typename P0, typename P1, typename P2, typename P3 >
		static int call(T* object, void (T::*func)(P0,P1,P2,P3), VariantSeq& stack) {
			(object->*func)(P(0),P(1),P(2),P(3));
			return 0;
		}

		template< typename T, typename P0, typename P1, typename P2, typename P3, typename P4 >
		static int call(T* object, void (T::*func)(P0,P1,P2,P3,P4), VariantSeq& stack) {
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

		Method_(const char* name, FunctionType m)
			: Member(name, Member::kMethodType)
			, m_m(m)
		{
			m_numParams = 0;
		}

		virtual int invoke(Object* obj, VariantSeq& stack) {
			AX_ASSERT(stack.size() == getNumParams());
			T* realobj = dynamic_cast<T*>(obj);
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

		Method_(const char* name, FunctionType m)
			: Member(name, Member::kMethodType)
			, m_m(m)
		{
			m_numParams = 1;
		}

		virtual int invoke(Object* obj, VariantSeq& stack) {
			AX_ASSERT(stack.size() == getNumParams());
			T* realobj = dynamic_cast<T*>(obj);
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

		Method_(const char* name, FunctionType m)
			: Member(name, Member::kMethodType)
			, m_m(m)
		{
			m_numParams = 2;
		}

		virtual int invoke(Object* obj, VariantSeq& stack) {
			AX_ASSERT(stack.size() == getNumParams());
			T* realobj = dynamic_cast<T*>(obj);
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

		Method_(const char* name, FunctionType m)
			: Member(name, Member::kMethodType)
			, m_m(m)
		{
			m_numParams = 3;
		}

		virtual int invoke(Object* obj, VariantSeq& stack) {
			AX_ASSERT(stack.size() == getNumParams());
			T* realobj = dynamic_cast<T*>(obj);
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

		Method_(const char* name, FunctionType m)
			: Member(name, Member::kMethodType)
			, m_m(m)
		{
			m_numParams = 4;
		}

		virtual int invoke(Object* obj, VariantSeq& stack) {
			AX_ASSERT(stack.size() == getNumParams());
			T* realobj = dynamic_cast<T*>(obj);
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

		Method_(const char* name, FunctionType m)
			: Member(name, Member::kMethodType)
			, m_m(m)
		{
			m_numParams = 5;
		}

		virtual int invoke(Object* obj, VariantSeq& stack) {
			AX_ASSERT(stack.size() == getNumParams());
			T* realobj = dynamic_cast<T*>(obj);
			AX_ASSERT(realobj);
			return ReturnSpecialization<Rt>::call(realobj, m_m, stack);
		}

	private:
		String m_name;
		FunctionType m_m;
	};


	//--------------------------------------------------------------------------
	// class TypeInfo
	//--------------------------------------------------------------------------

	class AX_API TypeInfo
	{
	public:
		friend class ScriptSystem;

		TypeInfo(const char* classname, TypeInfo* base);
		virtual ~TypeInfo();

		template< typename T, typename GetType, typename SetType >
		TypeInfo& addProperty(const char* name, GetType (T::*get)() const, void (T::*set)(SetType));

		template< typename T, typename GetType >
		TypeInfo& addProperty(const char* name, GetType (T::*get)() const);

		template< typename T, typename M >
		TypeInfo& addProperty(const char* name, M (T::*d));

		template< typename Signature >
		TypeInfo& addMethod(const char* name, Signature m);

		Member* findMember(const char* name) const;
		TypeInfo* getBaseTypeInfo() const;

		const char* getTypeName() const;
		const MemberSeq& getMembers() const;
		const String& getObjNamespace() const { return m_objNamespace; }

		virtual Object* createObject() = 0;

	protected:
		void addMember(Member* member);

	private:
		const char* m_typeName;
		TypeInfo* m_baseTypeInfo;
		MemberSeq m_members;
		MemberDict m_memberDict;
		String m_objNamespace;
	};

	inline TypeInfo::TypeInfo(const char* classname, TypeInfo* base)
		: m_typeName(classname)
		, m_baseTypeInfo(base)
	{}

	inline TypeInfo::~TypeInfo() {}

	template< typename T, typename GetType, typename SetType >
	TypeInfo& TypeInfo::addProperty(const char* name, GetType (T::*get)() const, void (T::*set)(SetType)) {
		Member* member = new Property_<T,GetType,SetType>(name, get, set);
		addMember(member);
		return *this;
	}

	template< typename T, typename GetType >
	TypeInfo& TypeInfo::addProperty(const char* name, GetType (T::*get)() const) {
		Member* member = new Property_<T,GetType,GetType>(name, get);
		addMember(member);
		return *this;
	}

	template< typename T, typename M >
	TypeInfo& TypeInfo::addProperty(const char* name, M (T::*d)) {
		Member* member = new SimpleProp_<T,M>(name, d);
		addMember(member);
		return *this;
	}


	template< typename Signature >
	TypeInfo& TypeInfo::addMethod(const char* name, Signature m) {
		Member* member = new Method_<Signature>(name, m);
		addMember(member);
		return *this;
	}

	inline void TypeInfo::addMember(Member* member) {
		m_members.push_back(member);
		m_memberDict[member->getName()] = member;
	}

	inline Member* TypeInfo::findMember(const char* name) const {
		MemberDict::const_iterator it = m_memberDict.find(name);

		if (it != m_memberDict.end())
			return it->second;
		return nullptr;
	}

	inline TypeInfo* TypeInfo::getBaseTypeInfo() const {
		return m_baseTypeInfo;
	}

	inline const char* TypeInfo::getTypeName() const {
		return m_typeName;
	}

	inline const MemberSeq& TypeInfo::getMembers() const {
		return m_members;
	}

	//--------------------------------------------------------------------------
	// class TypeInfo_
	//--------------------------------------------------------------------------
	template< class T >
	class TypeInfo_ : public TypeInfo {
	public:
		TypeInfo_(const char* classname, TypeInfo* base)
			: TypeInfo(classname, base)
		{}
		virtual ~TypeInfo_() {}

		virtual Object* createObject() {
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

		virtual void setProperty(Object* obj, const Variant& val);
		virtual Variant getProperty(const Object* obj);
		bool grouped() const { return m_group || m_propKind == kGroup; }

	protected:
		void init();
		void initEnumItems();
		int checkNameKind();
		int checkTableKind(Variant& result);
		bool isStringType(int kind);

	public:
		String m_realName;
		String m_showName;
		Variant m_defaultValue;
		ScriptProp* m_group;
	};
	typedef Sequence<ScriptProp*>		ScriptPropSeq;
	typedef Dict<String,ScriptProp*> ScriptPropDict;


	//--------------------------------------------------------------------------
	// class ClassInfo
	//--------------------------------------------------------------------------

	class AX_API ClassInfo {
	public:
		ClassInfo() {}

		Variant getField(const String& field) const;
		void initScriptProps();

		String m_className;
		String m_baseClassName;
		TypeInfo* m_typeInfo;
		ScriptPropSeq m_scriptPropSeq;
		ScriptPropDict m_scriptProps;
	};


	//--------------------------------------------------------------------------
	// class Object
	//--------------------------------------------------------------------------

	class File;
	class AX_API Object {
	public:
		friend class ScriptSystem;

		Object();
		Object(const String& objname);
		virtual ~Object();

		virtual TypeInfo* getTypeInfo() const;
		static TypeInfo* registerTypeInfo();
		static TypeInfo* m_typeinfo;

		const ClassInfo* getClassInfo() const { return m_classInfo; }

		Member* findMember(const char* name) const;
		Variant getProperty(const char* name) const;
		bool setProperty(const char* name, const Variant& value);
		bool setProperty(const char* name, const char* value);
		void invokeMethod(const char* name, const Variant& arg1);
		void invokeMethod(const char* name, const Variant& arg1, const Variant& arg2);

		// properties
		void set_objectName(const String& name);
		String get_objectName() const;

		bool inherits(const char* cls) const;
		bool isClass(const char* classname) const;

		// read and write properties
		void writeProperties(File* f, int indent=0) const;
		void readProperties(const TiXmlElement* node);
		void copyPropertiesFrom(const Object* rhs);

		// runtime
		void setRuntime(const char* name, const Variant& val);
		Variant getRuntime(const char* name);

		void doPropertyChanged();

	protected:
		virtual void onPropertyChanged();

		void invoke_onInit();
		void invoke_onFinalize();
		void invoke_onPropertyChanged();

		void invokeCallback(const String& callback);
		void invokeCallback(const String& callback, const Variant& param);

		void setObjectName(const String& name, bool force);
		void resetObjectName();

		void raiseSignal(const String& signal, const Variant& param);

		// event
		void initClassInfo(const ClassInfo* ci);

	private:
		const ClassInfo* m_classInfo;
		String m_objectNamespace;
		String m_objectName;
	};


	//--------------------------------------------------------------------------
	// class ScriptSystem
	//--------------------------------------------------------------------------

	class ScriptSystem;
	extern AX_API ScriptSystem* g_scriptSystem;

	class AX_API ScriptSystem
	{
	public:
		friend class Object;

		ScriptSystem();
		~ScriptSystem();

		void initialize();
		void finalize();

		void executeString(const String& text);
		void executeString(const char* text);
		void executeFile(const String& filename);
		bool invokeLuaMethod(const char* methodName, VariantSeq& stack, int nResult);
		bool invokeLuaMethod(const char* method, Variant& arg1);
		bool invokeLuaScoped(const char* text,VariantSeq& stack, int nResult);
		String generateLuaString(const String& text);

		Object* createObject(const char* classname);
		Object* cloneObject(const Object* obj);
		Object* findObject(const String& objectname);

		// for automatic name gen
		int getNameIndex(const String& str) const;
		void updateNameIndex(const String& str);
		int nextNameIndex(const String& str);
		String generateObjectName(const String& str);

		void registerType(TypeInfo* typeinfo);
		void registerClass(const String& self, const String& base);

		void getClassList(const char* prefix, bool sort, StringSeq& result) const;

		//
		// read from lua state
		//
		void beginRead();
		Variant readField(const char* objname, const char* fieldname);
		Variant readField(const char* objname);
		void endRead();

		// connect signal and slot
		bool connect(Object* sender, const String& sig, Object* recevier, const String& slot);
		bool disconnect(Object* sender, const String& sig, Object* recevier, const String& slot);

		// immediately read
		Variant readFieldImmediately(const char* objname, const char* fieldname);

		const String& getPackagePath() { return m_packagePath; }

	protected:
		void setTypeInfoToClassInfo(const String& name, TypeInfo* ti);

	private:
		typedef Dict<String,ClassInfo*>	ClassInfoDict;
		ClassInfoDict m_classInfoReg;

		typedef Dict<const char*, TypeInfo*, hash_cstr, equal_cstr> TypeInfoDict;
		TypeInfoDict m_typeInfoReg;

		typedef Dict<String,int> StringIntDict;
		StringIntDict m_objectNameGen;

		String m_packagePath;

		bool m_isReading;
		int m_readTop;
	};


	template< class T >
	T object_cast(Object* obj) {
		if (obj->inherits(T(0)->registerTypeInfo()->getTypeName())) {
			return (T)obj;
		}
		return nullptr;
	}
}


#endif // AX_SCRIPTSYSTEM_H


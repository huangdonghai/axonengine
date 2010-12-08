#ifndef AX_VARIANT_H
#define AX_VARIANT_H

#define AX_ARG(x) ConstRef::make(x)
#define AX_RETURN_ARG(x) Ref::make(x)
#define AX_DECL_STACK_VARIANT(name, id) Variant name(id, Alloca(Variant::getTypeSize(id)), Variant::InitStack)
#define AX_INIT_STACK_VARIANT(name, id) name.init(id, Alloca(Variant::getTypeSize(id)), Variant::InitStack)


AX_BEGIN_NAMESPACE

class Object;
class Variant;
class ConstRef;
class Ref;

//--------------------------------------------------------------------------
// class Variant
//--------------------------------------------------------------------------
class AX_API Variant
{
	enum {
		MINIBUF_SIZE = 3 * sizeof(float)
	};

	enum StoreMode {
		StoreMinibuf, StoreHeap, StoreRef, StoreStack
	};

public:
	enum TypeId {
		kVoid, kBool, kInt, kFloat, kString, kObject, kVector3, kColor3, kPoint, kRect, kMatrix/*, kTable*/, kScriptValue, kMaxType
	};

	enum InitMode {
		InitCopy, InitRef, InitStack
	};

	class TypeHandler
	{
	public:
		int m_dataSize;
		virtual bool canCast(Variant::TypeId toType) = 0;
		virtual bool rawCast(const void *fromData, Variant::TypeId toType, void *toData) = 0;
		virtual void construct(void *ptr, const void *copyfrom) = 0;
		virtual void construct(void *ptr) = 0;
		virtual void destruct(void *ptr) = 0;
	};

	// info get
	TypeId getTypeId() const { return (TypeId)m_type; }
	void *getPointer() { if (m_storeMode == StoreMinibuf) return m_minibuf; else return m_voidstar; }
	const void *getPointer() const { if (m_storeMode == StoreMinibuf) return m_minibuf; else return m_voidstar; }

	// constructor
	Variant();
	Variant(bool v);
	Variant(int v);
	Variant(float v);
	Variant(const std::string &v);
	Variant(const char *v);
	Variant(Object *v);
	Variant(const Vector3 &v);
	Variant(const Point &v);
	Variant(const Rect &v);
	Variant(const Color3 &v);
	Variant(const Variant &v);
	Variant(const Matrix &matrix);
	explicit Variant(TypeId typeId);
	explicit Variant(TypeId typeId, void * data, InitMode initMode=InitCopy);
	~Variant();

	void init(TypeId typeId) { clear(); _init(typeId, 0); }
	void init(TypeId typeId, const void *data) { clear(); _init(typeId, data); }
	void init(TypeId typeId, void *data, InitMode initMode = InitCopy);

	void clear();
	operator bool() const;
	operator int() const;
	operator float() const;
	operator std::string() const;
	operator Object*() const;
	operator Vector3() const;
	operator Point() const;
	operator Rect() const;
	operator Color3() const;
	Variant& operator=(const Variant &v);
	operator Matrix() const;

	std::string toString() const;
	void fromString(TypeId t, const char *str);

	template<class Q>
	Q cast()
	{
		return variant_cast<Q>(*this);
	}

	template<class Q>
	Q& as()
	{
		AX_ASSERT(GetVariantType_<Q>() == m_type);
		return *(Q *)getPointer();
	}

	template<class Q>
	const Q& as() const {
		AX_ASSERT(GetVariantType_<Q>() == m_type);
		return *(const Q *)getPointer();
	}

	bool castSelf(Variant::TypeId toId);
	bool castTo(const Ref &ref);
	bool castTo(TypeId typeId, void *data);


	static int getTypeSize(TypeId t);
	static bool canCast(TypeId fromType, TypeId toType);
	static bool rawCast(TypeId fromType, const void *fromData, TypeId toType, void *toData);
	static void construct(TypeId typeId, void *ptr, const void *copyfrom);
	static void construct(TypeId typeId, void *ptr);
	static void destruct(TypeId typeId, void *ptr);

protected:
	void _init(Variant::TypeId t, const void *fromData);
	void _init(Variant::TypeId t, void *fromData, InitMode initMode);
	TypeHandler *getHandler() const;
	static TypeHandler *getHandler(Variant::TypeId typeId);

private:
	template <class Q>
	Q castHelper() const
	{
		TypeHandler *handler = getHandler();
		TypeId toType = GetVariantType_<Q>();

		if (m_type == toType)
			return as<Q>();

		Q result;
		if (handler && handler->canCast(toType)) {
			handler->rawCast(getPointer(), toType, &result);
		}
		return result;
	}


	// member variable
	union {
		void *m_voidstar;
		byte_t m_minibuf[MINIBUF_SIZE];
	};

	TypeId m_type : 16;
	StoreMode m_storeMode : 7;
};

class ConstRef
{
public:
	ConstRef(const Variant &rhs) : m_typeId(rhs.getTypeId()), m_voidstar(rhs.getPointer()) {}

	explicit ConstRef(Variant::TypeId t = Variant::kVoid, const void *d = 0) : m_typeId(t), m_voidstar(d) {}

	Variant::TypeId getTypeId() const { return m_typeId; }
	const void *getPointer() const { return m_voidstar; }
	void set(Variant::TypeId id, const void *ptr) { m_typeId = id; m_voidstar = ptr; }

	template <class Q>
	static ConstRef make(const Q &aData) {
		return ConstRef(GetVariantType_<Q>(), static_cast<const void *>(&aData));
	}

	template <class Q>
	ConstRef(const Q &aData) : m_typeId(GetVariantType_<Q>()), m_voidstar(static_cast<const void *>(&aData)) {
	}

	template <class Q>
	const Q &as() const
	{
		AX_ASSERT(GetVariantType_<Q>() == m_typeId);
		return *reinterpret_cast<const Q *>(m_voidstar);
	}

	bool castTo(Variant::TypeId id, void *data) const
	{
		return Variant::rawCast(m_typeId, m_voidstar, id, data);
	}

	bool castTo(Variant &val) const;

private:
	Variant::TypeId m_typeId;
	const void *m_voidstar;
};

class Ref
{
public:
	explicit Ref(Variant::TypeId t = Variant::kVoid, void *d = 0) : m_typeId(t), m_voidstar(d) {}

	Variant::TypeId getTypeId() const { return m_typeId; }
	void *getPointer() const { return m_voidstar; }
	void set(Variant::TypeId id, void *ptr) { m_typeId = id; m_voidstar = ptr; }

	template <class Q>
	static Ref make(Q &aData) {
		return Ref(GetVariantType_<Q>(), static_cast<void *>(&aData));
	}

	template <class Q>
	Ref(Q &aData) : m_typeId(GetVariantType_<Q>()), m_voidstar(static_cast<void *>(&aData)) {
	}

	template <class Q>
	Q &as() const
	{
		AX_ASSERT(GetVariantType_<Q>() == m_typeId);
		return *reinterpret_cast<Q *>(m_voidstar);
	}

	bool castTo(Variant::TypeId id, void *data) const
	{
		return Variant::rawCast(m_typeId, m_voidstar, id, data);
	}

private:
	Variant::TypeId m_typeId;
	void *m_voidstar;
};


inline void Variant::construct(TypeId type, void *ptr, const void *copyfrom)
{
	TypeHandler *handler = getHandler(type);
	AX_ASSERT(handler);
	handler->construct(ptr, copyfrom);
}

inline void Variant::construct(TypeId type, void *ptr)
{
	TypeHandler *handler = getHandler(type);
	AX_ASSERT(handler);
	handler->construct(ptr);
}

inline void Variant::destruct(TypeId type, void *ptr)
{
	TypeHandler *handler = getHandler(type);
	AX_ASSERT(handler);
	handler->destruct(ptr);
}

inline bool Variant::castSelf( Variant::TypeId toId )
{
	if (m_type == toId) return true;
	if (!canCast(m_type, toId)) return false;

	AX_DECL_STACK_VARIANT(casted, toId);

	bool castSuccess = castTo(casted.getTypeId(), casted.getPointer());
	if (!castSuccess) {
		return false;
	}

	*this = casted;
	return true;
}

inline bool Variant::castTo( const Ref &ref )
{
	return castTo(ref.getTypeId(), ref.getPointer());
}

inline bool Variant::castTo( TypeId typeId, void *data )
{
	return rawCast(m_type, getPointer(), typeId, data);
}

typedef std::vector<Variant> VariantSeq;

template< typename T >
inline Variant::TypeId GetVariantType_() {
	// must be specialized, or raise a static error
	AX_STATIC_ASSERT(0);
}

template<>
inline Variant::TypeId GetVariantType_<void>() {
	return Variant::kVoid;
}

template<>
inline Variant::TypeId GetVariantType_<int>() {
	return Variant::kInt;
}

template<>
inline Variant::TypeId GetVariantType_<bool>() {
	return Variant::kBool;
}

template<>
inline Variant::TypeId GetVariantType_<float>() {
	return Variant::kFloat;
}

template<>
inline Variant::TypeId GetVariantType_<std::string>() {
	return Variant::kString;
}

template<>
inline Variant::TypeId GetVariantType_<Vector3>() {
	return Variant::kVector3;
}

template<>
inline Variant::TypeId GetVariantType_<Color3>() {
	return Variant::kColor3;
}

template<>
inline Variant::TypeId GetVariantType_<Point>() {
	return Variant::kPoint;
}

template<>
inline Variant::TypeId GetVariantType_<Rect>() {
	return Variant::kRect;
}

template<>
inline Variant::TypeId GetVariantType_<Object*>() {
	return Variant::kObject;
}

template<>
inline Variant::TypeId GetVariantType_<Matrix>() {
	return Variant::kMatrix;
}

#if 0
template<>
inline Variant::TypeId GetVariantType_<LuaTable>() {
	return Variant::kTable;
}
#endif

class ScriptValue;
template<>
inline Variant::TypeId GetVariantType_<ScriptValue>() {
	return Variant::kScriptValue;
}

// variant cast
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

AX_END_NAMESPACE

#include "variant.inl"

#endif // AX_VARIANT_H

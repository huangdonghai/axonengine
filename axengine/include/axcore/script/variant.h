#ifndef AX_VARIANT_H
#define AX_VARIANT_H

AX_BEGIN_NAMESPACE

class Object;
class Variant;

//--------------------------------------------------------------------------
// class LuaTable
//--------------------------------------------------------------------------

struct AX_API LuaTable
{
public:
	LuaTable();
	LuaTable(const LuaTable &rhs);
	explicit LuaTable(int index);

	void beginRead() const;
	Variant get(const String &n) const;
	void set(const String &n, const Variant &v);
	int getLength() const;
	// n is start from 0
	Variant get(int n) const;
	void endRead() const;

	void beginIterator() const;
	bool nextIterator(Variant &k, Variant &v) const;
	void endIterator() const;

	Vector3 toVector3() const;
	Color3 toColor() const;
	Point toPoint() const;
	Rect toRect() const;
	Object *toObject() const;

	// type conversion
	operator Vector3() const { return toVector3(); }
	operator Color3() const { return toColor(); }
	operator Point() const { return toPoint(); }
	operator Rect() const { return toRect(); }
	operator Object *() const { return toObject(); }

	LuaTable &operator=(const LuaTable &rhs);

public:
	const int m_index;
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
		kVoid, kBool, kInt, kFloat, kString, kObject, kVector3, kColor3, kPoint, kRect, kMatrix3x4, kTable, kScriptValue, kMaxType
	};

	enum {
		MINIBUF_SIZE = 3 * sizeof(float)
	};

	class TypeHandler
	{
	public:
		int m_dataSize;
		virtual bool canCast(Variant::Type toType) = 0;
		virtual bool rawCast(const void *fromData, Variant::Type toType, void *toData) = 0;
		virtual void construct(void *ptr, const void *copyfrom) = 0;
		virtual void construct(void *ptr) = 0;
		virtual void destruct(void *ptr) = 0;
	};

	Type getType() const { return (Type)m_type; }

	// constructor
	Variant();
	Variant(bool v);
	Variant(int v);
	Variant(float v);
	Variant(const String &v);
	Variant(const char *v);
	Variant(Object *v);
	Variant(const Vector3 &v);
	Variant(const Point &v);
	Variant(const Rect &v);
	Variant(const Color3 &v);
	Variant(const Variant &v);
	Variant(const LuaTable &table);
	Variant(const Matrix &matrix);
	~Variant();

	void clear();
	operator bool() const;
	operator int() const;
	operator float() const;
	operator String() const;
	operator Object*() const;
	operator Vector3() const;
	operator Point() const;
	operator Rect() const;
	operator Color3() const;
	operator LuaTable() const;
	Variant &operator=(const Variant &v);
	operator Matrix() const;

	String toString() const;
	void fromString(Type t, const char *str);

	template<class Q>
	Q cast() {
		return variant_cast<Q>(*this);
	}

	template<class Q>
	Q& ref() {
		AX_ASSERT(GetVariantType_<Q>() == m_type);
		return *(Q *)getPtr();
	}

	template<class Q>
	const Q& ref() const {
		AX_ASSERT(GetVariantType_<Q>() == m_type);
		return *(const Q *)getPtr();
	}

	static int getTypeSize(Type t);
	static bool canCast(Type fromType, Type toType);
	static bool rawCast(Type fromType, const void *fromData, Type toType, void *toData);

	void construct(Variant::Type t, const void *fromData);
	void *getPtr() { if (m_isMinibuf) return m_minibuf; else return m_voidstar; }
	const void *getPtr() const { if (m_isMinibuf) return m_minibuf; else return m_voidstar; }
	TypeHandler *getTypeHandler() const;

private:
	template <class Q>
	Q castHelper() const
	{
		Q result;
		TypeHandler *handler = getTypeHandler();
		Type toType = GetVariantType_<Q>();
		if (handler && handler->canCast(toType)) {
			handler->rawCast(getPtr(), toType, &result);
		}
		return result;
	}


	// member variable
	union {
		void *m_voidstar;
		byte_t m_minibuf[MINIBUF_SIZE];
	};

	Type m_type : 30;
	int m_isMinibuf : 1;
};


typedef Sequence<Variant> VariantSeq;

template< typename T >
inline Variant::Type GetVariantType_() {
	// must be specialized, or raise a static error
	AX_STATIC_ASSERT(0);
}

template<>
inline Variant::Type GetVariantType_<void>() {
	return Variant::kVoid;
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
inline Variant::Type GetVariantType_<Color3>() {
	return Variant::kColor3;
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
inline Variant::Type GetVariantType_<Matrix>() {
	return Variant::kMatrix3x4;
}

template<>
inline Variant::Type GetVariantType_<LuaTable>() {
	return Variant::kTable;
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

AX_END_NAMESPACE

#include "variant.inl"

#endif // AX_VARIANT_H

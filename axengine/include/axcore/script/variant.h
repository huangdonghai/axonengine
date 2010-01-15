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
	LuaTable(int index);

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
		kEmpty, kBool, kInt, kFloat, kString, kObject, kTable, kVector3, kColor3, kPoint, kRect, kMatrix3x4, kMaxType
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
	Variant(const String &v);
	Variant(const char *v);
	Variant(Object *v);
	Variant(const Vector3 &v);
	Variant(const Point &v);
	Variant(const Rect &v);
	Variant(const Color3 &v);
	Variant(const Variant &v);
	Variant(const LuaTable &table);
	Variant(const Matrix3x4 &matrix);
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
	operator Color3() const;
	operator LuaTable() const;
	Variant &operator=(const Variant &v);
	operator Matrix3x4() const;

	void set(int v);
	void set(float v);
	void set(double v);
	void set(const char *v);
	void set(const String &v);
	void set(const Variant &v);
	void set(Object *v);

	bool toBool() const { return operator bool(); }
	int toInt() const { return operator int(); }
	float toFloat() const { return operator float(); }

	String toScript() const;

	String toString() const;
	void fromString(Type t, const char *str);

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
		Object *obj;
		String *str;
		Matrix3x4 *mtr;
		byte_t minibuf[MINIBUF_SIZE];
	};
};

inline Variant::Variant() : type(kEmpty) {}

inline Variant::Variant(bool v) : type(kBool), boolval(v) {}

inline Variant::Variant(int v) : type(kInt), intval(v) {}

inline Variant::Variant(double v) : type(kFloat), realval(v) {}

inline Variant::Variant(const String &v) : type(kString), str(new String(v)) {}

inline Variant::Variant(const char *v) : type(kString), str(new String(v)) {}

inline Variant::Variant(Object *v) : type(kObject), obj(v) {}

inline Variant::Variant(const Vector3 &v) : type(kVector3) { new (minibuf) Vector3(v); }

inline Variant::Variant(const Point &v) : type(kPoint) { new (minibuf) Point(v); }

inline Variant::Variant(const Rect &v) : type(kRect) { new (minibuf) Rect(v); }

inline Variant::Variant(const Color3 &v) : type(kColor3) { new (minibuf) Color3(v); }

inline Variant::Variant(const Matrix3x4 &v) : type(kMatrix3x4), mtr(new Matrix3x4(v)) {}

inline Variant::Variant(const Variant &v) : type(v.type), realval(v.realval) {
	if (type == kString) {
		str = new String(*(String*)v.str);
		return;
	}

	if (type == kMatrix3x4) {
		mtr = new Matrix3x4(*(Matrix3x4*)v.mtr);
		return;
	}

	::memcpy(minibuf, v.minibuf, MINIBUF_SIZE);
}

inline Variant::Variant(const LuaTable &rhs) : type(kTable) {
	new(minibuf) LuaTable(rhs);
}

inline Variant::~Variant() { clear(); }

inline void Variant::clear() {
	if (type == kString) {
		delete str;
	} else if (type == kMatrix3x4) {
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

inline Variant::operator Matrix3x4() const {
	switch (type) {
	case kTable:
		break;
	case kRect:
		return *mtr;
		break;
	}

	return Matrix3x4();
}

inline Variant::operator Color3() const {
	switch (type) {
	case kTable:
		return ((LuaTable*)minibuf)->toColor();
		break;
	case kColor3:
		return *(Color3*)minibuf;
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

inline Variant &Variant::operator=(const Variant &v) {
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

inline void Variant::set(const char *v) {
	clear();
	type = kString;
	str = new String(v);
}

inline void Variant::set(const String &v) {
	clear();
	type = kString;
	str = new String(v);
}

inline void Variant::set(const Variant &v) {
	clear();
	type = v.type;
	::memcpy(minibuf, v.minibuf, MINIBUF_SIZE);
	if (type == kString) {
		str = new String(*v.str);
	} else if (type == kMatrix3x4) {
		mtr = new Matrix3x4(*v.mtr);
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
			return v.toString();
			break;
		}
	case kColor3:
		{
			Color3 v = *this;
			return v.toString();
			break;
		}
	case kPoint:
		{
			Point v = *this;
			return v.toString();
			break;
		}
	case kRect:
		{
			Rect v = *this;
			return v.toString();
			break;
		}
	case kMatrix3x4:
		{
			return mtr->toString();
		}
	default:
		AX_NO_DEFAULT;
	}
	return result;
}

inline void Variant::fromString(Type t, const char *str) {
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
	case kColor3:
		{
			Color3 v;
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
	case kMatrix3x4:
		{
			Matrix3x4 v;
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
inline Variant::Type GetVariantType_<Matrix3x4>() {
	return Variant::kMatrix3x4;
}

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

#endif // AX_VARIANT_H

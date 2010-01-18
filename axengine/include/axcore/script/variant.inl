AX_BEGIN_NAMESPACE

inline Variant::Variant() : m_type(kVoid), m_isMinibuf(true), m_voidstar(0)
{}

inline Variant::Variant(bool v)
{ construct(kBool, &v); }

inline Variant::Variant(int v)
{ construct(kInt, &v); }

inline Variant::Variant(float v)
{ construct(kFloat, &v); }

inline Variant::Variant(const String &v)
{ construct(kString, &v); }

inline Variant::Variant(const char *v)
{
	String str(v);
	construct(kString, &str);
}

inline Variant::Variant(Object *v)
{ construct(kObject, &v); }

inline Variant::Variant(const Vector3 &v)
{ construct(kVector3, &v); }

inline Variant::Variant(const Point &v)
{ construct(kPoint, &v); }

inline Variant::Variant(const Rect &v)
{ construct(kRect, &v); }

inline Variant::Variant(const Color3 &v)
{ construct(kColor3, &v); }

inline Variant::Variant(const Matrix &v)
{ construct(kMatrix3x4, &v); }

inline Variant::Variant(const Variant &v)
{ construct(v.m_type, v.getPtr()); }

inline Variant::Variant(const LuaTable &rhs)
{ construct(kTable, &rhs); }

inline Variant::~Variant()
{ clear(); }

inline Variant::operator bool() const
{
	return castHelper<bool>();
}

inline Variant::operator int() const
{
	return castHelper<int>();
}

inline Variant::operator float() const
{
	return castHelper<float>();
}

inline Variant::operator Vector3() const
{
	return castHelper<Vector3>();
}

inline Variant::operator Point() const
{
	return castHelper<Point>();
}

inline Variant::operator Rect() const
{
	return castHelper<Rect>();
}

inline Variant::operator Matrix() const
{
	return castHelper<Matrix>();
}

inline Variant::operator Color3() const
{
	return castHelper<Color3>();
}

inline Variant::operator LuaTable() const
{
	return castHelper<LuaTable>();
}

inline Variant::operator Object*() const
{
	return castHelper<Object*>();
}

inline Variant &Variant::operator=(const Variant &v) {
	clear();
	construct(v.m_type, v.getPtr());
	return *this;
}


inline Variant::operator String() const
{
	return castHelper<String>();
}
#if 0
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
#endif

AX_END_NAMESPACE
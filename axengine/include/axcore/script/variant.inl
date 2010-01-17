AX_BEGIN_NAMESPACE

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

AX_END_NAMESPACE
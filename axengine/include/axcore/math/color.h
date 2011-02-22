/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_COLOR_H
#define AX_CORE_COLOR_H

AX_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
// Rgb, 3 unsigned byte, identical to D3DFMT_R8G8B8
//------------------------------------------------------------------------------
class Color3;
struct AX_API Rgb {
	enum {
		B, G, R
	};
	byte_t b, g, r;

	// constructor and destructor
	Rgb();
	Rgb(byte_t ir, byte_t ig, byte_t ib);
	Rgb(const Vector3 &vec);
	Rgb(const Color3 &rhs);

	// operator overridden
	bool operator==(const Rgb &color) const;
	bool operator!=(const Rgb &color) const;
	void clear();
	Rgb &set(byte_t ir, byte_t ig, byte_t ib);
	Rgb operator*(const Rgb &other);
	Rgb& operator*=(const Rgb &other);
	Rgb operator*(float scale);
	Rgb& operator*=(float scale);
	Rgb operator+(const Rgb &other) const;
	Rgb& operator+=(const Rgb &other);

	byte_t& operator[](int index);
	byte_t operator[](int index) const;

	void fromVector(const Vector3 &v);
	Vector3 toVector() const;
	Color3 toColor3() const;
	std::string toString() const;
	bool fromString(const char *str);

	const static Rgb Black ;
	const static Rgb Red ;
	const static Rgb Green ;
	const static Rgb Blue ;
	const static Rgb Yellow ;
	const static Rgb Magenta ;
	const static Rgb Cyan ;
	const static Rgb White ;
	const static Rgb LtGrey ;
	const static Rgb MdGrey ;
	const static Rgb DkGrey ;
	const static Rgb ColorTable[];
};

inline Rgb::Rgb() {}

inline Rgb::Rgb(byte_t ir, byte_t ig, byte_t ib)
	: r(ir), g(ig), b(ib) {}

inline Rgb::Rgb(const Vector3 &vec)
	: r(Math::clampByte(vec.x*255.f))
	, g(Math::clampByte(vec.y*255.f))
	, b(Math::clampByte(vec.z*255.f)) {}


// operator overridden
inline bool Rgb::operator==(const Rgb &color) const {
	return r == color.r && g == color.g && b == color.b;
}

inline bool Rgb::operator!=(const Rgb &color) const {
	return r != color.r || g != color.g || b != color.b;
}

inline void Rgb::clear() { r = g = b = 0; }

inline Rgb &Rgb::set(byte_t ir, byte_t ig, byte_t ib) {
	r=ir; g=ig; b=ib; return *this;
}

inline Rgb Rgb::operator*(const Rgb &other) {
	Rgb c;
	c.r = ((int)r * other.r) >> 8;
	c.g = ((int)g * other.g) >> 8;
	c.b = ((int)b * other.b) >> 8;
	return c;
}

inline Rgb& Rgb::operator*=(const Rgb &other) {
	r = ((int)r * other.r) >> 8;
	g = ((int)g * other.g) >> 8;
	b = ((int)b * other.b) >> 8;
	return *this;
}


inline Rgb Rgb::operator*(float scale) {
	Rgb c;

	c.r = (byte_t)(scale * r);
	c.g = (byte_t)(scale * g);
	c.b = (byte_t)(scale * b);
	return c;
}

inline Rgb& Rgb::operator*=(float scale) {
	r = (byte_t)(scale * r);
	g = (byte_t)(scale * g);
	b = (byte_t)(scale * b);
	return *this;
}

inline Rgb Rgb::operator+(const Rgb &other) const {
	Rgb c;
	c.r = Math::clampByte((int)r + other.r);
	c.g = Math::clampByte((int)g + other.g);
	c.b = Math::clampByte((int)b + other.b);
	return c;
}

inline Rgb& Rgb::operator+=(const Rgb &other) {
	r = Math::clampByte((int)r + other.r);
	g = Math::clampByte((int)g + other.g);
	b = Math::clampByte((int)b + other.b);
	return *this;
}

inline byte_t &Rgb::operator[](int index) {
	AX_ASSERT(index>=0 && index < 3);
	return *(&r+index);
}

inline byte_t Rgb::operator[](int index) const {
	AX_ASSERT(index>=0 && index < 3);
	return *(&r+index);
}

inline void Rgb::fromVector(const Vector3 &v) {
	r = Math::clampByte(v.x * 255.0f);
	g = Math::clampByte(v.y * 255.0f);
	b = Math::clampByte(v.z * 255.0f);
}

inline Vector3 Rgb::toVector() const {
	float inv255 = 1.0f / 255.0f;
	return Vector3(r*inv255, g*inv255, b*inv255);

}


//------------------------------------------------------------------------------
// Rgba, 4 unsigned byte, identical to D3DFMT_A8R8G8B8
//------------------------------------------------------------------------------

struct AX_API Rgba {
	enum {
		B, G, R, A
	};
	union {
		struct {
			byte_t b, g, r, a;
		};
		uint_t dword;
	};

	Rgba();
	Rgba(byte_t ir, byte_t ig, byte_t ib, byte_t ia=0xFF);
	Rgba(int ir, int ig, int ib, int ia=0xFF);
	Rgba(float _r, float _g, float _b, float _a = 1.0f);
	Rgba(const Vector3 &vec);
	Rgba(const Vector4 &vec);
	Rgba(const Color4 &rhs);

	bool operator==(const Rgba &color) const;
	bool operator!=(const Rgba &color) const ;
	operator const byte_t*() const;
	void clear();
	Rgba &set(byte_t ir, byte_t ig, byte_t ib, byte_t ia=255);
	Rgba &set(const Rgb &rgb);
	Rgba operator*(const Rgba &other) const;
	Rgba operator*(float scale) const;
	Rgba operator+(const Rgba &other) const;
	Rgba operator-(const Rgba &other) const;
	Rgba &operator*=(float scale);
	Rgba &operator+=(const Rgba &other);
	Vector4 toVector4() const;
	byte_t &operator[](int index);
	byte_t operator[](int index) const;

	std::string toStringRgb() const;
	void parseRgb(const char *text);

	Rgb rgb() const { return Rgb(r,g,b); }

	std::string toString() const;
	void fromString(const char *str);

	static Rgba randColor();

	const static Rgba Zero;	// r=g=b=a=0
	const static Rgba Black;	// r=g=b=0, a=255
	const static Rgba Red;
	const static Rgba Green;
	const static Rgba Blue;
	const static Rgba Yellow;
	const static Rgba Magenta;
	const static Rgba Cyan;
	const static Rgba White;
	const static Rgba LtGrey;
	const static Rgba MdGrey;
	const static Rgba DkGrey;
	const static Rgba ColorTable[];
};

inline Rgba::Rgba() {}

inline Rgba::Rgba(byte_t ir, byte_t ig, byte_t ib, byte_t ia)
	: r(ir), g(ig), b(ib), a(ia) {}

inline Rgba::Rgba(int ir, int ig, int ib, int ia)
	: r(ir), g(ig), b(ib), a(ia) {}

inline Rgba::Rgba(float _r, float _g, float _b, float _a)
	: r(Math::clampByte(_r*255.f))
	, g(Math::clampByte(_g*255.f))
	, b(Math::clampByte(_b*255.f))
	, a(Math::clampByte(_a*255.f)) {}


inline Rgba::Rgba(const Vector3 &vec)
	: r(Math::clampByte(vec.x*255.f))
	, g(Math::clampByte(vec.y*255.f))
	, b(Math::clampByte(vec.z*255.f))
	, a(255) {}

inline Rgba::Rgba(const Vector4 &vec)
	: r(Math::clampByte(vec.x*255.f))
	, g(Math::clampByte(vec.y*255.f))
	, b(Math::clampByte(vec.z*255.f))
	, a(Math::clampByte(vec.w*255.f)) {}

inline bool Rgba::operator==(const Rgba &rhs) const {
	return dword == rhs.dword;
}

inline bool Rgba::operator!=(const Rgba &rhs) const {
	return dword != rhs.dword;
}

inline Rgba::operator const byte_t*() const { return &r; }

inline void Rgba::clear() { r=g=b=a=0; }

inline Rgba &Rgba::set(byte_t ir, byte_t ig, byte_t ib, byte_t ia) {
	r=ir; g=ig; b=ib; a=ia; return *this;
}

inline Rgba &Rgba::set(const Rgb &rgb) {
	r=rgb.r; g=rgb.g; b=rgb.b; a=255; return *this;
}

inline Rgba Rgba::operator*(const Rgba &other) const {
	Rgba c;
	c.r = ((int)r * other.r) >> 8;
	c.g = ((int)g * other.g) >> 8;
	c.b = ((int)b * other.b) >> 8;
	c.a = ((int)a * other.a) >> 8;
	return c;
}

inline Rgba Rgba::operator*(float scale) const {
	Rgba c;

	c.r = (byte_t)(scale * r);
	c.g = (byte_t)(scale * g);
	c.b = (byte_t)(scale * b);
	c.a = (byte_t)(scale * a);
	return c;
}

inline Rgba Rgba::operator+(const Rgba &other) const {
	Rgba c;

	c.r = r + other.r;
	c.g = g + other.g;
	c.b = b + other.b;
	c.a = a + other.a;

	return c;
}

inline Rgba Rgba::operator-(const Rgba &other) const {
	Rgba c;

	c.r = r - other.r;
	c.g = g - other.g;
	c.b = b - other.b;
	c.a = a - other.a;

	return c;
}

inline Rgba &Rgba::operator*=(float scale) {
	return *this = (*this * scale);
}

inline Rgba &Rgba::operator+=(const Rgba &other) {
	return *this = (*this + other);
}

inline Vector4 Rgba::toVector4() const {
	Vector4 result;
	float inv255 = 1.0f / 255.0f;
	result.x = r * inv255;
	result.y = g * inv255;
	result.z = b * inv255;
	result.w = a * inv255;
	return result;
}

inline byte_t &Rgba::operator[](int index) {
	AX_ASSERT(index>=0);
	AX_ASSERT(index<4);
	return *(&r+index);
}

inline byte_t Rgba::operator[](int index) const {
	AX_ASSERT(index>=0);
	AX_ASSERT(index<4);
	return *(&r+index);
}

inline std::string Rgba::toStringRgb() const {
	std::string result;

	StringUtil::sprintf(result, "%d,%d,%d", r,g,b);

	return result;
}

inline void Rgba::parseRgb(const char *text) {
	int _r, _g, _b;
	int v = sscanf(text, "%d,%d,%d", &_r, &_g, &_b);
	AX_ASSERT(v = 3);
	r = _r; g = _g; b = _b; a = 255;
}

inline Rgba Rgba::randColor() {
	Rgba result;
	result.r = rand() % 255;
	result.g = rand() % 255;
	result.b = rand() % 255;
	result.a = 255;
	return result;
}

struct AX_API Color3
{
	float r, g, b;
	// constructor and destructor
	inline Color3() {}
	inline Color3(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}
	explicit inline Color3(const Vector3& vec3) : r(vec3.x), g(vec3.y), b(vec3.z) {}
	inline ~Color3() {}

	Color3& set(float _r, float _g, float _b) { r = _r; g = _g; b = _b; return *this; }

	Vector3 toVector3() const { return Vector3(r,g,b); }
	Rgb toRgb() const { return Rgb(*this); }

	// compare
	bool operator==(const Color3 &v) const { return r==v.r && g==v.g && b==v.b; }
	bool operator!=(const Color3 &v) const { return r!=v.r || g!=v.g || b!=v.b; }
	bool operator<(const Color3 &rhs) const { return r < rhs.r && g < rhs.g && b < rhs.b; }

	// math
	Color3 operator*(const Color3 &rhs) const { return Color3(r*rhs.r, g*rhs.g, b*rhs.b); }
	Color3& operator*=(const Color3 &rhs) { r *= rhs.r; g *= rhs.g; b *= rhs.b; return *this; }

	Color3 operator*(float rhs) const { return Color3(r*rhs, g*rhs, b*rhs); }
	Color3& operator*=(float rhs) { r *= rhs; g *= rhs; b *= rhs; return *this; }

	Color3 operator+(const Color3 &rhs) const { return Color3(r+rhs.r, g+rhs.g, b+rhs.b); }
	Color3& operator+=(const Color3 &rhs) { r += rhs.r; g += rhs.g; b += rhs.b; return *this; }

	Color3 operator-(const Color3 &rhs) const { return Color3(r-rhs.r, g-rhs.g, b-rhs.b); }
	Color3& operator-=(const Color3 &rhs) { r -= rhs.r; g -= rhs.g; b -= rhs.b; return *this; }

	Color3 operator/(const Color3 &rhs) const { return Color3(r/rhs.r, g/rhs.g, b/rhs.b); }
	Color3& operator/=(const Color3 &rhs) { r /= rhs.r; g /= rhs.g; b /= rhs.b; return *this; }

	Color3 operator/(float rhs) const { return Color3(r/rhs, g/rhs, b/rhs); }
	Color3& operator/=(float rhs) { r /= rhs; g /= rhs; b /= rhs; return *this; }

	const float *c_ptr() const { return &r; }

	// bool
	bool isZero() const { return r == 0 && g == 0 && b == 0; }

	std::string toString() const;
	bool fromString(const char *str);
};

struct AX_API Color4
{
	float r, g, b, a;

	Color4() {}
	Color4(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
	explicit Color4(const Vector4 &rhs) : r(rhs.x), g(rhs.y), b(rhs.z), a(rhs.w) {}
	~Color4() {}

	Color4& set(float _r, float _g, float _b, float _a) {r=_r; g=_g; b=_b; a=_a; return *this; }
	Vector4 toVector4() const { return Vector4(r,g,b,a); }
	Rgba toRgba() const { return Rgba(*this); }

	// compare
	bool operator==(const Color4 &rhs) const { return r==rhs.r && g==rhs.g && b==rhs.b && a==rhs.a; }
	bool operator!=(const Color4 &rhs) const { return r!=rhs.r || g!=rhs.g || b!=rhs.b || a!= rhs.a; }
	bool operator<(const Color4 &rhs) const { return r < rhs.r && g < rhs.g && b < rhs.b && a<=rhs.a; }

	// math
	Color4 operator*(const Color4 &rhs) const { return Color4(r*rhs.r, g*rhs.g, b*rhs.b, a*rhs.a); }
	Color4& operator*=(const Color4 &rhs) { return *this = *this * rhs; }

	Color4 operator*(float rhs) const { return Color4(r*rhs, g*rhs, b*rhs,a*rhs); }
	Color4& operator*=(float rhs) { return *this = *this * rhs; }

	Color4 operator+(const Color4 &rhs) const { return Color4(r+rhs.r, g+rhs.g, b+rhs.b, a+rhs.a); }
	Color4& operator+=(const Color4 &rhs) { return (*this) = (*this) + rhs; }

	Color4 operator-(const Color4 &rhs) const { return Color4(r-rhs.r, g-rhs.g, b-rhs.b, a-rhs.a); }
	Color4& operator-=(const Color4 &rhs) { return (*this) = (*this) - rhs; }

	Color4 operator/(const Color4 &rhs) const { return Color4(r/rhs.r, g/rhs.g, b/rhs.b, a/rhs.a); }
	Color4& operator/=(const Color4 &rhs) { return *this = *this / rhs; }

	Color4 operator/(float rhs) const { return *this * (1.0f/rhs); }
	Color4& operator/=(float rhs) { return *this = *this / rhs; }

	std::string toString() const;
	bool fromString(const char *str);
};

inline Rgb::Rgb(const Color3 &rhs)
	: r(Math::clampByte(rhs.r*255.f))
	, g(Math::clampByte(rhs.g*255.f))
	, b(Math::clampByte(rhs.b*255.f))
{}

inline Rgba::Rgba(const Color4 &rhs)
	: r(Math::clampByte(rhs.r*255.f))
	, g(Math::clampByte(rhs.g*255.f))
	, b(Math::clampByte(rhs.b*255.f))
	, a(Math::clampByte(rhs.a*255.f))
{}

inline Color3 Rgb::toColor3() const
{
	Color3 c(r,g,b);
	c *= 1.0f / 255.0f;
	return c;
}


AX_END_NAMESPACE

#endif // end guardian


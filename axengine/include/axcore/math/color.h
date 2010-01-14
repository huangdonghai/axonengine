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
// Color, 3 unsigned byte
//------------------------------------------------------------------------------

struct AX_API Rgb {
	byte_t r, g, b;

	// constructor and destructor
	Rgb();
	Rgb(byte_t ir, byte_t ig, byte_t ib);
	Rgb(const Vector3 &vec);

	// operator overridden
	bool operator==(const Rgb &color) const;
	bool operator!=(const Rgb &color) const;
	void clear();
	Rgb &set(byte_t ir, byte_t ig, byte_t ib);
	Rgb operator*(const Rgb &other);
	Rgb &operator*=(const Rgb &other);
	Rgb operator*(float scale);
	Rgb operator*=(float scale);
	byte_t &operator[](int index);
	byte_t operator[](int index) const;

	void fromVector(const Vector3 &v);
	Vector3 toVector() const;
	String toString() const;
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

inline Rgb &Rgb::operator*=(const Rgb &other) {
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

inline Rgb Rgb::operator*=(float scale) {
	r = (byte_t)(scale * r);
	g = (byte_t)(scale * g);
	b = (byte_t)(scale * b);
	return *this;
}

inline byte_t &Rgb::operator[](int index) {
	AX_STRICT_ASSERT(index>=0 && index < 3);
	return *(&r+index);
}

inline byte_t Rgb::operator[](int index) const {
	AX_STRICT_ASSERT(index>=0 && index < 3);
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
// Color, 4 unsigned byte
//------------------------------------------------------------------------------

struct AX_API Rgba {
	byte_t r, g, b, a;

	Rgba();
	Rgba(byte_t ir, byte_t ig, byte_t ib, byte_t ia=0xFF);
	Rgba(int ir, int ig, int ib, int ia=0xFF);
	Rgba(float _r, float _g, float _b, float _a = 1.0f);
	Rgba(const Vector3 &vec);
	Rgba(const Vector4 &vec);
	Rgba &operator=(const Vector3 &v);
	Rgba &operator=(const Vector4 &v);

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
	Vector4 toVector() const;
	byte_t &operator[](int index);
	byte_t operator[](int index) const;

	String toStringRgb() const;
	void parseRgb(const char *text);

	Bgr bgr() const;
	Rgb rgb() const { return Rgb(r,g,b); }

	String toString() const;
	void fromString(const char *str);

	static Rgba randColor();

	const static Rgba Zero ;	// r=g=b=a=0
	const static Rgba Black ;	// r=g=b=0, a=255
	const static Rgba Red ;
	const static Rgba Green ;
	const static Rgba Blue ;
	const static Rgba Yellow ;
	const static Rgba Magenta ;
	const static Rgba Cyan ;
	const static Rgba White ;
	const static Rgba LtGrey ;
	const static Rgba MdGrey ;
	const static Rgba DkGrey ;
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

// operator overridden
inline Rgba &Rgba::operator=(const Vector3 &v) {
	r = Math::clampByte(v.x*255.f);
	g = Math::clampByte(v.y*255.f);
	b = Math::clampByte(v.z*255.f);
	a = 255;
	return *this;
}

inline Rgba &Rgba::operator=(const Vector4 &v) {
	r = Math::clampByte(v.x*255.f);
	g = Math::clampByte(v.y*255.f);
	b = Math::clampByte(v.z*255.f);
	a = Math::clampByte(v.w*255.f);
	return *this;
}
inline bool Rgba::operator==(const Rgba &color) const {
	return r == color.r && g == color.g && b == color.b && a == color.a;
}

inline bool Rgba::operator!=(const Rgba &color) const {
	return r != color.r || g != color.g || b != color.b || a != color.a;
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

inline Vector4 Rgba::toVector() const {
	Vector4 result;
	float inv255 = 1.0f / 255.0f;
	result.x = r * inv255;
	result.y = g * inv255;
	result.z = b * inv255;
	result.w = a * inv255;
	return result;
}

inline byte_t &Rgba::operator[](int index) {
	AX_STRICT_ASSERT(index>=0);
	AX_STRICT_ASSERT(index<4);
	return *(&r+index);
}

inline byte_t Rgba::operator[](int index) const {
	AX_STRICT_ASSERT(index>=0);
	AX_STRICT_ASSERT(index<4);
	return *(&r+index);
}

inline String Rgba::toStringRgb() const {
	String result;

	StringUtil::sprintf(result, "%d,%d,%d", r,g,b);

	return result;
}

inline void Rgba::parseRgb(const char *text) {
	int _r, _g, _b;
	int v = sscanf(text, "%d,%d,%d", &_r, &_g, &_b);
	AX_STRICT_ASSERT(v = 3);
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

//------------------------------------------------------------------------------
// struct Bgr
//------------------------------------------------------------------------------

struct AX_API Bgr {
	byte_t b, g, r;

	// constructor and destructor
	Bgr();
	Bgr(byte_t ir, byte_t ig, byte_t ib);
	Bgr(const Vector3 &vec);

	// operator overridden
	bool operator==(const Bgr &color) const;
	bool operator!=(const Bgr &color) const;
	void clear();
	Bgr &set(byte_t ir, byte_t ig, byte_t ib);
	Bgr operator*(const Bgr &other) const;
	Bgr &operator*=(const Bgr &other);
	Bgr operator*(int filter) const;
	Bgr &operator*=(int filter);
	Bgr operator*(float scale) const;
	Bgr &operator*=(float scale);
	Bgr operator+(const Bgr &other) const;
	Bgr &operator+=(const Bgr &other);
	byte_t &operator[](int index);
	byte_t operator[](int index) const;
};

inline Bgr::Bgr() {}
inline Bgr::Bgr(byte_t ir, byte_t ig, byte_t ib)
	: r(ir), g(ig), b(ib) {}
inline Bgr::Bgr(const Vector3 &vec)
	: r(Math::clampByte(vec.x*255.f))
	, g(Math::clampByte(vec.y*255.f))
	, b(Math::clampByte(vec.z*255.f)) {}

// operator overridden
inline bool Bgr::operator==(const Bgr &color) const {
	return r == color.r && g == color.g && b == color.b;
}

inline bool Bgr::operator!=(const Bgr &color) const {
	return r != color.r || g != color.g || b != color.b;
}

inline void Bgr::clear() { r = g = b = 0; }

inline Bgr &Bgr::set(byte_t ir, byte_t ig, byte_t ib) {
	r=ir; g=ig; b=ib; return *this;
}

inline Bgr Bgr::operator*(const Bgr &other) const {
	Bgr c;
	c.r = ((int)r * other.r) / 255;
	c.g = ((int)g * other.g) / 255;
	c.b = ((int)b * other.b) / 255;
	return c;
}

inline Bgr &Bgr::operator*=(const Bgr &other) {
	r = ((int)r * other.r) / 255;
	g = ((int)g * other.g) / 255;
	b = ((int)b * other.b) / 255;
	return *this;
}

inline Bgr Bgr::operator*(int filter) const {
	Bgr c;
	c.r = ((int)r * filter) / 255;
	c.g = ((int)g * filter) / 255;
	c.b = ((int)b * filter) / 255;
	return c;
}

inline Bgr &Bgr::operator*=(int filter) {
	r = ((int)r * filter) / 255;
	g = ((int)g * filter) / 255;
	b = ((int)b * filter) / 255;
	return *this;
}

inline Bgr Bgr::operator*(float scale) const {
	Bgr c;

	c.r = Math::clampByte(scale * r);
	c.g = Math::clampByte(scale * g);
	c.b = Math::clampByte(scale * b);
	return c;
}

inline Bgr &Bgr::operator*=(float scale) {
	r = Math::clampByte(scale * r);
	g = Math::clampByte(scale * g);
	b = Math::clampByte(scale * b);
	return *this;
}

inline Bgr Bgr::operator+(const Bgr &other) const {
	Bgr c;
	c.r = Math::clampByte((int)r + other.r);
	c.g = Math::clampByte((int)g + other.g);
	c.b = Math::clampByte((int)b + other.b);
	return c;
}

inline Bgr &Bgr::operator+=(const Bgr &other) {
	r = Math::clampByte((int)r + other.r);
	g = Math::clampByte((int)g + other.g);
	b = Math::clampByte((int)b + other.b);
	return *this;
}

inline byte_t &Bgr::operator[](int index) {
	AX_STRICT_ASSERT(index>=0 && index < 3);
	return ((byte_t*)this)[index];
}

inline byte_t Bgr::operator[](int index) const {
	AX_STRICT_ASSERT(index>=0 && index < 3);
	return ((byte_t*)this)[index];
}


//------------------------------------------------------------------------------
// Bgra
//------------------------------------------------------------------------------

struct AX_API Bgra {
	byte_t b, g, r, a;

	// constructor and destructor
	Bgra() {}

	Bgra(byte_t ir, byte_t ig, byte_t ib, byte_t ia=0xFF)
		: b(ib), g(ig), r(ir), a(ia) {}

	Bgra(const Vector3 &vec)
		: b(Math::clampByte(vec.z*255.f))
		, g(Math::clampByte(vec.y*255.f))
		, r(Math::clampByte(vec.x*255.f))
		, a(255) {}

	Bgra(const Vector4 &plane)
		: b(Math::clampByte(plane.z*255.f))
		, g(Math::clampByte(plane.y*255.f))
		, r(Math::clampByte(plane.x*255.f))
		, a(Math::clampByte(plane.w*255.f)) {}

	// operator overridden
	Bgra &operator=(const Vector3 &v) {
		r = Math::clampByte(v.x*255.f);
		g = Math::clampByte(v.y*255.f);
		b = Math::clampByte(v.z*255.f);
		a = 255;
		return *this;
	}

	Bgra &operator=(const Vector4 &v) {
		r = Math::clampByte(v.x*255.f);
		g = Math::clampByte(v.y*255.f);
		b = Math::clampByte(v.z*255.f);
		a = Math::clampByte(v.w*255.f);
		return *this;
	}
	bool operator==(const Bgra &color) const {
		return r == color.r && g == color.g && b == color.b && a == color.a;
	}

	bool operator!=(const Bgra &color) const {
		return r != color.r || g != color.g || b != color.b || a != color.a;
	}

	operator const byte_t*() const { return (byte_t*)this; }

	void clear() { r=g=b=a=0; }

	Bgra &Set(byte_t ir, byte_t ig, byte_t ib, byte_t ia=255) {
		r=ir; g=ig; b=ib; a=ia; return *this;
	}

	Bgra operator*(const Bgra &other) const {
		Bgra c;
		c.r = ((int)r * other.r) >> 8;
		c.g = ((int)g * other.g) >> 8;
		c.b = ((int)b * other.b) >> 8;
		c.a = a;
		return c;
	}

	Bgra operator*(float scale) const {
		Bgra c;

		c.r = (byte_t)(scale * r);
		c.g = (byte_t)(scale * g);
		c.b = (byte_t)(scale * b);
		c.a = a;
		return c;
	}

	Bgra operator+(const Bgra &other) const {
		Bgra c;

		c.r = r + other.r;
		c.g = g + other.g;
		c.b = b + other.b;
		c.a = a + other.a;

		return c;
	}

	Bgra operator-(const Bgra &other) const {
		Bgra c;

		c.r = r - other.r;
		c.g = g - other.g;
		c.b = b - other.b;
		c.a = a - other.a;

		return c;
	}

	Bgra operator*=(float scale) {
		return (*this * scale);
	}
};

//------------------------------------------------------------------------------
// convert
//------------------------------------------------------------------------------

inline Bgr Rgba::bgr() const {
	return Bgr(r,g,b);
}


struct Color3 : public Vector3
{
public:
	// constructor and destructor
	inline Color3() {}
	inline Color3(float ix, float iy, float iz) : Vector3(ix,iy,iz) {}
	inline Color3(const Vector2 &v, float iz) : Vector3(v, iz) {}
	inline Color3(const Vector3& vec3) : Vector3(vec3) {}
	inline ~Color3() {}
};



AX_END_NAMESPACE

#endif // end guardian


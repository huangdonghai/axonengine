/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_VECTOR2_H
#define AX_CORE_VECTOR2_H

namespace Axon {

	struct AX_API Vector2 {
		float x, y;

		// constructor and destructor
		Vector2();
		Vector2(float ix, float iy);
		~Vector2();

		Vector2 operator+(const Vector2& v) const;
		Vector2 operator-() const;
		Vector2 operator-(const Vector2& v) const;
		Vector2 operator*(const float scale) const;
		Vector2 operator*(const Vector2& v) const;
		Vector2 operator/(const Vector2& v) const;
		Vector2 operator/(float scale) const;
		float operator|(const Vector2& v) const;
		bool operator==(const Vector2& v) const;
		bool operator!=(const Vector2& v) const;
		Vector2 operator+=(const Vector2& v);
		Vector2 operator-=(const Vector2& v);
		Vector2 operator*=(float scale);
		Vector2 operator/=(float v);
		Vector2 operator*=(const Vector2& v);
		Vector2 operator/=(const Vector2& v);
		float& operator[](int index);
		const float& operator[](int index) const;
		float getMax() const;
		float getAbsMax() const;
		float getMin() const;
		float getLength() const;
		float getLengthSquared() const;
		bool isZero() const;
		float normalize();
		Vector2 getNormalized();
		void clear();
		Vector2& set(float ix=0.0f, float iy=0.0f);
		const float* toFloatPointer() const;
		String toString() const;
		bool parse(const String& str);
		bool parse(const char* text);
	};

	inline Vector2::Vector2(){}
	inline Vector2::Vector2(float ix, float iy) : x(ix), y(iy) {}
	inline Vector2::~Vector2(){}

	// algo
	inline Vector2 Vector2::operator+(const Vector2& v) const {
		return Vector2(x + v.x, y + v.y);
	}
	// negative vector
	inline Vector2 Vector2::operator-() const {
		return Vector2(-x, -y);
	}
	inline Vector2 Vector2::operator-(const Vector2& v) const {
		return Vector2(x - v.x, y - v.y);
	}
	// scale the vector
	inline Vector2 Vector2::operator*(const float scale) const {
		return Vector2(x*scale, y*scale);
	}
	// multiply
	inline Vector2 Vector2::operator*(const Vector2& v) const {
		return Vector2(x*v.x, y*v.y);
	}
	// div
	inline Vector2 Vector2::operator/(const Vector2& v) const {
		return Vector2(x/v.x, y/v.y);
	}
	// inverse scale
	inline Vector2 Vector2::operator/(float scale) const {
		return Vector2(x/scale, y/scale);
	}
	// Dot Product
	inline float Vector2::operator|(const Vector2& v) const {
		return x*v.x + y*v.y;
	}
	// compare
	inline bool Vector2::operator==(const Vector2& v) const {
		return x==v.x && y==v.y;
	}
	inline bool Vector2::operator!=(const Vector2& v) const {
		return x!=v.x || y!=v.y;
	}

	// assign operator
	inline Vector2 Vector2::operator+=(const Vector2& v) {
		x += v.x; y += v.y;
		return *this;
	}
	inline Vector2 Vector2::operator-=(const Vector2& v) {
		x -= v.x; y -= v.y;
		return *this;
	}
	inline Vector2 Vector2::operator*=(float scale) {
		x *= scale; y *= scale;
		return *this;
	}
	inline Vector2 Vector2::operator/=(float v) {
		x /= v; y /= v;
		return *this;
	}
	inline Vector2 Vector2::operator*=(const Vector2& v) {
		x *= v.x; y *= v.y;
		return *this;
	}
	inline Vector2 Vector2::operator/=(const Vector2& v) {
		x /= v.x; y /= v.y;
		return *this;
	}
	inline float& Vector2::operator[](int index) {
		AX_STRICT_ASSERT(index >= 0);
		AX_STRICT_ASSERT(index < 2);
		return *(&x + index);
	}

	inline const float& Vector2::operator[](int index) const {
		AX_STRICT_ASSERT(index >= 0);
		AX_STRICT_ASSERT(index < 2);
		return *(&x + index);
	}

	// Simple functions.
	inline float Vector2::getMax() const {
		return std::max(x,y);
	}
	inline float Vector2::getAbsMax() const {
		return std::max(Math::abs(x), Math::abs(y));
	}
	inline float Vector2::getMin() const {
		return std::min(x,y);
	}
	inline float Vector2::getLength() const {
		return (float)sqrt((double)(x*x + y*y));
	}
	inline float Vector2::getLengthSquared() const {
		return x*x + y*y;
	}
	inline bool Vector2::isZero() const {
		return x==0.f && y==0.f;
	}
	// return length
	inline float Vector2::normalize() {
		float s = x*x+y*y;
		if (s >= 1.0e-8f) {
			float scale = 1.f/sqrt(s);
			x *= scale; y *= scale;
			return s;
		} else {
			x=y=0.f;
			return 0.f;
		}
	}
	// return self value
	inline Vector2 Vector2::getNormalized() {
		float s = x*x+y*y;
		if (s >= 1.0e-8f) {
			float scale = 1.f/sqrt(s);
			return Vector2(x*scale,y*scale);
		} else {
			return Vector2(0,0);
		}
	}
	// Clear self
	inline void Vector2::clear() {
		x=0.0f;y=0.0f;
	}
	// Set value
	inline Vector2& Vector2::set(float ix, float iy) {
		x = ix; y = iy;
		return *this;
	}
	inline const float* Vector2::toFloatPointer() const {
		return &x;
	}

	inline String Vector2::toString() const {
		String result;
		StringUtil::sprintf(result, "%f,%f", x, y);
		return result;
	}

	inline bool Vector2::parse(const String& str) {
		return parse(str.c_str());
	}

	inline bool Vector2::parse(const char* text) {
		float tx, ty;
		int v = sscanf(text, "%f,%f", &tx, &ty);
		if (v != 2)
			return false;
		x = tx; y = ty;
		return true;
	}
} // namespace Axon

#endif // end guardian


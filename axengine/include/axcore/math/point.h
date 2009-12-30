/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_POINT_H
#define AX_CORE_POINT_H

AX_BEGIN_NAMESPACE

	struct AX_API Point {
		int x, y;

		// constructor and destructor
		inline Point(){}
		inline Point(int ix, int iy) : x(ix), y(iy) {}
		inline ~Point(){}

		// algo
		inline Point operator+(const Point& v) const {
			return Point(x + v.x, y + v.y);
		}

		inline Point operator+(int v) const {
			return Point(x+v, y+v);
		}
		inline Point& operator+=(int v) {
			x += v; y += v;
			return *this;
		}
		inline Point operator-(int v) const {
			return Point(x-v, y-v);
		}
		inline Point& operator-=(int v) {
			x -= v;
			y -= v;
			return *this;
		}
		// negative vector
		inline Point operator-() const {
			return Point(-x, -y);
		}
		inline Point operator-(const Point& v) const {
			return Point(x - v.x, y - v.y);
		}
		// scale the vector
		inline Point operator*(const int scale) const {
			return Point(x*scale, y*scale);
		}
		// multiply
		inline Point operator*(const Point& v) const {
			return Point(x*v.x, y*v.y);
		}
		// div
		inline Point operator/(const Point& v) const {
			return Point(x/v.x, y/v.y);
		}
		// inverse scale
		inline Point operator/(int scale) const {
			return Point(x/scale, y/scale);
		}
		// Dot Product
		inline int operator|(const Point& v) const {
			return x*v.x + y*v.y;
		}
		// compare
		inline bool operator==(const Point& v) const {
			return x==v.x && y==v.y;
		}
		bool operator!=(const Point& v) const {
			return x!=v.x || y!=v.y;
		}

		// assign operator
		inline Point operator+=(const Point& v) {
			x += v.x; y += v.y;
			return *this;
		}
		inline Point operator-=(const Point& v) {
			x -= v.x; y -= v.y;
			return *this;
		}
		inline Point operator*=(int scale) {
			x *= scale; y *= scale;
			return *this;
		}
		inline Point operator/=(int v) {
			x /= v; y /= v;
			return *this;
		}
		inline Point operator*=(const Point& v) {
			x *= v.x; y *= v.y;
			return *this;
		}
		inline Point operator/=(const Point& v) {
			x /= v.x; y /= v.y;
			return *this;
		}
		inline int& operator[](int index) {
			AX_STRICT_ASSERT(index >= 0);
			AX_STRICT_ASSERT(index < 2);
			return *(&x + index);
		}

		inline int operator[](int index) const {
			AX_STRICT_ASSERT(index >= 0);
			AX_STRICT_ASSERT(index < 2);
			return *(&x + index);
		}

		// Simple functions.
		inline int getMax() const {
			return std::max(x,y);
		}
		inline int getAbsMax() const {
			return std::max(Math::abs(x), Math::abs(y));
		}
		inline int getMin() const {
			return std::min(x,y);
		}
		inline int getLength() const {
			return (int)sqrt((double)(x*x + y*y));
		}
		inline int getLengthSquared() const {
			return x*x + y*y;
		}
		inline bool isZero() const {
			return x==0 && y==0;
		}
		// Clear self
		inline void clear() {
			x=0;y=0;
		}
		// Set value
		inline Point& set(int ix=0, int iy=0) {
			x = ix; y = iy;
			return *this;
		}
		inline operator const int*() const {
			return &x;
		}

		String toString() const;
		bool fromString(const char* str);
	};


AX_END_NAMESPACE

#endif // end guardian


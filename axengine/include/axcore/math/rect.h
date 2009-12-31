/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_RECT_H
#define AX_CORE_RECT_H

AX_BEGIN_NAMESPACE

	// 2D screen coordinate rectangle
	struct AX_API Rect {
		int x, y, width, height;				// x, y, width, height

		Rect();
		Rect(int ix, int iy, int iw, int ih);
		Rect(const Rect &rect);
		~Rect();

		// add need function below
		void clear();
		bool operator==(const Rect &other) const;
		bool contains(int ix, int iy) const;
		bool contains(const Point &point) const;
		bool contains(const Rect &r) const;
		bool intersects(const Rect &r);
		Point getCenter() const;
		Point getMins() const;
		Point getMaxs() const;
		int xMin() const;
		int yMin() const;
		int xMax() const;
		int yMax() const;

		template< class Q >
		Rect &operator/=(Q scale);
		template< class Q >
		Rect operator/(Q scale) const;
		template< class Q >
		Rect &operator*=(Q scale);
		template< class Q >
		Rect operator*(Q scale) const;

		// if two rect don't intersected, returned rect have zero or negative width
		// and height, check isEmpty()
		// self rect and other rect must have positive width and height to do this function
		Rect intersect(const Rect &other) const;

		// like intersect, but returned rect is self offset base, for texture clipping
		Rect intersectToLocal(const Rect &other) const;

		// return a new rect contain both two rect
		Rect unite(const Rect &other) const;

		// just do intersect
		Rect operator&(const Rect &other) const;
		Rect &operator&=(const Rect &other);
		Rect operator|(const Rect &other) const;
		Rect &operator|=(const Rect &other);
		// just do offset, no resize
		Rect operator-(const Point &point) const;
		Rect &operator-=(const Point &point);
		// just do offset, no resize
		Rect operator+(const Point &point) const;
		Rect &operator+=(const Point &point);
		bool isEmpty() const;
		void deflate(int x, int y);
		void deflate(const Point &point);
		void inflate(int x, int y);
		void inflate(const Point &point);

		void offset(int x);
		void offset(int x, int y);
		void offset(const Point &point);

		Vector4 toVector4() const;

		String toString() const;
		bool fromString(const char *str);
	};

	inline Rect::Rect() {}
	inline Rect::Rect(int ix, int iy, int iw, int ih) : x(ix), y(iy), width(iw), height(ih) {}
	inline Rect::Rect(const Rect &rect) : x(rect.x), y(rect.y), width(rect.width), height(rect.height) {}
	inline Rect::~Rect() {}

	// add need function below
	inline void Rect::clear() {
		x = std::numeric_limits<int>::max();
		y = std::numeric_limits<int>::max();
		width = std::numeric_limits<int>::min();
		height = std::numeric_limits<int>::min();
	}

	inline bool Rect::operator==(const Rect &other) const {
		return (x==other.x) && (y==other.y) && (width==other.width) && (height==other.height);
	}

	inline bool Rect::contains(int ix, int iy) const {
		return ix >= x && ix < x+width && iy >= y && iy < y+height;
	}

	inline bool Rect::contains(const Point &point) const {
		return contains(point.x, point.y);
	}

	inline bool Rect::contains(const Rect &r) const {
		if (r.isEmpty())
			return true;

		if (isEmpty())
			return false;

		return x <= r.x && y <= r.y && xMax() >= r.xMax() && yMax() >= r.yMax();
	}

	inline bool Rect::intersects(const Rect &r) {
		if (x > r.xMax())
			return false;
		if (y > r.yMax())
			return false;
		if (r.x > xMax())
			return false;
		if (r.y > yMax())
			return false;
		return true;
	}

	inline Point Rect::getCenter() const { return Point(x + width / 2, y + height / 2); }
	inline Point Rect::getMins() const { return Point(x, y); }
	inline Point Rect::getMaxs() const { return Point(x+width, y+height); }
	inline int Rect::xMin() const { return x; }
	inline int Rect::yMin() const { return y; } // bottom
	inline int Rect::xMax() const { return x + width; }
	inline int Rect::yMax() const { return y + height; }

	template< class Q >
	Rect &Rect::operator/=(Q scale) {
		x /= scale;
		y /= scale;
		width /= scale;
		height /= scale;

		return *this;
	}

	template< class Q >
	Rect Rect::operator/(Q scale) const {
		return Rect(x/scale, y/scale, width/scale, height/scale);
	}

	template< class Q >
	Rect &Rect::operator*=(Q scale) {
		x *= scale;
		y *= scale;
		width *= scale;
		height *= scale;

		return *this;
	}

	template< class Q >
	Rect Rect::operator*(Q scale) const {
		return Rect(x*scale, y*scale, width*scale, height*scale);
	}

	// if two rect don't intersected, returned rect have zero or negative width
	// and height, check isEmpty()
	// self rect and other rect must have positive width and height to do this function
	inline Rect Rect::intersect(const Rect &other) const {
		Rect r(0, 0, 0, 0);

		r.x = std::max(x, other.x);
		r.y = std::max(y, other.y);
		r.width = std::min(xMax(), other.xMax()) - r.x;
		r.height = std::min(yMax(), other.yMax()) - r.y;

		return r;
	}

	// lick intersect, but returned rect is self offset base, for texture clipping
	inline Rect Rect::intersectToLocal(const Rect &other) const {
		return intersect(other) - getMins();
	}

	// return a new rect contain both two rect
	inline Rect Rect::unite(const Rect &other) const {
		if (isEmpty())
			return other;

		Rect r;

		r.x = std::min(xMin(), other.xMin());
		r.y = std::min(yMin(), other.yMin());
		r.width = std::max(xMax(), other.xMax());
		r.height = std::max(yMax(), other.yMax());

		r.width -= r.x;
		r.height -= r.y;

		return r;
	}

	// just do intersect
	inline Rect Rect::operator&(const Rect &other) const {
		return intersect(other);
	}

	inline Rect &Rect::operator&=(const Rect &other) {
		*this = *this & other;

		return *this;
	}

	inline Rect Rect::operator|(const Rect &other) const {
		return unite(other);
	}

	inline Rect &Rect::operator|=(const Rect &other) {
		*this = *this | other;

		return *this;
	}

	// just do offset, no resize
	inline Rect Rect::operator-(const Point &point) const {
		return Rect(x - point.x, y - point.y, width, height);
	}

	inline Rect &Rect::operator-=(const Point &point) {
		x -= point.x;
		y -= point.y;

		return *this;
	}

	// just do offset, no resize
	inline Rect Rect::operator+(const Point &point) const {
		return Rect(x + point.x, y + point.y, width, height);
	}

	inline Rect &Rect::operator+=(const Point &point) {
		x += point.x;
		y += point.y;
		return *this;
	}

	inline bool Rect::isEmpty() const {
		return width <= 0 || height <= 0;
	}


	inline void Rect::deflate(int m_x, int m_y) {
		x += m_x;
		y += m_y;
		width -= 2 * m_x;
		height -= 2 * m_y;
	}

	inline void Rect::deflate(const Point &point) {
		deflate(point.x, point.y);
	}

	inline void Rect::inflate(int m_x, int m_y) {
		deflate(-m_x, -m_y);
	}

	inline void Rect::inflate(const Point &point) {
		inflate(point.x, point.y);
	}

	inline void  Rect::offset(int x) {
		offset(x, x);
	}
	inline void  Rect::offset(int x, int y) {
		this->x += x;
		this->y += y;
	}
	inline void  Rect::offset(const Point &point) {
		offset(point.x, point.y);
	}

	inline Vector4 Rect::toVector4() const {
		Vector4 v;
		v.x = x;
		v.y = y;
		v.z = width;
		v.w = height;
		return v;
	}

AX_END_NAMESPACE

#endif // end guardian


#ifndef AX_CORE_SIZE_H
#define AX_CORE_SIZE_H

AX_BEGIN_NAMESPACE

struct Size {
	Size() {}
	Size(int w, int h) : width(w), height(h) {}

	bool isZero() const { return width == 0 && height == 0; }
	void set(int w, int h) { width = w; height = h; }

	Size &operator+=(const Size &rhs) { width += rhs.width; height += rhs.height; return *this; }
	Size &operator-=(const Size &rhs) { width -= rhs.width; height -= rhs.height; return *this; }
	Size &operator*=(float c) { width *= c; height *= c; return *this; }
	Size &operator/=(float c) { return *this *= (1.0f/c); }

	friend inline bool operator==(const Size &, const Size &);
	friend inline bool operator!=(const Size &, const Size &);
	friend inline bool operator<=(const Size &, const Size &);
	friend inline const Size operator+(const Size &, const Size &);
	friend inline const Size operator-(const Size &, const Size &);
	friend inline const Size operator*(const Size &, float);
	friend inline const Size operator*(float, const Size &);
	friend inline const Size operator/(const Size &, float);

	int width, height;
};

inline bool operator==(const Size &a, const Size &b) { return a.width == b.width && a.height == b.height; }
inline bool operator!=(const Size &a, const Size &b) { return !(a == b); }
inline bool operator<=(const Size &a, const Size &b) { return a.width <= b.width && a.height <= b.height; }
inline const Size operator+(const Size &a, const Size &b) { return Size(a.width+b.width, a.height+b.height); }
inline const Size operator-(const Size &a, const Size &b) { return Size(a.width-b.width, a.height-b.height); }
inline const Size operator*(const Size &a, float f) { return Size(a.width * f, a.height * f); }
inline const Size operator*(float f, const Size &b) { return b * f; }
inline const Size operator/(const Size &a, float f) { return a * (1.0f / f); }


struct SizeF {
	SizeF();
	SizeF(float w, float h) : width(w), height(h) {}
	float width, height;
};


AX_END_NAMESPACE

#endif // AX_CORE_SIZE_H
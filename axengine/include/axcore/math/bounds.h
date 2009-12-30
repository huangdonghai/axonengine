/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_BOUNDS_H
#define AX_CORE_BOUNDS_H

AX_BEGIN_NAMESPACE

	//--------------------------------------------------------------------------
	// struct BoundingRange, 1D bounding
	//--------------------------------------------------------------------------

	struct AX_API BoundingRange {
		float min, max;

		BoundingRange();
		BoundingRange(float l, float h);
		~BoundingRange();

		void clear();
		void set(float l, float h);
		void set(const Vector2& v);
		void expandBy(float f);
		float width() const;
		bool isEmpty() const;
		bool contains(float f) const;
		bool contains(const BoundingRange& other) const;
		bool intersects(const BoundingRange& other) const;
		BoundingRange intersected(const BoundingRange& other) const;
		float distance(float f) const;
		void deflate(float f);
		void inflate(float f);
	};

	inline BoundingRange::BoundingRange() {}
	inline BoundingRange::BoundingRange(float l, float h) : min(l), max(h) {}
	inline BoundingRange::~BoundingRange() {}

	inline void BoundingRange::clear() {
		min = std::numeric_limits<float>::max();
		max = -min;
	}

	inline void BoundingRange::set(float l, float h) {
		min = l; max = h;
	}

	inline void BoundingRange::set(const Vector2& v) {
		min = v.x; max = v.y;
	}

	inline void BoundingRange::expandBy(float f) {
		min = std::min(f, min);
		max = std::max(f, max);
	}

	inline float BoundingRange::width() const {
		return max - min;
	}

	inline bool BoundingRange::isEmpty() const {
		return width() <= 0;
	}

	inline bool BoundingRange::contains(float f) const {
		return f >= min && f <= max;
	}

	inline bool BoundingRange::contains(const BoundingRange& other) const {
		return min <= other.min && max >= other.max;
	}

	inline bool BoundingRange::intersects(const BoundingRange& other) const {
		return !intersected(other).isEmpty();
	}

	inline BoundingRange BoundingRange::intersected(const BoundingRange& other) const {
		BoundingRange result;
		result.min = std::max(min, other.min);
		result.max = std::min(max, other.max);
		return result;
	}

	inline float BoundingRange::distance(float f) const {
		if (contains(f)) return 0;

		if (f < min) return min - f;
		
		return f - max;
	}

	inline void BoundingRange::deflate(float f) {
		min += f; max -= f;
	}

	inline void BoundingRange::inflate(float f) {
		deflate(-f);
	}

	//--------------------------------------------------------------------------
	// struct BoundingRect, 2D bounding
	//--------------------------------------------------------------------------

	struct AX_API BoundingRect {
		Vector2 min, max;

		BoundingRect() {}
		BoundingRect(const BoundingRect& other) : min(other.min), max(other.max) {}
		BoundingRect(const Vector2& _mins, const Vector2& _maxs) : min(_mins), max(_maxs) {}
		BoundingRect(float min_x, float min_y, float max_x, float max_y) : min(min_x,min_y), max(max_x,max_y) {}
		~BoundingRect() {}

		inline bool empty() const {
			return (min.x >= max.x || min.y >= max.y);
		}

		inline bool contains(const Vector2& p) const {
			return (p.x >= min.x && p.y >= min.y && p.x < max.x && p.y < max.y);
		}

		inline float width() const {
			return max.x - min.x;
		}

		inline float height() const {
			return max.y - min.y;
		}

		inline bool intersects(const BoundingRect& other, BoundingRect& intersection) const {
			if ((*this).empty() || other.empty()) {
				intersection.clear();
				return false;
			}
			intersection.min.x = std::max(min.x, other.min.x);
			intersection.min.y = std::max(min.y, other.min.y);
			intersection.max.x = -std::max(max.x, other.max.x);
			intersection.max.y = -std::max(max.y, other.max.y);
			return !intersection.empty();
		}

		inline void clear() {
			min.x = min.y = std::numeric_limits<float>::max();
			max.x = max.y = std::numeric_limits<float>::min();
		}

		inline void expandBy(const Vector2& p) {
			for (int i=0; i<2; i++) {
				if (p[i] < min[i])
					min[i] = p[i];
				if (p[i] > max[i])
					max[i] = p[i];
			}
		}

		inline void clipMinX(float v) {
			if (min.x < v)
				min.x = v;
		}

		inline void clipMinY(float v) {
			if (min.y < v)
				min.y = v;
		}

		inline void clipmaxX(float v) {
			if (max.x > v)
				max.x = v;
		}

		inline void clipmaxY(float v) {
			if (max.y > v)
				max.y = v;
		}

		inline void clampVertex(Vector2& v) {
			v.x = Math::clamp(v.x, min.x, max.x);
			v.y = Math::clamp(v.y, min.y, max.y);
		}

		inline void move(float mx, float my) {
			min.x += mx;
			min.y += my;
			max.x += mx;
			max.y += my;
		}

		inline BoundingRect operator + (const BoundingRect& other) const {
			BoundingRect ret;
			ret.min.x = std::min(min.x, other.min.x);
			ret.min.y = std::min(min.y, other.min.y);
			ret.max.x = std::max(max.x, other.max.x);
			ret.max.y = std::max(max.y, other.max.y);
			return ret;
		}

		inline BoundingRect operator += (const BoundingRect& other) {
			min.x = std::min(min.x, other.min.x);
			min.y = std::min(min.y, other.min.y);
			max.x = std::max(max.x, other.max.x);
			max.y = std::max(max.y, other.max.y);
			return *this;
		}
	};


	//--------------------------------------------------------------------------
	// struct BoundingBox, 3D bounding
	//--------------------------------------------------------------------------

	struct AX_API BoundingBox {
		static const BoundingBox EmptyBox;
		static const BoundingBox UnitBox;
		static const BoundingBox LargestBox;

		Vector3 min, max;

		BoundingBox();
		BoundingBox(const BoundingBox& other);
		BoundingBox(const Vector3& _mins, const Vector3& _maxs);
		BoundingBox(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z);
		~BoundingBox();

		bool empty() const;
		bool contains(const Vector3& p) const;
		bool contains(const BoundingBox& rhs) const;
		void clear();
		void expandBy(const Vector3& p);
		Vector3 getCenter() const;
		// return min + (max - min) * t
		Vector3 getLerped(const Vector3& t) const;
		// (max-min) * c
		Vector3 getExtends(const Vector3& t) const;

		Vector3 getExtends() const;

		BoundingBox& unite(const BoundingBox& rhs);

		BoundingBox intersected(const BoundingBox& other) const;
		BoundingBox united(const BoundingBox& other) const;

		BoundingBox getTransformed(const AffineMat& matrix) const;
		float pointDistance(const Vector3& point);
	};

	inline BoundingBox::BoundingBox()
	{}

	inline BoundingBox::BoundingBox(const BoundingBox& other)
		: min(other.min), max(other.max)
	{}

	inline BoundingBox::BoundingBox(const Vector3& _mins, const Vector3& _maxs)
		: min(_mins), max(_maxs)
	{}

	inline BoundingBox::BoundingBox(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z)
		: min(Vector3(min_x,min_y,min_z))
		, max(Vector3(max_x,max_y,max_z))
	{}

	inline BoundingBox::~BoundingBox()
	{}

	inline bool BoundingBox::empty() const {
		return (min.x >= max.x
			|| min.y >= max.y
			|| min.z >= max.z);
	}

	inline bool BoundingBox::contains(const Vector3& p) const {
		return (p.x >= min.x && p.y >= min.y && p.z >= min.z
			&& p.x < max.x && p.y < max.y && p.z < max.z);
	}

	inline bool BoundingBox::contains(const BoundingBox& rhs) const {
		return min <= rhs.min && max >= rhs.max;
	}

	inline void BoundingBox::clear() {
		min.x = min.y = min.z = +999999999.f;
		max.x = max.y = max.z = -999999999.f;
	}

	inline void BoundingBox::expandBy(const Vector3& p) {
		for (int i=0; i<3; i++) {
			if (p[i] < min[i])
				min[i] = p[i];
			if (p[i] > max[i])
				max[i] = p[i];
		}
	}

	inline Vector3 BoundingBox::getCenter() const {
		return (min + max) * 0.5f;
	}

	// return low + (high - low) * t
	inline Vector3 BoundingBox::getLerped(const Vector3& t) const {
		return min + getExtends(t);
	}

	// (high-low) * c
	inline Vector3 BoundingBox::getExtends(const Vector3& t) const {
		return (max - min) * t;
	}

	inline Vector3 BoundingBox::getExtends() const {
		return max - min;
	}

	inline BoundingBox& BoundingBox::unite(const BoundingBox& rhs) {
		expandBy(rhs.min);
		expandBy(rhs.max);
		return *this;
	}

	inline BoundingBox BoundingBox::intersected(const BoundingBox& other) const {
		BoundingBox result;
		result.min.x = std::max(min.x, other.min.x);
		result.min.y = std::max(min.y, other.min.y);
		result.min.z = std::max(min.z, other.min.z);
		result.max.x = std::min(max.x, other.max.x);
		result.max.y = std::min(max.y, other.max.y);
		result.max.z = std::min(max.z, other.max.z);
		return result;
	}

	inline BoundingBox BoundingBox::united(const BoundingBox& other) const {
		BoundingBox result = *this;
		result.expandBy(other.min);
		result.expandBy(other.max);
		return result;
	}

#if 0
	inline BoundingBox BoundingBox::getTransformed(const Vector3& origin, const Matrix3& axis) const {
		int i;
		Vector3 center, extents, rotatedExtents;
		BoundingBox out;

		center = getCenter();
		extents = high - center;

		for (i = 0; i < 3; i++) {
			rotatedExtents[i] = fabsf(extents[0] * axis[0][i]) +
				fabsf(extents[1] * axis[1][i]) +
				fabsf(extents[2] * axis[2][i]);
		}

		center = origin + axis * center;
		out.low = center - rotatedExtents;
		out.high = center + rotatedExtents;

		return out;
	}

	inline BoundingBox BoundingBox::getTransformed(const Vector3& origin, const Angles& angles) const {
		Matrix3 m;
		m.fromAngles(angles);
		return getTransformed(origin, m);
	}

	inline BoundingBox BoundingBox::getTransformed(const Vector3& origin, const Angles& angles, const Vector3& scales) const {
		Matrix3 m;
		m.fromAnglesScales(angles, scales);
		return getTransformed(origin, m);
	}
#endif

	inline float BoundingBox::pointDistance(const Vector3& point) {
		Vector3 dist;
		for (int i = 0; i < 3; i++) {
			if (point[i] < min[i]) {
				dist[i] = min[i] - point[i];
			} else if (point[i] > max[i]) {
				dist[i] = point[i] - max[i];
			} else {
				dist[i] = 0;
			}
		}
		return dist.getLength();
	}

AX_END_NAMESPACE

#endif // end guardian


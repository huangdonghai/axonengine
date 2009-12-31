/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_MATRIX3_H
#define AX_CORE_MATRIX3_H

AX_BEGIN_NAMESPACE

	//--------------------------------------------------------------------------
	// Matrix3, column major
	//--------------------------------------------------------------------------

	struct AX_API Matrix3
	{
		enum { Forward, Left, Up };

		static const Matrix3 Identity;

		Vector3 m[3];

		Matrix3();
		Matrix3(const float in[9]);
		Matrix3(const Quaternion &q);
		Matrix3(const Vector3 &forward, const Vector3 &left, const Vector3 &up);
		Matrix3(float e0, float e1, float e2, float e3, float e4, float e5, float e6, float e7, float e8);
		~Matrix3();

		void clear() { m[0].clear(); m[1].clear(); m[2].clear(); }

		// operator
		Vector3 &operator[] (int index);
		const Vector3 &operator[] (int index) const;
		Vector3 getRow(int index) const;
		const Matrix3 &operator*=(float scale);
		Matrix3 operator*(const Matrix3 &im) const;
		bool operator==(const Matrix3 &other) const;
		bool operator!=(const Matrix3 &other) const;

		// transform a vector
		Vector3 transformPoint(const Vector3 &p) const;
		Vector3 operator*(const Vector3 &p) const;
		// function
		void setIdentity();
		void fromQuaternion(const Quaternion &q);

		// implemented in angles.hpp
		void fromAngles(const Angles &angles);
		void fromAnglesScales(const Angles &angles, float scale);

		// must no scale to convert angles
		Angles toAngles() const;
		Matrix3 getInverse() const;
		Matrix3 getTranspose() const;

		void toAnglesScale(Angles &angles, float &scale) const;

		Matrix3 scale(float s) const;

		// remove scale transform
		float removeScale();
		// remove shear transform
		void removeShear();

		const float *toFloatPointer() const;
		operator const float*() const;

		String toString() const;
		void fromString(const char *str);

		static Matrix3 getIdentity();
	};

	inline Matrix3::Matrix3()
	{}

	inline Matrix3::Matrix3(const float in[9])
	{
		memcpy(m, in, sizeof(in));
	}

	inline Matrix3::Matrix3(const Quaternion &q)
	{
		fromQuaternion(q);
	}

	inline Matrix3::Matrix3(const Vector3 &forward, const Vector3 &left, const Vector3 &up)
	{
		m[0][0] = forward.x;m[0][1] = forward.y;m[0][2] = forward.z;
		m[1][0] = left.x;	m[1][1] = left.y;	m[1][2] = left.z;
		m[2][0] = up.x;		m[2][1] = up.y;		m[2][2] = up.z;
	}

	inline Matrix3::Matrix3(float e0, float e1, float e2, float e3, float e4, float e5, float e6, float e7, float e8)
	{
		m[0][0] = e0;  m[0][1] = e1;   m[0][2] = e2;
		m[1][0] = e3;  m[1][1] = e4;   m[1][2] = e5;
		m[2][0] = e6;  m[2][1] = e7;   m[2][2] = e8;
	}

	inline Matrix3::~Matrix3()
	{}

	// operator
	inline Vector3 &Matrix3::operator[] (int index)
	{
		AX_STRICT_ASSERT(index>=0 && index<3);
		return m[index];
	}

	inline const Vector3 &Matrix3::operator[] (int index) const
	{
		AX_STRICT_ASSERT(index>=0 && index<3);
		return m[index];
	}

	inline Vector3 Matrix3::getRow(int index) const
	{
		AX_STRICT_ASSERT(index>=0 && index<3);
		return Vector3(m[0][index], m[1][index], m[2][index]);
	}

	inline const Matrix3 &Matrix3::operator*=(float scale)
	{
		m[0] *= scale;
		m[1] *= scale;
		m[2] *= scale;
		return *this;
	}


	inline Matrix3 Matrix3::operator*(const Matrix3 &im) const
	{
		Matrix3 result;

		result[0][0] = m[0][0]*im[0][0] + m[1][0]*im[0][1] + m[2][0]*im[0][2];
		result[0][1] = m[0][1]*im[0][0] + m[1][1]*im[0][1] + m[2][1]*im[0][2];
		result[0][2] = m[0][2]*im[0][0] + m[1][2]*im[0][1] + m[2][2]*im[0][2];

		result[1][0] = m[0][0]*im[1][0] + m[1][0]*im[1][1] + m[2][0]*im[1][2];
		result[1][1] = m[0][1]*im[1][0] + m[1][1]*im[1][1] + m[2][1]*im[1][2];
		result[1][2] = m[0][2]*im[1][0] + m[1][2]*im[1][1] + m[2][2]*im[1][2];

		result[2][0] = m[0][0]*im[2][0] + m[1][0]*im[2][1] + m[2][0]*im[2][2];
		result[2][1] = m[0][1]*im[2][0] + m[1][1]*im[2][1] + m[2][1]*im[2][2];
		result[2][2] = m[0][2]*im[2][0] + m[1][2]*im[2][1] + m[2][2]*im[2][2];

		return result;
	}

	inline bool Matrix3::operator==(const Axon::Matrix3 &other) const
	{
		return m[0] == other.m[0]
			&& m[1] == other.m[1]
			&& m[2] == other.m[2];
	}

	inline bool Matrix3::operator!=(const Axon::Matrix3 &other) const
	{
		return !this->operator==(other);
	}

	// transform a vector
	inline Vector3 Matrix3::transformPoint(const Vector3 &p) const
	{
		Vector3 result;
		result.x = p.x * m[0][0] + p.y * m[1][0] + p.z * m[2][0];
		result.y = p.x * m[0][1] + p.y * m[1][1] + p.z * m[2][1];
		result.z = p.x * m[0][2] + p.y * m[1][2] + p.z * m[2][2];

		return result;
	}

	inline Vector3 Matrix3::operator*(const Vector3 &p) const
	{
		return transformPoint(p);
	}

	// function
	inline void Matrix3::setIdentity()
	{
		m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;
		m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;
		m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;
	}

	inline void Matrix3::fromQuaternion(const Quaternion &q)
	{
		m[0][0] = 1.0 - 2.0 * q.y * q.y - 2.0 * q.z * q.z;
		m[0][1] = 2.0 * q.x * q.y + 2.0 * q.w * q.z;
		m[0][2] = 2.0 * q.x * q.z - 2.0 * q.w * q.y;

		m[1][0] = 2.0 * q.x * q.y - 2.0 * q.w * q.z;
		m[1][1] = 1.0 - 2.0 * q.x * q.x - 2.0 * q.z * q.z;
		m[1][2] = 2.0 * q.y * q.z + 2.0 * q.w * q.x;

		m[2][0] = 2.0 * q.x * q.z + 2.0 * q.w * q.y;
		m[2][1] = 2.0 * q.y * q.z - 2.0 * q.w * q.x;
		m[2][2] = 1.0 - 2.0 * q.x * q.x - 2.0 * q.y * q.y;
	}

	inline void Matrix3::fromAnglesScales(const Angles &angles, float scale)
	{
		fromAngles(angles);
		m[0] *= scale;
		m[1] *= scale;
		m[2] *= scale;
	}


	inline Matrix3 Matrix3::getInverse() const
	{
		Matrix3 result;

		float b00,b01,b02,b10,b11,b12,b20,b21,b22;

		b00 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
		b01 = m[0][2] * m[2][1] - m[0][1] * m[2][2];
		b02 = m[0][1] * m[1][2] - m[0][2] * m[1][1];
		b10 = m[1][2] * m[2][0] - m[1][0] * m[2][2];
		b11 = m[0][0] * m[2][2] - m[0][2] * m[2][0];
		b12 = m[0][2] * m[1][0] - m[0][0] * m[1][2];
		b20 = m[1][0] * m[2][1] - m[1][1] * m[2][0];
		b21 = m[0][1] * m[2][0] - m[0][0] * m[2][1];
		b22 = m[0][0] * m[1][1] - m[0][1] * m[1][0];

		float d = b00 * m[0][0] + b01*m[1][0] + b02 * m[2][0];

		if (d == 0.0f) {		//singular?
			result.setIdentity();
			return result;
		}

		d = 1.0f / d;

		//only do assignment at the end, in case dest == this:
		result.m[0][0] = b00*d; result.m[0][1] = b01*d; result.m[0][2] = b02*d;
		result.m[1][0] = b10*d; result.m[1][1] = b11*d; result.m[1][2] = b12*d;
		result.m[2][0] = b20*d; result.m[2][1] = b21*d; result.m[2][2] = b22*d;

		return result;
	}

	inline Matrix3 Matrix3::getTranspose() const
	{
		Matrix3 result;

		result.m[0][0] = m[0][0]; result.m[0][1] = m[1][0]; result.m[0][2] = m[2][0];
		result.m[1][0] = m[0][1]; result.m[1][1] = m[1][1]; result.m[1][2] = m[2][1];
		result.m[2][0] = m[0][2]; result.m[2][1] = m[1][2]; result.m[2][2] = m[2][2];

		return result;
	}

	inline Matrix3 Matrix3::scale(float s) const
	{
		return Matrix3(m[0] * s, m[1] * s, m[2] * s);
	}

	// remove scale transform
	inline float Matrix3::removeScale()
	{
		m[0].normalize();
		m[1].normalize();
		return m[2].normalize();
	}

	// remove shear transform
	inline void Matrix3::removeShear()
	{
		removeScale();

		m[2] = m[0] ^ m[1];
		m[1] = m[2] ^ m[0];
	}


	inline Matrix3::operator const float*() const
	{
		return (float*)this;
	}

	inline const float *Matrix3::toFloatPointer() const
	{
		return (const float*)this;
	}



AX_END_NAMESPACE

#endif // end guardian


/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_MATRIX4_H
#define AX_CORE_MATRIX4_H

AX_BEGIN_NAMESPACE

	//------------------------------------------------------------------------------
	// Matrix4
	//
	//	A 4x4 matrix, it is totally identical to OpenGL matrix definition and memory
	//	stored as same with OpenGL matrix, stored in column-major order. That is, the
	//	matrix is stored as follows
	//		0 4 8 12
	//		1 5 9 13
	//		2 6 10 14
	//		3 7 11 15
	//------------------------------------------------------------------------------

	struct AX_API Matrix4 {
		static const Matrix4 Identity;

		Vector4 m[4];

		// ctor and dtor
		Matrix4();
		Matrix4(float e0, float e1, float e2, float e3, float e4, float e5, float e6, float e7, float e8, float e9, float e10, float e11, float e12, float e13, float e14, float e15);
		Matrix4(const float im[16]);
		Matrix4(const Vector4 &px, const Vector4 &py, Vector4 &pz, Vector4 pw);
		Matrix4(const Vector4 &px, const Vector4 &py, Vector4 &pz);
		Matrix4(const Matrix3 &axis, const Vector3 &origin);
		~Matrix4();

		// member
		Matrix4 &setIdentity();
		Matrix4 &setOrtho(float left, float right, float bottom, float top, float znear, float zfar);
		Matrix4 &setTranslate(float x, float y, float z);
		Matrix4 &setRotation(float x, float y, float z);
		Matrix4 &setScale(float x, float y, float z);
		Matrix4 &setFrustum(float left, float right, float bottom, float top, float znear, float zfar);
		Matrix4 &setFrustumDX(float left, float right, float bottom, float top, float znear, float zfar);
		Matrix4 &setReflect(const Plane &plane);
		Matrix4 &translate(float x, float y, float z);
		Matrix4 &rotate(float x, float y, float z);
		Matrix4 &scale(float x, float y, float z);

		// transform a plane
		Plane transformPlane(const Plane &p) const;

		// transform a vector3, no w divided
		Vector3 fastTransform(const Vector3 &p) const;

		// transform a vector4
		Vector4 transformPoint(const Vector4 &p) const;

		// transform a point
		Vector3 transformPoint(const Vector3 &v) const;

		// transform a normal
		Vector3 transformNormal(const Vector3 &v) const;

		Matrix4 getTranspose() const;
		AffineMat getAffineMat() const;

		float getDeterminant() const;
		Matrix4 getInverse() const;
		void mirror(const Vector4 &plane);
		void fromAxis(const Matrix3 &axis, const Vector3 &origin);
		void toAxis(Matrix3 &axis, Vector3 &origin) const;
		Vector3 getTranslate() const;
		void removeTranslate();
		Matrix4 getRotateMatrix();
		void fromAxisInverse(const Matrix3 &axis, const Vector3 &origin);
		void fromQuaternion(const Quaternion &q);
		static Matrix4 getIdentity();

		//zzx add
		void fromRotate(const Vector3 &pos,const Vector3 &axis,float angles,float radius,bool isFixupStartRotDir = true);

		inline void extract3x3Matrix(Matrix3 &m3x3) const
		{
			m3x3.m[0][0] = m[0][0];
			m3x3.m[0][1] = m[0][1];
			m3x3.m[0][2] = m[0][2];
			m3x3.m[1][0] = m[1][0];
			m3x3.m[1][1] = m[1][1];
			m3x3.m[1][2] = m[1][2];
			m3x3.m[2][0] = m[2][0];
			m3x3.m[2][1] = m[2][1];
			m3x3.m[2][2] = m[2][2];

		}

		// operator
		Matrix4 operator*(const Matrix4 &im) const;
		void operator*=(const Matrix4 im);
		bool operator==(Matrix4 &other) const;
		bool operator!=(Matrix4 &other) const;
		Vector4 &operator[](int index);
		const Vector4 &operator[](int index) const;
		Vector4 operator*(const Vector4 &p) const;
		Vector3 operator*(const Vector3 &p) const;
		const float *toFloatPointer() const;
		operator const float*() const;
	};

	inline Matrix4::Matrix4() {}

	inline Matrix4::Matrix4(float e0, float e1, float e2, float e3,
		float e4, float e5, float e6, float e7,
		float e8, float e9, float e10, float e11,
		float e12, float e13, float e14, float e15)
	{
		m[0][0] = e0;  m[0][1] = e1;   m[0][2] = e2;   m[0][3] = e3;
		m[1][0] = e4;  m[1][1] = e5;   m[1][2] = e6;   m[1][3] = e7;
		m[2][0] = e8;  m[2][1] = e9;   m[2][2] = e10;  m[2][3] = e11;
		m[3][0] = e12; m[3][1] = e13;  m[3][2] = e14;  m[3][3] = e15;
	}

	inline Matrix4::Matrix4(const float im[16]) {
		memcpy(m, im, sizeof(float)*16);
	}

	inline Matrix4::Matrix4(const Vector4 &px, const Vector4 &py, Vector4 &pz, Vector4 pw)
	{
		m[0][0] = px.x; m[0][1] = px.y;  m[0][2] = px.z;  m[0][3] = px.w;
		m[1][0] = py.x; m[1][1] = py.y;  m[1][2] = py.z;  m[1][3] = py.w;
		m[2][0] = pz.x; m[2][1] = pz.y;  m[2][2] = pz.z;  m[2][3] = pz.w;
		m[3][0] = pw.x; m[3][1] = pw.y;  m[3][2] = pw.z;  m[3][3] = pw.w;
	}

	inline Matrix4::Matrix4(const Vector4 &px, const Vector4 &py, Vector4 &pz)
	{
		Vector4 pw(0.f, 0.f, 0.f, 1.f);
		Matrix4(px, py, pz, pw);
	}

	inline Matrix4::Matrix4(const Matrix3 &axis, const Vector3 &origin)
	{
		fromAxis(axis, origin);
	}

	inline Matrix4::~Matrix4() {}

	// member
	inline Matrix4 &Matrix4::setIdentity()
	{
		m[0][0] = 1; m[0][1] = 0;  m[0][2] = 0;  m[0][3] = 0;
		m[1][0] = 0; m[1][1] = 1;  m[1][2] = 0;  m[1][3] = 0;
		m[2][0] = 0; m[2][1] = 0;  m[2][2] = 1;  m[2][3] = 0;
		m[3][0] = 0; m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;

		return *this;
	}

	inline Matrix4 &Matrix4::setOrtho(float left, float right, float bottom, float top, float znear, float zfar)
	{
		// glOrtho(left, right, top, bottom, znear, zfar)
		m[0][0] = 2.f / (right - left);
		m[0][1] = 0.f;
		m[0][2] = 0.f;
		m[0][3] = 0.f;
		m[1][0] = 0.f;
		m[1][1] = 2.f / (top - bottom);
		m[1][2] = 0.f;
		m[1][3] = 0.f;
		m[2][0] = 0.f;
		m[2][1] = 0.f;
		m[2][2] = -2.f / (zfar - znear);
		m[2][3] = 0.f;
		m[3][0] = -(right + left) / (right - left);
		m[3][1] = -(top + bottom) / (top - bottom);
		m[3][2] = -(zfar + znear) / (zfar - znear);
		m[3][3] = 1.f;	// !!!IMPORTANT: OpenGL document in MSDN said this should be -1, it is wrong

		return *this;
	}

	inline Matrix4 &Matrix4::setTranslate(float x, float y, float z)
	{
		m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
		m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
		m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
		m[3][0] = x; m[3][1] = y; m[3][2] = z; m[3][3] = 1;

		return *this;
	}

	inline Matrix4 &Matrix4::translate(float x, float y, float z)
	{
		Matrix4 temp;
		temp.setTranslate(x, y, z);

		*this = temp * (*this);

		return *this;
	}

	inline Matrix4 &Matrix4::setRotation(float x, float y, float z)
	{
		//       |  CE       BDE+AF  -ADE+BF  0 |
		//  M  = | -CF      -BDF+AE   ADF+BE  0 |
		//       |  D       -BC       AC      0 |
		//       |  0        0        0       1 |
		//
		// where A,B are the cosine and sine of the X-axis rotation axis,
		//	     C,D are the cosine and sine of the Y-axis rotation axis,
		//	     E,F are the cosine and sine of the Z-axis rotation axis.
		float A = cos(x*AX_D2R);
		float B = sin(x*AX_D2R);
		float C = cos(y*AX_D2R);
		float D = sin(y*AX_D2R);
		float E = cos(z*AX_D2R);
		float F = sin(z*AX_D2R);
		m[0][0] =  C*E;			m[0][1] =  B*D*E+A*F;	m[0][2] = -A*D*E+B*F;	m[0][3] = 0;
		m[1][0] = -C*F;			m[1][1] = -B*D*F+A*E;	m[1][2] =  A*D*F+B*E;	m[1][3] = 0;
		m[2][0] =  D;			m[2][1] = -B*C;			m[2][2] =  A*C;			m[2][3] = 0;
		m[3][0] =  0;			m[3][1] =  0;			m[3][2] =  0;			m[3][3] = 1;
		return *this;
	}

	inline Matrix4 &Matrix4::rotate(float x, float y, float z)
	{
		Matrix4 temp;
		temp.setRotation(x, y, z);

		*this = temp * (*this);

		return *this;
	}

	inline Matrix4 &Matrix4::setScale(float x, float y, float z)
	{
		m[0][0] = x;   m[0][1] = 0.f; m[0][2] = 0.f; m[0][3] = 0.f;
		m[1][0] = 0.f; m[1][1] = y;   m[1][2] = 0.f; m[1][3] = 0.f;
		m[2][0] = 0.f; m[2][1] = 0.f; m[2][2] = z;   m[2][3] = 0.f;
		m[3][0] = 0.f; m[3][1] = 0.f; m[3][2] = 0.f; m[3][3] = 1.f;

		return *this;
	}

	inline Matrix4 &Matrix4::scale(float x, float y, float z)
	{
		Matrix4 t;
		t.setScale(x, y, z);
		*this = t * (*this);

		return *this;
	}

	inline Matrix4 &Matrix4::setFrustum(float left, float right, float bottom, float top, float znear, float zfar)
	{
		double one_deltax;
		double one_deltay;
		double one_deltaz;
		double doubleznear;

		doubleznear = 2. * znear;
		one_deltax = 1. / (right - left);
		one_deltay = 1. / (top - bottom);
		one_deltaz = 1. / (zfar - znear);

		// glFrusturm(xmax, xmin, ymax, ymin, znear, zfar);
		// above function call will generate below projection matrix

		m[0][0] = (float)(doubleznear * one_deltax);
		m[0][1] = 0.f;
		m[0][2] = 0.f;
		m[0][3] = 0.f;
		m[1][0] = 0.f;
		m[1][1] = (float)(doubleznear * one_deltay);
		m[1][2] = 0.f;
		m[1][3] = 0.f;
		m[2][0] = (float)((right + left) * one_deltax);
		m[2][1] = (float)((top + bottom) * one_deltay);
		m[2][2] = (float)(-(zfar + znear) * one_deltaz);
		m[2][3] = -1.f;
		m[3][0] = 0.f;
		m[3][1] = 0.f;
		m[3][2] = (float)(-(zfar * doubleznear) * one_deltaz);
		m[3][3] = 0.f;

		return *this;
	}

	inline Matrix4 &Matrix4::setFrustumDX(float left, float right, float bottom, float top, float znear, float zfar)
	{
		// set Perspective matrix for DX
		// like D3DXMatrixPerspectiveOffCenterRH
		double one_deltax;
		double one_deltay;
		double one_deltaz;
		double doubleznear;

		doubleznear = 2. * znear;
		one_deltax = 1. / (right - left);
		one_deltay = 1. / (top - bottom);
		one_deltaz = 1. / (zfar - znear);

		m[0][0] = (float)(doubleznear * one_deltax);
		m[0][1] = 0.f;
		m[0][2] = 0.f;
		m[0][3] = 0.f;
		m[1][0] = 0.f;
		m[1][1] = (float)(doubleznear * one_deltay);
		m[1][2] = 0.f;
		m[1][3] = 0.f;
		m[2][0] = (float)((right + left) * one_deltax);
		m[2][1] = (float)((top + bottom) * one_deltay);
		m[2][2] = (float)(-zfar * one_deltaz);
		m[2][3] = -1.f;
		m[3][0] = 0.f;
		m[3][1] = 0.f;
		m[3][2] = (float)((zfar * znear) * one_deltaz);
		m[3][3] = 0.f;

		return *this;
	}

	inline Matrix4 &Matrix4::setReflect(const Plane &P)
	{
		// plane's normal must be normalized

		m[0][0] = -2 * P.a * P.a + 1;	m[0][1] = -2 * P.b * P.a;		m[0][2] = -2 * P.c * P.a;		m[0][3] = 0.f;
		m[1][0] = -2 * P.a * P.b;		m[1][1] = -2 * P.b * P.b + 1;   m[1][2] = -2 * P.c * P.b;		m[1][3] = 0.f;
		m[2][0] = -2 * P.a * P.c;		m[2][1] = -2 * P.b * P.c;		m[2][2] = -2 * P.c * P.c + 1;   m[2][3] = 0.f;
		m[3][0] = -2 * P.a * P.d;		m[3][1] = -2 * P.b * P.d;		m[3][2] = -2 * P.c * P.d;		m[3][3] = 1.f;

		return *this;
	}

	// matrix multiply
	inline Matrix4 Matrix4::operator*(const Matrix4 &im) const
	{
		Matrix4 om;
		om[0][0] = m[0][0]*im[0][0] + m[1][0]*im[0][1] + m[2][0]*im[0][2] + m[3][0]*im[0][3];
		om[0][1] = m[0][1]*im[0][0] + m[1][1]*im[0][1] + m[2][1]*im[0][2] + m[3][1]*im[0][3];
		om[0][2] = m[0][2]*im[0][0] + m[1][2]*im[0][1] + m[2][2]*im[0][2] + m[3][2]*im[0][3];
		om[0][3] = m[0][3]*im[0][0] + m[1][3]*im[0][1] + m[2][3]*im[0][2] + m[3][3]*im[0][3];

		om[1][0] = m[0][0]*im[1][0] + m[1][0]*im[1][1] + m[2][0]*im[1][2] + m[3][0]*im[1][3];
		om[1][1] = m[0][1]*im[1][0] + m[1][1]*im[1][1] + m[2][1]*im[1][2] + m[3][1]*im[1][3];
		om[1][2] = m[0][2]*im[1][0] + m[1][2]*im[1][1] + m[2][2]*im[1][2] + m[3][2]*im[1][3];
		om[1][3] = m[0][3]*im[1][0] + m[1][3]*im[1][1] + m[2][3]*im[1][2] + m[3][3]*im[1][3];

		om[2][0] = m[0][0]*im[2][0] + m[1][0]*im[2][1] + m[2][0]*im[2][2] + m[3][0]*im[2][3];
		om[2][1] = m[0][1]*im[2][0] + m[1][1]*im[2][1] + m[2][1]*im[2][2] + m[3][1]*im[2][3];
		om[2][2] = m[0][2]*im[2][0] + m[1][2]*im[2][1] + m[2][2]*im[2][2] + m[3][2]*im[2][3];
		om[2][3] = m[0][3]*im[2][0] + m[1][3]*im[2][1] + m[2][3]*im[2][2] + m[3][3]*im[2][3];

		om[3][0] = m[0][0]*im[3][0] + m[1][0]*im[3][1] + m[2][0]*im[3][2] + m[3][0]*im[3][3];
		om[3][1] = m[0][1]*im[3][0] + m[1][1]*im[3][1] + m[2][1]*im[3][2] + m[3][1]*im[3][3];
		om[3][2] = m[0][2]*im[3][0] + m[1][2]*im[3][1] + m[2][2]*im[3][2] + m[3][2]*im[3][3];
		om[3][3] = m[0][3]*im[3][0] + m[1][3]*im[3][1] + m[2][3]*im[3][2] + m[3][3]*im[3][3];
		return om;
	}

	inline void Matrix4::operator*=(const Matrix4 im)
	{
		*this=this->operator*(im);
	}

	inline bool Matrix4::operator==(Matrix4 &other) const
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (m[i][j] != other.m[i][j])
					return false;

		return true;
	}

	inline bool Matrix4::operator!=(Matrix4 &other) const
	{
		return !(*this == other);
	}

	inline Vector4 &Matrix4::operator[](int index)
	{
		AX_STRICT_ASSERT(index>=0 && index<4);
		return m[index];
	}

	inline const Vector4 &Matrix4::operator[](int index) const
	{
		AX_STRICT_ASSERT(index>=0 && index<4);
		return m[index];
	}

	// transform a plane
	inline Plane Matrix4::transformPlane(const Plane &p) const
	{
		/* Vector4 op;
		Matrix4 m = Inverse();
		op.x = p.x * m.m[0][0] + p.y * m.m[0][1] + p.z * m.m[0][2] + p.w * m.m[0][3];
		op.y = p.x * m.m[1][0] + p.y * m.m[1][1] + p.z * m.m[1][2] + p.w * m.m[1][3];
		op.z = p.x * m.m[2][0] + p.y * m.m[2][1] + p.z * m.m[2][2] + p.w * m.m[2][3];
		op.w = p.x * m.m[3][0] + p.y * m.m[3][1] + p.z * m.m[3][2] + p.w * m.m[3][3];
		op.Normalize();
		return op;
		*/
		Plane dst;
		dst.a = m[0][0] * p.a + m[1][0] * p.b + m[2][0] * p.c;
		dst.b = m[0][1] * p.a + m[1][1] * p.b + m[2][1] * p.c;
		dst.c = m[0][2] * p.a + m[1][2] * p.b + m[2][2] * p.c;
		dst.d = p.d - (m[3][0] * dst.a + m[3][1] * dst.b + m[3][2] * dst.c);
//		dst.xyz().normalize();
		return dst;
	}

	// transform a vector3, no w divided
	inline Vector3 Matrix4::fastTransform(const Vector3 &p) const
	{
		Vector3 op;
		op.x = p.x * m[0][0] + p.y * m[1][0] + p.z * m[2][0] + m[3][0];
		op.y = p.x * m[0][1] + p.y * m[1][1] + p.z * m[2][1] + m[3][1];
		op.z = p.x * m[0][2] + p.y * m[1][2] + p.z * m[2][2] + m[3][2];

		return op;
	}

	// transform a vector4
	inline Vector4 Matrix4::transformPoint(const Vector4 &p) const
	{
		Vector4 op;
		op.x = p.x * m[0][0] + p.y * m[1][0] + p.z * m[2][0] + p.w * m[3][0];
		op.y = p.x * m[0][1] + p.y * m[1][1] + p.z * m[2][1] + p.w * m[3][1];
		op.z = p.x * m[0][2] + p.y * m[1][2] + p.z * m[2][2] + p.w * m[3][2];
		op.w = p.x * m[0][3] + p.y * m[1][3] + p.z * m[2][3] + p.w * m[3][3];

		return op;
	}

	inline Vector4 Matrix4::operator*(const Vector4 &p) const
	{
		return transformPoint(p);
	}

	inline Vector3 Matrix4::operator*(const Vector3 &p) const
	{
		return transformPoint(p);
	}

	// transform a vector
	inline Vector3 Matrix4::transformPoint(const Vector3 &v) const
	{
		Vector4 temp = transformPoint(Vector4(v,1.0f));
		temp /= temp.w;
		return temp.xyz();
	}

	// transform a normal
	inline Vector3 Matrix4::transformNormal(const Vector3 &v) const
	{
		// return TransformPlane(Vector4(v,0.0f));
		Vector3 dst;
		dst.x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0];
		dst.y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1];
		dst.z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2];
		return dst;
	}

	inline Matrix4 Matrix4::getTranspose() const
	{
		Matrix4 om;

		om.m[0][0] = m[0][0]; om.m[0][1] = m[1][0]; om.m[0][2] = m[2][0]; om.m[0][3] = m[3][0];
		om.m[1][0] = m[0][1]; om.m[1][1] = m[1][1]; om.m[1][2] = m[2][1]; om.m[1][3] = m[3][1];
		om.m[2][0] = m[0][2]; om.m[2][1] = m[1][2]; om.m[2][2] = m[2][2]; om.m[2][3] = m[3][2];
		om.m[3][0] = m[0][3]; om.m[3][1] = m[1][3]; om.m[3][2] = m[2][3]; om.m[3][3] = m[3][3];

		return om;
	}

	inline float Matrix4::getDeterminant() const
	{
		return m[0][0] * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
			m[2][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +
			m[3][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2])) -
			m[1][0] * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
			m[2][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
			m[3][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2])) +
			m[2][0] * (m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) -
			m[1][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
			m[3][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2])) -
			m[3][0] * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) -
			m[1][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]) +
			m[2][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));
	}
	inline Matrix4 Matrix4::getInverse() const
	{
		Matrix4 om;

		float det = getDeterminant();

		if (det == 0.0f) {
			om.setIdentity();
			return om;
		}

		float rd = 1.0f / det;

		om.m[0][0] = rd * (	m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
			m[2][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +
			m[3][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]));
		om.m[0][1] = -rd * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
			m[2][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
			m[3][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]));
		om.m[0][2] = rd * (	m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) -
			m[1][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
			m[3][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));
		om.m[0][3] = -rd * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) -
			m[1][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]) +
			m[2][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));

		om.m[1][0] = -rd * (m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
			m[2][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +
			m[3][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]));
		om.m[1][1] = rd * (	m[0][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
			m[2][0] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
			m[3][0] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]));
		om.m[1][2] = -rd * (m[0][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) -
			m[1][0] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
			m[3][0] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));
		om.m[1][3] = rd * (	m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) -
			m[1][0] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]) +
			m[2][0] * (m[0][2] * m[1][3] - m[0][3] * m[1][2]));

		om.m[2][0] = rd * (	m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
			m[2][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) +
			m[3][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]));
		om.m[2][1] = -rd * (m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) -
			m[2][0] * (m[0][1] * m[3][3] - m[0][3] * m[3][1]) +
			m[3][0] * (m[0][1] * m[2][3] - m[0][3] * m[2][1]));
		om.m[2][2] = rd * (	m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) -
			m[1][0] * (m[0][1] * m[3][3] - m[0][3] * m[3][1]) +
			m[3][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]));
		om.m[2][3] = -rd * (m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) -
			m[1][0] * (m[0][1] * m[2][3] - m[0][3] * m[2][1]) +
			m[2][0] * (m[0][1] * m[1][3] - m[0][3] * m[1][1]));

		om.m[3][0] = -rd * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
			m[2][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) +
			m[3][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));
		om.m[3][1] = rd * (	m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) -
			m[2][0] * (m[0][1] * m[3][2] - m[0][2] * m[3][1]) +
			m[3][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]));
		om.m[3][2] = -rd * (m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) -
			m[1][0] * (m[0][1] * m[3][2] - m[0][2] * m[3][1]) +
			m[3][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]));
		om.m[3][3] = rd * (	m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
			m[1][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]) +
			m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]));
		return om;
	}

	inline void Matrix4::mirror(const Vector4 &plane)
	{
		// components
		Vector4 &x = m[0];
		Vector4 &y = m[1];
		Vector4 &z = m[2];
		Vector4 &t = m[3];
		Vector4 n2 = plane * -2;

		// mirror translation
		Vector4 mt = t + n2 * (t | plane);

		// mirror x rotation
		x += t;
		x += n2 * (x | plane);
		x -= mt;

		// mirror y rotation
		y += t;
		y += n2 * (y | plane);
		y -= mt;

		// mirror z rotation
		z += t;
		z += n2 * (z | plane);
		z -= mt;

		// write result
		mt = t;
	}

	inline void Matrix4::fromAxis(const Matrix3 &axis, const Vector3 &origin)
	{
		m[0][0] = axis[0][0];
		m[0][1] = axis[0][1];
		m[0][2] = axis[0][2];
		m[0][3] = 0.f;
		m[1][0] = axis[1][0];
		m[1][1] = axis[1][1];
		m[1][2] = axis[1][2];
		m[1][3] = 0.f;
		m[2][0] = axis[2][0];
		m[2][1] = axis[2][1];
		m[2][2] = axis[2][2];
		m[2][3] = 0.f;
		m[3][0] = origin[0];
		m[3][1] = origin[1];
		m[3][2] = origin[2];
		m[3][3] = 1.f;
	}

	inline void Matrix4::toAxis(Matrix3 &axis, Vector3 &origin) const
	{
		axis.m[0][0] = m[0][0];
		axis.m[0][1] = m[0][1];
		axis.m[0][2] = m[0][2];
		axis.m[1][0] = m[1][0];
		axis.m[1][1] = m[1][1];
		axis.m[1][2] = m[1][2];
		axis.m[2][0] = m[2][0];
		axis.m[2][1] = m[2][1];
		axis.m[2][2] = m[2][2];

		origin[0] = m[3][0];
		origin[1] =	m[3][1];
		origin[2] =	m[3][2];
	}

	inline Vector3 Matrix4::getTranslate() const
	{
		Vector3 t;
		t[0] =  m[3][0];
		t[1] =	m[3][1];
		t[2] =	m[3][2];
		return t;
	}

	inline void Matrix4::removeTranslate()
	{
		m[3][0] = 0;
		m[3][1] = 0;
		m[3][2] = 0;
	}

	inline Matrix4 Matrix4::getRotateMatrix()
	{
		Matrix4 m = *this;
		m[3][0] = 0;
		m[3][1] = 0;
		m[3][2] = 0;
		return m;
	}

	inline void Matrix4::fromAxisInverse(const Matrix3 &axis, const Vector3 &origin)
	{
		m[0][0] = axis[0][0];
		m[0][1] = axis[1][0];
		m[0][2] = axis[2][0];
		m[0][3] = 0.f;
		m[1][0] = axis[0][1];
		m[1][1] = axis[1][1];
		m[1][2] = axis[2][1];
		m[1][3] = 0.f;
		m[2][0] = axis[0][2];
		m[2][1] = axis[1][2];
		m[2][2] = axis[2][2];
		m[2][3] = 0.f;
		m[3][0] = -(origin | axis[0]);
		m[3][1] = -(origin | axis[1]);
		m[3][2] = -(origin | axis[2]);
		m[3][3] = 1.f;
	}

	inline void Matrix4::fromQuaternion(const Quaternion &q)
	{
		float x,y,z,w;
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
		m[0][0] = 1 - (2*y*y + 2*z*z);
		m[0][1] = 2*x*y + 2*z*w;
		m[0][2] = 2*x*z - 2*y*w;
		m[0][3] = 0;
		m[1][0] = 2*x*y - 2*z*w;
		m[1][1] = 1 - (2*x*x + 2*z*z);
		m[1][2] = 2*y*z + 2*x*w;
		m[1][3] = 0;
		m[2][0] = 2*x*z + 2*y*w;
		m[2][1] = 2*y*z - 2*x*w;
		m[2][2] = 1 - (2*x*x + 2*y*y);
		m[2][3] = 0;
		m[3][0] = 0;
		m[3][1] = 0;
		m[3][2] = 0;
		m[3][3] = 1;
	}

	//zzx add
	inline void Matrix4::fromRotate(const Vector3 &pos,const Vector3 &axis,float angles,float radius,bool isFixupStartRotDir /* = true */)
	{
		Quaternion q;
		Vector3 dir;

		if (!isFixupStartRotDir)
		{
			q.fromAxisAngle(axis,180.0f);

			Vector3 inversePos = q * pos;

			dir = pos - inversePos;

			dir.normalize();

			if (dir == Vector3(0.0f,0.0f,0.0f))
				dir = Vector3(0.0f,1.0f,0.0f);
		}
		else
		{
            dir = Vector3(0.0f,1.0f,0.0f);
		}
	
		Vector3 transPos = dir * radius;

		Matrix4 transMatrix1,transMatrix2;

		Vector3 distance = transPos - pos;

		transMatrix1.setTranslate(-distance.x,-distance.y,-distance.z);
		transMatrix2.setTranslate(distance.x,distance.y,distance.z);

		Matrix4 rotateMatrix;

		q.fromAxisAngle(axis,angles);
		rotateMatrix.fromQuaternion(q);
         
		*this = transMatrix1 * rotateMatrix * transMatrix2;
	}



	inline Matrix4::operator const float*() const
	{
		return (float*)this;
	}

	inline const float *Matrix4::toFloatPointer() const
	{
		return (const float*)this;
	}

	inline Matrix4 Matrix4::getIdentity()
	{
		Matrix4 out;

		out.setIdentity();

		return out;
	}
AX_END_NAMESPACE

#endif // end guardian


/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_AFFINEMAT_H
#define AX_AFFINEMAT_H

AX_BEGIN_NAMESPACE

// column major, affine matrix
struct AX_API Matrix3x4
{
public:
	enum { Forward, Left, Up, Origin };
	enum { NumFloat = 12 };

	static const Matrix3x4 Identity;

	Matrix3x4();
	Matrix3x4(const Matrix3 &axis,  const Vector3 &origin);
	Matrix3x4(float f[12]);
	Matrix3x4(float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10, float f11 );
	Matrix3x4(const Angles &angles, const Vector3 &origin);
	Matrix3x4(const Angles &angles, const Vector3 &origin, float scale);
	~Matrix3x4();

	void setIdentity();
	void set(const Matrix3 &axis,  const Vector3 &origin);
	void setInverse(const Matrix3 &axis,  const Vector3 &origin);
	void setInverse();
	void setOrigin(float x, float y, float z);
	void setAxis(const Angles &angles);
	void setAxis(const Angles &angles, float scale);
	Angles getAngles() const;
	float getScales() const;

	void fromMatrix4(const Matrix4 &mat4x4);
	Matrix4 toMatrix4() const;

	Vector3 transformPoint(const Vector3 &point) const;
	Vector3 transformNormal(const Vector3 &normal) const;

	Matrix3x4 multiply(const Matrix3x4 &other) const;
	Matrix3x4 &multiplySelf(const Matrix3x4 &other);
	const float *c_ptr() const;
	float *w_ptr() { return (float *)this; } // writable pointer

	Vector4 getRow(int index) const;

	// matrix init and transform self
	void initTranslate(float x, float y, float z);
	void initRotation(float x, float y, float z);
	void initRotateByAngles(float pitch, float yaw, float roll);
	void initScale(float x, float y, float z);
	void translateSelf(float x, float y, float z);
	void translateSelf(const Vector3 &v) { translateSelf(v.x, v.y, v.z); }
	void rotateSelf(float x, float y, float z);
	void rotateSelfByAngles(float pitch, float yaw, float roll);
	void scaleSelf(float x, float y, float z);

	String toString() const;
	bool fromString(const char *str);

	static Matrix3x4 getIdentity() { Matrix3x4 result; result.setIdentity(); return result; }

	// operator
	bool operator==(const Matrix3x4 &other) const;
	bool operator!=(const Matrix3x4 &other) const;
	Vector3 &operator[](int index);
	const Vector3 &operator[](int index) const;
	Matrix3x4 operator*(const Matrix3x4 &other) const;
	Matrix3x4 &operator*=(const Matrix3x4 &other);
	Vector3 operator*(const Vector3 &point) const;

	Matrix3 axis;
	Vector3 origin;
};

inline Matrix3x4::Matrix3x4()
{}

inline Matrix3x4::Matrix3x4(const Matrix3 &rotation, const Vector3 &org)
{
	axis = rotation;
	origin = org;
}

inline Matrix3x4::Matrix3x4(float f[12])
{
	axis[0].set(f[0], f[1], f[2]);
	axis[1].set(f[3], f[4], f[5]);
	axis[2].set(f[6], f[7], f[8]);
	origin.set(f[9], f[10], f[11]);
}

inline Matrix3x4::Matrix3x4(float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10, float f11)
{
	axis[0].set(f0, f1, f2);
	axis[1].set(f3, f4, f5);
	axis[2].set(f6, f7, f8);
	origin.set(f9, f10, f11);
}

inline Matrix3x4::Matrix3x4(const Angles &angles, const Vector3 &org)
{
	axis.fromAngles(angles);
	origin = org;
}

inline Matrix3x4::Matrix3x4(const Angles &angles, const Vector3 &org, float scale)
{
	axis.fromAnglesScales(angles, scale);
	origin = org;
}

inline Matrix3x4::~Matrix3x4()
{}

inline void Matrix3x4::setIdentity()
{
	axis.setIdentity();
	origin.set(0, 0, 0);
}

inline void Matrix3x4::set(const Matrix3 &rot,  const Vector3 &org)
{
	axis = rot;
	origin = org;
}

inline void Matrix3x4::setInverse(const Matrix3 &rot,  const Vector3 &org)
{
	axis = rot.getTranspose();
	origin = -(axis * org);
}


inline void Matrix3x4::setOrigin(float x, float y, float z)
{
	origin.set(x, y, z);
}

inline void Matrix3x4::setAxis(const Angles &angles)
{
	axis.fromAngles(angles);
}

inline void Matrix3x4::setAxis(const Angles &angles, float scale)
{
	axis.fromAnglesScales(angles, scale);
}

inline Angles Matrix3x4::getAngles() const
{
	return axis.toAngles();
}

inline float Matrix3x4::getScales() const
{
	return axis[0].getLength();
}

inline void Matrix3x4::fromMatrix4(const Matrix4 &mat4x4)
{
	Matrix3 axis;
	Vector3 origin;
        mat4x4.toAxis(axis,origin);
	set(axis,origin);
}

inline Matrix4 Matrix3x4::toMatrix4() const
{
	return Matrix4(axis, origin);
}

inline Vector3 Matrix3x4::transformPoint(const Vector3 &point) const
{
	return axis * point + origin;
}

inline Vector3 Matrix3x4::transformNormal(const Vector3 &normal) const
{
	return transformPoint(normal);
}

inline Matrix3x4 Matrix3x4::multiply(const Matrix3x4 &other) const
{
	Matrix3x4 result;
	result.origin = axis * other.origin + origin;
	result.axis = axis * other.axis;
	return result;
}

inline Matrix3x4 &Matrix3x4::multiplySelf(const Matrix3x4 &other)
{
	*this = multiply(other);
	return *this;
}

inline const float *Matrix3x4::c_ptr() const
{
	return (const float*)this;
}

inline void Matrix3x4::initTranslate(float x, float y, float z)
{
	axis.setIdentity();
	origin.set(x, y, z);
}

inline Vector4 Matrix3x4::getRow(int index) const
{
	AX_STRICT_ASSERT(index>=0 && index<3);
	return Vector4(axis.getRow(index), origin[index]);
}

inline void Matrix3x4::translateSelf(float x, float y, float z)
{
	Matrix3x4 temp;
	temp.initTranslate(x, y, z);

	*this *= temp;
}

inline void Matrix3x4::initRotation(float x, float y, float z)
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
	axis[0][0] =  C*E;			axis[0][1] =  B*D*E+A*F;	axis[0][2] = -A*D*E+B*F;
	axis[1][0] = -C*F;			axis[1][1] = -B*D*F+A*E;	axis[1][2] =  A*D*F+B*E;
	axis[2][0] =  D;			axis[2][1] = -B*C;			axis[2][2] =  A*C;		
	origin.clear();		
}

inline void Matrix3x4::rotateSelf(float x, float y, float z)
{
	Matrix3x4 temp;
	temp.initRotation(x, y, z);

	*this *= temp;
}

inline void Matrix3x4::initRotateByAngles(float pitch, float yaw, float roll)
{
	origin.clear();
	axis.fromAngles(Angles(pitch, yaw, roll));
}

inline void Matrix3x4::rotateSelfByAngles(float pitch, float yaw, float roll)
{
	Matrix3x4 temp;
	temp.initRotateByAngles(pitch, yaw, roll);
	*this *= temp;
}

inline void Matrix3x4::initScale(float x, float y, float z)
{
	axis[0][0] = x;   axis[0][1] = 0.f; axis[0][2] = 0.f;;
	axis[1][0] = 0.f; axis[1][1] = y;   axis[1][2] = 0.f;;
	axis[2][0] = 0.f; axis[2][1] = 0.f; axis[2][2] = z;  ;
	origin.clear();
}

inline void Matrix3x4::scaleSelf(float x, float y, float z)
{
	Matrix3x4 t;
	t.initScale(x, y, z);
	*this *= t;
}

inline bool Matrix3x4::operator==(const Matrix3x4 &other) const
{
	const float *lhs = this->c_ptr();
	const float *rhs = other.c_ptr();

	for (int i = 0; i < 12; i++) {
		if (lhs[i] != rhs[i]) {
			return false;
		}
	}

	return true;
}

inline bool Matrix3x4::operator!=(const Matrix3x4 &other) const
{
	return !this->operator ==(other);
}

inline Vector3 &Matrix3x4::operator[](int index)
{
	AX_STRICT_ASSERT(index >= 0 && index < 4);
	return axis[index];
}

inline const Vector3 &Matrix3x4::operator[](int index) const
{
	AX_STRICT_ASSERT(index >= 0 && index < 4);
	return axis[index];
}

inline Matrix3x4 Matrix3x4::operator*(const Matrix3x4 &other) const
{
	return multiply(other);
}

inline Matrix3x4 &Matrix3x4::operator*=(const Matrix3x4 &other)
{
	return multiplySelf(other);
}


inline Vector3 Matrix3x4::operator*(const Vector3 &point) const
{
	return transformPoint(point);
}


AX_END_NAMESPACE

#endif // end guardian


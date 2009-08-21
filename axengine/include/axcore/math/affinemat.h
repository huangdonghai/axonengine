/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_AFFINEMAT_H
#define AX_AFFINEMAT_H

namespace Axon {

	// column major, affine matrix
	struct AX_API AffineMat {
	public:
		static const AffineMat Identity;

		AffineMat();
		AffineMat(const Matrix3& axis,  const Vector3& origin);
		AffineMat(float f[12]);
		AffineMat(float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10, float f11 );
		AffineMat(const Angles& angles, const Vector3& origin);
		AffineMat(const Angles& angles, const Vector3& origin, float scale);
		~AffineMat();

		void setIdentity();
		void set(const Matrix3& axis,  const Vector3& origin);
		void setInverse(const Matrix3& axis,  const Vector3& origin);
		void setInverse();
		void setOrigin(float x, float y, float z);
		void setAxis(const Angles& angles);
		void setAxis(const Angles& angles, float scale);
		Angles getAngles() const;
		float getScales() const;

		void fromMatrix4(const Matrix4& mat4x4);
		Matrix4 toMatrix4() const;

		Vector3 transformPoint(const Vector3& point) const;
		Vector3 transformNormal(const Vector3& normal) const;

		AffineMat multiply(const AffineMat& other) const;
		AffineMat& multiplySelf(const AffineMat& other);
		const float* toFloatPointer() const;
		float* toFloatPointer();

		Vector4 getRow(int index) const;

		// matrix init and transform self
		void initTranslate(float x, float y, float z);
		void initRotation(float x, float y, float z);
		void initRotateByAngles(float pitch, float yaw, float roll);
		void initScale(float x, float y, float z);
		void translateSelf(float x, float y, float z);
		void translateSelf(const Vector3& v) { translateSelf(v.x, v.y, v.z); }
		void rotateSelf(float x, float y, float z);
		void rotateSelfByAngles(float pitch, float yaw, float roll);
		void scaleSelf(float x, float y, float z);

		String toString() const;
		bool fromString(const char* str);

		static AffineMat getIdentity() { AffineMat result; result.setIdentity(); return result; }


		// operator
		bool operator==(const AffineMat& other) const;
		bool operator!=(const AffineMat& other) const;
		Vector3& operator[](int index);
		const Vector3& operator[](int index) const;
		AffineMat operator*(const AffineMat& other) const;
		AffineMat& operator*=(const AffineMat& other);
		Vector3 operator*(const Vector3& point) const;

		Matrix3 axis;
		Vector3 origin;
	};

	inline AffineMat::AffineMat() {}

	inline AffineMat::AffineMat(const Matrix3& rotation, const Vector3& org) {
		axis = rotation;
		origin = org;
	}

	inline AffineMat::AffineMat(float f[12]) {
		axis[0].set(f[0], f[1], f[2]);
		axis[1].set(f[3], f[4], f[5]);
		axis[2].set(f[6], f[7], f[8]);
		origin.set(f[9], f[10], f[11]);
	}

	inline AffineMat::AffineMat(float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10, float f11) {
		axis[0].set(f0, f1, f2);
		axis[1].set(f3, f4, f5);
		axis[2].set(f6, f7, f8);
		origin.set(f9, f10, f11);
	}

	inline AffineMat::AffineMat(const Angles& angles, const Vector3& org) {
		axis.fromAngles(angles);
		origin = org;
	}

	inline AffineMat::AffineMat(const Angles& angles, const Vector3& org, float scale) {
		axis.fromAnglesScales(angles, scale);
		origin = org;
	}

	inline AffineMat::~AffineMat() {}

	inline void AffineMat::setIdentity() {
		axis.setIdentity();
		origin.set(0, 0, 0);
	}

	inline void AffineMat::set(const Matrix3& rot,  const Vector3& org) {
		axis = rot;
		origin = org;
	}

	inline void AffineMat::setInverse(const Matrix3& rot,  const Vector3& org) {
		axis = rot.getTranspose();
		origin = -(axis * org);
	}


	inline void AffineMat::setOrigin(float x, float y, float z) {
		origin.set(x, y, z);
	}

	inline void AffineMat::setAxis(const Angles& angles) {
		axis.fromAngles(angles);
	}

	inline void AffineMat::setAxis(const Angles& angles, float scale) {
		axis.fromAnglesScales(angles, scale);
	}

	inline Angles AffineMat::getAngles() const {
		return axis.toAngles();
	}

	inline float AffineMat::getScales() const {
		return axis[0].getLength();
	}

	inline void AffineMat::fromMatrix4(const Matrix4& mat4x4){
		Matrix3 axis;
		Vector3 origin;
        mat4x4.toAxis(axis,origin);
		set(axis,origin);
	}

	inline Matrix4 AffineMat::toMatrix4() const {
		return Matrix4(axis, origin);
	}

	inline Vector3 AffineMat::transformPoint(const Vector3& point) const {
		return axis * point + origin;
	}

	inline Vector3 AffineMat::transformNormal(const Vector3& normal) const {
		return transformPoint(normal);
	}

	inline AffineMat AffineMat::multiply(const AffineMat& other) const {
		AffineMat result;
		result.origin = axis * other.origin + origin;
		result.axis = axis * other.axis;
		return result;
	}

	inline AffineMat& AffineMat::multiplySelf(const AffineMat& other) {
		*this = multiply(other);
		return *this;
	}

	inline const float* AffineMat::toFloatPointer() const {
		return (const float*)this;
	}

	inline float* AffineMat::toFloatPointer() {
		return (float*)this;
	}

	inline void AffineMat::initTranslate(float x, float y, float z) {
		axis.setIdentity();
		origin.set(x, y, z);
	}

	inline Vector4 AffineMat::getRow(int index) const {
		AX_STRICT_ASSERT(index>=0 && index<3);
		return Vector4(axis.getRow(index), origin[index]);
	}

	inline void AffineMat::translateSelf(float x, float y, float z) {
		AffineMat temp;
		temp.initTranslate(x, y, z);

		*this *= temp;
	}

	inline void AffineMat::initRotation(float x, float y, float z) {
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

	inline void AffineMat::rotateSelf(float x, float y, float z) {
		AffineMat temp;
		temp.initRotation(x, y, z);

		*this *= temp;
	}

	inline void AffineMat::initRotateByAngles(float pitch, float yaw, float roll) {
		origin.clear();
		axis.fromAngles(Angles(pitch, yaw, roll));
	}

	inline void AffineMat::rotateSelfByAngles(float pitch, float yaw, float roll) {
		AffineMat temp;
		temp.initRotateByAngles(pitch, yaw, roll);
		*this *= temp;
	}

	inline void AffineMat::initScale(float x, float y, float z) {
		axis[0][0] = x;   axis[0][1] = 0.f; axis[0][2] = 0.f;;
		axis[1][0] = 0.f; axis[1][1] = y;   axis[1][2] = 0.f;;
		axis[2][0] = 0.f; axis[2][1] = 0.f; axis[2][2] = z;  ;
		origin.clear();
	}

	inline void AffineMat::scaleSelf(float x, float y, float z) {
		AffineMat t;
		t.initScale(x, y, z);
		*this *= t;
	}

	inline bool AffineMat::operator==(const AffineMat& other) const {
		const float* lhs = this->toFloatPointer();
		const float* rhs = other.toFloatPointer();

		for (int i = 0; i < 12; i++) {
			if (lhs[i] != rhs[i]) {
				return false;
			}
		}

		return true;
	}

	inline bool AffineMat::operator!=(const AffineMat& other) const {
		return !this->operator ==(other);
	}

	inline Vector3& AffineMat::operator[](int index) {
		AX_STRICT_ASSERT(index >= 0 && index < 4);
		return axis[index];
	}

	inline const Vector3& AffineMat::operator[](int index) const {
		AX_STRICT_ASSERT(index >= 0 && index < 4);
		return axis[index];
	}

	inline AffineMat AffineMat::operator*(const AffineMat& other) const {
		return multiply(other);
	}

	inline AffineMat& AffineMat::operator*=(const AffineMat& other) {
		return multiplySelf(other);
	}


	inline Vector3 AffineMat::operator*(const Vector3& point) const {
		return transformPoint(point);
	}


} // namespace Axon

#endif // end guardian


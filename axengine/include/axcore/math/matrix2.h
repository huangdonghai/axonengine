/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_MATRIX2_H
#define AX_CORE_MATRIX2_H

AX_BEGIN_NAMESPACE

	//--------------------------------------------------------------------------
	// Matrix2
	//--------------------------------------------------------------------------

	struct AX_API Matrix2 {
		float m[2][2];

		inline Matrix2() {}
		inline Matrix2(const float in[4]) {
			memcpy(m, in, sizeof(m));
		}
		inline ~Matrix2() {}

		// operator
		inline Vector2& operator[] (int index) {
			AX_STRICT_ASSERT(index>=0 && index<2);
			return *(Vector2*)m[index];
		}

		inline const Vector2& operator[] (int index) const {
			AX_STRICT_ASSERT(index>=0 && index<2);
			return *(Vector2*)m[index];
		}

		inline Matrix2 operator*(const Matrix2& im) const {
			Matrix2 om;

			om[0][0] = m[0][0]*im[0][0] + m[1][0]*im[0][1];
			om[0][1] = m[0][1]*im[0][0] + m[1][1]*im[0][1];

			om[1][0] = m[0][0]*im[1][0] + m[1][0]*im[1][1];
			om[1][1] = m[0][1]*im[1][0] + m[1][1]*im[1][1];

			return om;
		}

		// function
		inline void setIdentity() {
			m[0][0] = 1;	m[0][1] = 0;
			m[1][0] = 0;	m[1][1] = 1;
		}
	};


AX_END_NAMESPACE

#endif // end guardian


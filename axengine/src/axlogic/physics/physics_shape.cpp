/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

AX_BEGIN_NAMESPACE

	PhysicsShape::PhysicsShape() {
	}

	PhysicsShape::~PhysicsShape() {
	}

	class MyHeightFieldShape : public hkpSampledHeightFieldShape {
	public:
		MyHeightFieldShape(const hkpSampledHeightFieldBaseCinfo &ci, const hkUint16 *data)
			:	hkpSampledHeightFieldShape(ci),
			m_data(data)
		{
		}

		// Generate a rough terrain
		HK_FORCE_INLINE hkReal getHeightAtImpl(int x, int z) const {
			// Lookup data and return a float
			// We scale the data artifically by 5 to make it look interesting
			return hkReal(m_data[x * m_zRes + z]) / hkReal(hkUint16(-1)) * 2048.0f - 1024.0f;
		}

		//	This should return true if the two triangles share the edge p00-p11
		// otherwise it should return false if the triangles share the edge p01-p10
		HK_FORCE_INLINE hkBool getTriangleFlipImpl() const {	
			return false;
		}

		virtual void collideSpheres(const CollideSpheresInput &input, SphereCollisionOutput *outputArray) const {
			hkSampledHeightFieldShape_collideSpheres(*this, input, outputArray);
		}

	private:
		const hkUint16 *m_data;
	};


	Heightfield::Heightfield(const ushort_t *data, int size, float tilemeters) {
		hkpSampledHeightFieldBaseCinfo ci;
		ci.m_xRes = size;
		ci.m_zRes = size;
		ci.m_scale.set(tilemeters, 1, tilemeters);
		ci.m_minHeight = -1024.0f;
		ci.m_maxHeight = 1024.0f;

		m_rep = new MyHeightFieldShape(ci, data);
	}

	Heightfield::~Heightfield() {
		m_rep->removeReference();
	}

AX_END_NAMESPACE


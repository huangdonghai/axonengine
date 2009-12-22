/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_PHYSICS_Util_H
#define AX_PHYSICS_Util_H

AX_BEGIN_NAMESPACE

	// cast entity pointer to number
	inline hkUlong e2n(PhysicsEntity* ent) {
		return hkUlong(ent);
	}

	// cast number to entity pointer
	inline PhysicsEntity* n2e(hkUlong ul) {
		return (PhysicsEntity*)ul;
	}

	inline void h2x(const hkVector4& hk, Vector3& ax) {
		hk.store3(&ax[0]);
	}

	inline Vector3 h2x(const hkVector4& h) {
		return *(Vector3*)&h;
	}

	inline void x2h(const Vector3& ax, hkVector4& hk) {
		hk.set(ax.x, ax.y, ax.z);
	}

	inline hkVector4 x2h(const Vector3& ax) {
		hkVector4 hk;
		x2h(ax, hk);
		return hk;
	}

	inline void h2x(const hkRotation& hk, Matrix3& ax) {
		h2x(hk.getColumn(0), ax[0]);
		h2x(hk.getColumn(1), ax[1]);
		h2x(hk.getColumn(2), ax[2]);
	}

	inline void x2h(const Matrix3& ax, hkRotation& hk) {
		hk.setCols(x2h(ax[0]), x2h(ax[1]), x2h(ax[2]));
	}

	inline hkRotation x2h(const Matrix3& ax) {
		hkRotation hk;
		x2h(ax, hk);
		return hk;
	}

	inline void h2x(const hkTransform& hk, AffineMat& ax) {
		h2x(hk.getTranslation(), ax.origin);
		h2x(hk.getRotation(), ax.axis);
	}

	inline AffineMat h2x(const hkTransform& hk) {
		AffineMat ax;
		h2x(hk, ax);
		return ax;
	}

	inline void x2h(const AffineMat& ax, hkTransform& hk) {
		hk.setTranslation(x2h(ax.origin));
		hk.setRotation(x2h(ax.axis));
	}

	inline hkTransform x2h(const AffineMat& ax) {
		hkTransform hk;
		x2h(ax, hk);
		return hk;
	}

	inline AffineMat h2x(const hkMatrix4& hk) {
		AffineMat ax;
		h2x(hk.getColumn(0), ax.axis[0]);
		h2x(hk.getColumn(1), ax.axis[1]);
		h2x(hk.getColumn(2), ax.axis[2]);
		h2x(hk.getColumn(3), ax.origin);
		return ax;
	}


	inline PhysicsEntity::MotionType h2x(hkpMotion::MotionType hk) {
		switch (hk) {
			default:
			case hkpMotion::MOTION_INVALID:
				return PhysicsEntity::Motion_Invalid;

			case hkpMotion::MOTION_DYNAMIC:
			case hkpMotion::MOTION_SPHERE_INERTIA:
			case hkpMotion::MOTION_STABILIZED_SPHERE_INERTIA:
			case hkpMotion::MOTION_BOX_INERTIA:
			case hkpMotion::MOTION_STABILIZED_BOX_INERTIA:
			case hkpMotion::MOTION_THIN_BOX_INERTIA:
				return PhysicsEntity::Motion_Dynamic;

			case hkpMotion::MOTION_KEYFRAMED:
				return PhysicsEntity::Motion_Keyframed;
			case hkpMotion::MOTION_FIXED:
				return PhysicsEntity::Motion_Fixed;
			case hkpMotion::MOTION_CHARACTER:
				return PhysicsEntity::Motion_Dynamic;
		}
	}

	inline hkpMotion::MotionType x2h(PhysicsEntity::MotionType ax) {
		switch (ax) {
			default:
			case PhysicsEntity::Motion_Invalid:
				return hkpMotion::MOTION_INVALID;
			case PhysicsEntity::Motion_Fixed:
				return hkpMotion::MOTION_FIXED;
			case PhysicsEntity::Motion_Dynamic:
				return hkpMotion::MOTION_DYNAMIC;
			case PhysicsEntity::Motion_Keyframed:
				return hkpMotion::MOTION_KEYFRAMED;
		}
	}

	inline String h2x(hkxMaterial* mat) {
		if (!mat) {
			return String();
		}

		if (!mat->m_numStages) {
			return String();
		}


		if (mat->m_stages->m_texture.m_class != &hkxTextureFileClass) {
			return String();
		}

		hkxTextureFile* texfile = (hkxTextureFile*)mat->m_stages->m_texture.m_object;
		if (!texfile) {
			return String();
		}

		String result = PathUtil::getRelativePath(l2u(texfile->m_filename));
		return PathUtil::removeExt(result);
	}

	inline void h2x(const hkAabb& aabb, BoundingBox& bbox) {
		h2x(aabb.m_min, bbox.min);
		h2x(aabb.m_max, bbox.max);
	}

	struct Util {
		static inline hkaRagdollInstance* clone(hkaRagdollInstance* src) {
			hkArray<hkpRigidBody*> rigidbodies;
			hkArray<hkpConstraintInstance*> constraints;

			Dict<hkpRigidBody*,hkpRigidBody*> rigidbodymap;

			// clone rigidbodies
			for (int i = 0; i < src->getNumBones(); i++) {
				hkpRigidBody* rb = src->getRigidBodyOfBone(i);
				// Initialize with quality type and collision filter
				if (rb != HK_NULL) {
					hkpRigidBody* cloned = rb->clone();
					rigidbodies.pushBack(cloned);
					rigidbodymap[rb] = cloned;
				} else {
//					rigidbodies.pushBack(nullptr);
				}
			}

			// clone constraints
			const hkArray<hkpConstraintInstance*>& src_constraints = src->getConstraintArray();
			for (int i = 0; i < src_constraints.getSize(); i++) {
				hkpConstraintInstance* constraint = src_constraints[i];
				hkpRigidBody* enta = constraint->getRigidBodyA();
				hkpRigidBody* entb = constraint->getRigidBodyB();
				enta = rigidbodymap[enta];
				entb = rigidbodymap[entb];

				AX_ASSERT(enta);
				AX_ASSERT(entb);

				constraint = constraint->clone(enta, entb);
				constraints.pushBack(constraint);
			}

			hkaRagdollInstance* result = new hkaRagdollInstance(rigidbodies, constraints, src->m_skeleton, src->m_boneToRigidBodyMap);
			return result;
		}
	};

AX_END_NAMESPACE

#endif // AX_PHYSICS_INTERNAL_H


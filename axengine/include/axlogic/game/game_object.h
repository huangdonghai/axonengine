/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_GAME_NODE_H
#define AX_GAME_NODE_H

AX_BEGIN_NAMESPACE

	//--------------------------------------------------------------------------
	// class GameObject
	//--------------------------------------------------------------------------

	class AX_API GameObject : public Object
	{
		AX_DECLARE_CLASS(GameObject, Object)
			AX_PROP(outdoorOnly)
			AX_PROP(castShadow)
			AX_PROP(recvShadow)
			AX_PROP(lodRatio)
		AX_END_CLASS()

	public:
		friend class GameWorld;

		enum Mode {
			Editing, Simulating, Gaming
		};

		enum Update {
			MatrixSet = 1,
			Present = 2,
			Visual = 4,
			Animation = 8,
			Sound = 16,
			ReadPhysics = 32,
			SetPhysics = 64
		};
		typedef Flags_<Update>	UpdateFlags;

		GameObject();
		virtual ~GameObject();

		// geometry
		virtual BoundingBox getLocalBoundingBox();
		virtual BoundingBox getBoundingBox();

		// for editor use
		virtual void doHitTest() const;
		virtual void doDebugRender() const;

		// game logic
		virtual void doSpawn();
		virtual void doRemove();

		virtual bool isFixed() const { return false; }

		// read and write
		void writeXml(File *f, int indent=0);
		void readXml(const TiXmlElement *node);

		// matrix
		Vector3 getOrigin_p() const;
		void setOrigin_p(const Vector3 &pos);
		const Matrix3 &getAxis_p() const;
		void setAxis_p(const Matrix3 &axis);
		void setMatrix_p(const Matrix &matrix);
		const Matrix &getMatrix_p() const;
		Vector3 getInstanceColor() const { return m_instanceColor_p; }
		void setInstanceColor(const Vector3 &color);

		// properties
		bool get_outdoorOnly() const { return m_outdoorOnly; }
		void set_outdoorOnly(bool val) { m_outdoorOnly = val; }
		bool get_castShadow() const { return m_castShadow; }
		void set_castShadow(bool val) { m_castShadow = val; }
		bool get_recvShadow() const { return m_recvShadow; }
		void set_recvShadow(bool val) { m_recvShadow = val; }
		float get_lodRatio() const { return m_lodRatio; }
		void set_lodRatio(float val) { m_lodRatio = val; }

	protected:
		void setRenderEntity(RenderEntity *entity) { m_renderEntity = entity; }
		RenderEntity *getRenderEntity() const { return m_renderEntity; };

		void setPhysicsEntity(PhysicsEntity *physics) { m_physicsEntity = physics; }
		PhysicsEntity *getPhysicsEntity() const { return m_physicsEntity; }

		void setSoundEntity(SoundEntity *entity) { m_soundEntity = entity; }
		SoundEntity *getSoundEntity() const { return m_soundEntity; }

		virtual void onMatrixChanged();
		virtual void onPropertyChanged();
		virtual void reload() {};
		virtual void clear() {};

	protected:
		Matrix m_matrix_p;
		Vector3 m_instanceColor_p;
		bool m_matrixDirty;
		bool m_outdoorOnly;
		bool m_castShadow;
		bool m_recvShadow;
		float m_lodRatio;

		UpdateFlags m_updateFlags;
		bool m_spawned;

	private:
		RenderEntity *m_renderEntity;
		PhysicsEntity *m_physicsEntity;
		SoundEntity *m_soundEntity;
	};


AX_END_NAMESPACE

#endif // end guardian


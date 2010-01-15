/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_CAMERA_H
#define AX_RENDER_CAMERA_H

AX_BEGIN_NAMESPACE

class RenderCamera;

class Convex {
public:
	enum {
		MAX_PLANES = 6
	};

	Convex() { m_numPlanes = 0; }

	void initFromCamera(const RenderCamera &camera);
	void initFromCamera(const RenderCamera &shadowCamera, const RenderCamera &visCamera);

	Plane::Side checkBox(const BoundingBox &bbox) const;
	bool cullBox(const BoundingBox &bbox) const;

	int m_numPlanes;
	Plane m_planes[MAX_PLANES];
};


inline Plane::Side Convex::checkBox(const BoundingBox &bbox) const {
	bool cross = false;
	for (int i = 0; i < m_numPlanes; i++) {
		Plane::Side side = m_planes[i].side(bbox);
		if (side == Plane::Back)
			return Plane::Back;

		if (side == Plane::Cross)
			cross = true;
	}

	if (cross)
		return Plane::Cross;

	return Plane::Front;
}

inline bool Convex::cullBox(const BoundingBox &bbox) const {
	return checkBox(bbox) == Plane::Back;
}


//------------------------------------------------------------------------------
// class RenderCamera
//------------------------------------------------------------------------------

class RenderTarget;

class AX_API RenderCamera {
public:
	enum Flag {
		ClearColor = 1
	};

	friend class Convex;

	// method
	RenderCamera();
	~RenderCamera();

	// provide a system window handle
	void setTarget(RenderTarget *target);
	RenderTarget *getTarget() const;

	void setOrigin(const Vector3 &vieworg);
	const Vector3 &getOrigin() const;
	void setViewRect(const Rect &viewrect);
	const Rect &getViewRect() const;

	void setViewAxis(const Matrix3 &axis);
	void setViewAngles(const Angles &angles);
	const Matrix3 &getViewAxis() const;

	void setFov(float fov_x, float fov_y = 0);
	void setFov(float fov_x, float fov_y, float znear, float zfar);
	float getFovX() const;

	bool isOrthoProjection() const;
	float getTop() const;
	float getZnear() const;
	float getZfar() const;

	void setTime(uint_t time);
	uint_t getTime() const;
	int getFrameTime() const { return m_frameTime; }

	void setOrtho(float width, float height, float depth);
	void setOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
	const Vector3 &getOrtho();

	// set overlay view. ortho projection
	// left-top will map to(0, 0) origin, not same with OpenGL
	void setOverlay(float left, float top, float width, float height);
	void setOverlay(const Rect &rect);

	// set perspective overlay view
	void setPersOverlay(const Rect &rect, float fov);

	// flags
	void setFlags(int flags) { m_flags = flags; }
	void addFlag(int flag) { m_flags |= flag; }
	bool isFlagSet(int flag) { return (m_flags & flag) != 0; }

	// clearcolor
	void setClearColor(Rgba color) { m_clearColor = color; addFlag(ClearColor); }
	Rgba getClearColor() { return m_clearColor; }

	// check box intersect frustum
	Plane::Side checkBox(const BoundingBox &bbox) const;
	// if box is in or part in view frustum, return true
	// assume frustum has been calculated
	bool cullBox(const BoundingBox &bbox) const;

	// reflection function
	void enableReflection(const Plane &refl_plane);
	void disableReflection();
	bool isReflectionEnabled();
	const Plane &getReflectionPlane() const { return m_reflectionPlane; }
	Plane getClipSpaceReflectionPlane() const { return m_vp.transformPlane(m_reflectionPlane); }

	// for trapezoidal shadow map
	void enableTrapezodial(const Vector3 &lightdir, float focusdis);

	// screen project and unproject
	Vector3 worldToScreen(const Vector3 &in) const;
	Vector3 screenToWorld(const Vector3 &in) const;

	// create selection view from self
	RenderCamera createSelectionCamera(const Rect &region) const;
	RenderCamera createMirrorCamera(const Plane &mirror_plane) const;

	// matrix and view port
	const Matrix4 &getViewMatrix() const { return m_viewMatrix; }
	const Matrix4 &getProjMatrix() const { return m_projMatrix; }
	const Matrix4 &getViewProjMatrix() const { return m_vp; }
	const Matrix4 &getViewProjNoTranslate() const { return m_vpNoTranslate; }
	const Vector4 &getViewPort() const { return m_viewport; }
	const Vector4 &getViewPortDX() const { return m_viewportDX; }

	// horizon space trapezoidal shadow map for direction(sun) light
	void getTrapezoidalMatrix(Matrix4 &lightMatrix, Matrix4 &tsmMatrix, const Vector3 &lightdir, float focusdistance) const;

	// calculate points along view ray
	void calcPointsAlongZdist(Vector3 result[4], float zdist) const;

	// convert camera z to perspective z
	float calcPerspectiveZ(float cameraZ) const;

	static void createCubemapCameras(const RenderCamera &main, const Matrix3x4 &mtx,  RenderCamera result[6], float znear=0.5f, float zfar=16.0f);
	static void createCubemapCameras(const Matrix3x4 &mtx,  RenderCamera result[6], float znear=0.5f, float zfar=16.0f);

protected:
	//
	// calculate model view projection matrix
	//
	void calcViewMatrix();
	void calcProjectionMatrix();
	void calcViewPort();
	void calcFrustum();
	void adjustProjMatrixForReflection();

private:
	// window handle
	RenderTarget *m_target;

	Rect m_viewRect;
	float m_fovX, m_fovY;
	Vector3 m_viewOrg;
	Matrix3 m_viewAxis;			// transformation matrix
	Rgba m_clearColor;
	bool m_isOrthoProjection;

	// time in milliseconds for material effects and other time dependent rendering issues
	uint_t m_time;
	int m_frameTime;
	int m_flags;				// RVF_ortho ...
	float m_znear;
	float m_zfar;				// z far clip

	// frustum definition
	float m_left, m_right, m_top, m_bottom;

	bool m_isReflection;
	Plane m_reflectionPlane;

	// matrix and viewport
	Matrix4 m_viewMatrix;
	Matrix4 m_projMatrix;
	Vector4 m_viewport;
	Vector4 m_viewportDX;		// viewport for dx. origin at left-upper
	Point m_clientSize;

	// some cached matrix for fast computation
	Matrix4 m_vp;				// view-projection matrix
	Matrix4 m_vpInverse;			// for vertex inverse transform

	Matrix4 m_projNoReflectionAdjust;
	Matrix4 m_vpNoTranslate;				// view-projection matrix

	Convex m_convex;
};

inline float RenderCamera::getTop() const {
	return m_top;
}

inline float RenderCamera::getZnear() const {
	return m_znear;
}

inline float RenderCamera::getZfar() const {
	return m_zfar;
}

inline bool RenderCamera::isOrthoProjection() const {
	return m_isOrthoProjection;
}

AX_END_NAMESPACE

#endif // AX_RENDER_CAMERA_H

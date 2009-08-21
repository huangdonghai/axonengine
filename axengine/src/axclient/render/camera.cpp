/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

namespace Axon { namespace Render {

	Camera::Camera()
		: m_isOrthoProjection(false)
		, m_flags(0)
	{
		m_fovX = 90;
		m_fovY = 0;
		m_isReflection = false;
		m_reflectionPlane.set(0, 0, 1, 0);
		m_reflectionPlane.normal().normalize();
	}

	Camera::~Camera() {
	}


	void Camera::setTarget(Target* target) {
		m_target = target;
		calcViewPort();
	}

	Target* Camera::getTarget() const {
		return m_target;
	}


	void Camera::setOrigin(const Vector3& view_org) {
		if (m_viewOrg == view_org)
			return;

		m_viewOrg = view_org;

		calcViewMatrix();
		calcFrustum();
	}

	const Vector3& Camera::getOrigin() const {
		return m_viewOrg;
	}

	void Camera::setViewRect(const Rect& view_rect) {
		if (m_viewRect == view_rect)
			return;

		m_viewRect = view_rect;
		calcViewPort();
		calcProjectionMatrix();
		calcFrustum();
	}

	const Rect& Camera::getViewRect() const {
		return m_viewRect;
	}


	void Camera::setViewAxis(const Matrix3& axis) {
		if (m_viewAxis == axis)
			return;

		m_viewAxis = axis;
		calcViewMatrix();
		calcFrustum();
	}

	void Camera::setViewAngles(const Angles& angles) {
		m_viewAxis = angles.toMatrix3();
		calcViewMatrix();
		calcFrustum();
	}


	const Matrix3& Camera::getViewAxis() const {
		return m_viewAxis;
	}


	void Camera::setFov(float fov_x, float fov_y) {
		m_fovX = fov_x;
		m_fovY = fov_y;
		m_znear = r_znearMin->getFloat();
		m_zfar = 8192;
		m_isOrthoProjection = false;
		calcProjectionMatrix();
		calcFrustum();
	}

	void Camera::setFov(float fov_x, float fov_y, float znear, float zfar)
	{
		m_fovX = fov_x;
		m_fovY = fov_y;
		m_znear = znear;
		m_zfar = zfar;
		m_isOrthoProjection = false;
		calcProjectionMatrix();
		calcFrustum();
	}

	float Camera::getFovX() const {
		return m_fovX;
	}


	void Camera::setTime(uint_t time) {
		m_frameTime = time - m_time;
		m_time = time;
	}

	uint_t Camera::getTime() const {
		return m_time;
	}


	void Camera::setOrtho(float width, float height, float depth) {
		m_isOrthoProjection = true;

		m_left = -width * 0.5f;
		m_right = -m_left;
		m_top = height * 0.5f;
		m_bottom = -m_top;
		m_znear = -depth * 0.5f;
		m_zfar = depth * 0.5f;

		calcProjectionMatrix();
		calcFrustum();
	}

	void Camera::setOrtho(float left, float right, float bottom, float top, float zNear, float zFar) {
		m_isOrthoProjection = true;
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;
		m_znear = zNear;
		m_zfar = zFar;

		calcProjectionMatrix();
		calcFrustum();
	}


	void Camera::setOverlay(float left, float top, float width, float height) {
		m_viewOrg.x = width * 0.5f;
		m_viewOrg.y = height * 0.5f;
		m_viewOrg.z = 0;

		m_viewAxis[0] = Vector3(0.0f, 0.0f, 1.0f);
		m_viewAxis[1] = Vector3(-1.0f, 0.0f, 0.0f);
		m_viewAxis[2] = Vector3(0.0f, -1.0f, 0.0f);

		m_left = -width * 0.5f;
		m_right = width * 0.5f;
		m_bottom = -height * 0.5f;
		m_top = height * 0.5f;
		m_znear = -1000;
		m_zfar = 1000;

		m_viewRect = Rect(left, top, width, height);

		m_isOrthoProjection = true;

		// recalc matrix
		calcViewMatrix();
		calcViewPort();
		calcProjectionMatrix();
		calcFrustum();
	}

	void Camera::setOverlay(const Rect& rect) {
		setOverlay(rect.x, rect.y, rect.width, rect.height);
	}


	void Camera::setPersOverlay(const Rect& rect, float fov) {
		m_viewOrg.x = rect.width * 0.5f;
		m_viewOrg.y = rect.height * 0.5f;
		m_viewOrg.z = - m_viewOrg.x * tan(fov * 0.5f * AX_D2R);

		m_viewAxis[0] = Vector3(0.0f, 0.0f, 1.0f);
		m_viewAxis[1] = Vector3(-1.0f, 0.0f, 0.0f);
		m_viewAxis[2] = Vector3(0.0f, -1.0f, 0.0f);
		m_fovX = fov;

		m_viewRect = rect;

		m_isOrthoProjection = false;

		// recalc matrix
		calcViewMatrix();
		calcViewPort();
		calcProjectionMatrix();
		calcFrustum();
	}


	Plane::Side Camera::checkBox(const BoundingBox& bbox) const {
		g_statistic->incValue(stat_frustumCullCall);
#if 0

		size_t i;
		bool cross = false;
		for (i = 0; i < ArraySize(m_frustum); i++) {
			Plane::Side side = m_frustum[i].side(bbox);
			if (side == Plane::Back)
				return Plane::Back;

			if (side == Plane::Cross)
				cross = true;
		}

		if (cross)
			return Plane::Cross;

		return Plane::Front;
#else
		return m_convex.checkBox(bbox);
#endif
	}

	bool Camera::cullBox(const BoundingBox& bbox) const {
#if 0
		return checkBox(bbox) == Plane::Back;
#else
		g_statistic->incValue(stat_frustumCullCall);
		return m_convex.cullBox(bbox);
#endif
	}

	Vector3 Camera::worldToScreen(const Vector3& in) const {
		Vector4 out(in);

		out = m_vp * out;

		if (out.w == 0.0f) {
			return out.xyz();
		}

		out /= out.w;

		out.x = m_viewport[0] + (1 + out.x) * m_viewport[2] / 2;
		out.y = m_viewport[1] + (1 + out.y) * m_viewport[3] / 2;
		out.y = m_clientSize.y - out.y;

		return out.xyz();
	}

	Vector3 Camera::screenToWorld(const Vector3& in) const {
		Vector4 vert(in);

		vert.y = m_clientSize.y - vert.y;

		vert.x = (vert.x - m_viewport[0]) * 2 / m_viewport[2] - 1.0;
		vert.y = (vert.y - m_viewport[1]) * 2 / m_viewport[3] - 1.0;
		vert.z = vert.z;
		vert.w = 1.0;

		vert = m_vpInverse * vert;
		if (vert.w == 0.0f)
			return vert.xyz();

		vert /= vert.w;

		return vert.xyz();
	}


	Camera Camera::createSelectionCamera(const Rect& region) const {
		Camera ret = *this;

		// calculate projec matrix
		float cx,cy,width,height,dx,dy,sx,sy,tx,ty;

		width = (float)region.width;
		height = (float)region.height;
		dx = width / 2;
		dy = height / 2;
		cx = (float)region.x + dx;
		cy = (float)(getViewRect().height - region.height - region.y) + dy;

		Vector4 viewport = getViewPort();
		sx = viewport[2] / width;
		sy = viewport[3] / height;
		tx = (viewport[2] + 2.0f * (viewport[0] - cx)) / width;
		ty = (viewport[3] + 2.0f * (viewport[1] - cy)) / height;

		Matrix4 m (
			sx,	0,	0,	0,
			0,	sy,	0,	0,
			0,	0,	1,	0,
			tx,	ty,	0,	1);

		ret.m_projMatrix = m * getProjMatrix();

		// calculate frustum
		ret.calcFrustum();

		return ret;
	}


	void Camera::calcViewMatrix() {
#if 1
		// calculate model view matrix
		static Matrix4 transform(
			0,  0, -1,  0,
			-1,  0,  0,  0,
			0,  1,  0,  0,
			0,  0,  0,  1
			);

		Matrix4 m;

		m.fromAxisInverse(m_viewAxis, m_viewOrg);

		m_viewMatrix = transform * m;

		if (m_isReflection) {
			m.setReflect(m_reflectionPlane);
			m_viewMatrix = m_viewMatrix * m;
		}
#else
		m_viewMatrix.fromAxisInverse(m_viewAxis, m_viewOrg);

		if (m_isReflection) {
			Matrix4 m;
			m.setReflect(m_reflectionPlane);
			m_viewMatrix = m_viewMatrix * m;
		}
#endif
	}

	void Camera::calcProjectionMatrix() {
		static Matrix4 transform(
			0,  0, -1,  0,
			-1,  0,  0,  0,
			0,  1,  0,  0,
			0,  0,  0,  1
			);

		// calculate projection matrix
		if (m_isOrthoProjection) {
#if 0
			m_left = -m_ortho[0] * 0.5f;
			m_right = -m_left;
			m_top = m_ortho[1] * 0.5f;
			m_bottom = -m_top;
			m_znear = -2.f;
			m_zfar = m_ortho[2];
#endif
			m_projMatrix.setOrtho(m_left, m_right, m_bottom, m_top, m_znear, m_zfar);
		} else {
#if 0
			m_znear = r_znearMin->getFloat();
			m_zfar = 8192;
#endif
			if (m_fovX == 0) {
				m_top = m_znear * tanf(m_fovY * AX_D2R * 0.5f);
				m_bottom = -m_top;

				m_right = m_top * m_viewRect.width / m_viewRect.height;
				m_left = - m_right;

				m_fovX = atan2(m_right, m_znear) * AX_R2D * 2.0f;
			} else if (m_fovY == 0) {
				m_right = m_znear * tanf(m_fovX * AX_D2R * 0.5f);
				m_left = -m_right;

				m_top = m_right * m_viewRect.height / m_viewRect.width;
				m_bottom = -m_top;
				m_fovY = atan2(m_top, m_znear) * AX_R2D * 2.0f;
			} else {
				m_right = m_znear * tanf(m_fovX * AX_D2R * 0.5f);
				m_left = -m_right;
				m_top = m_znear * tanf(m_fovY * AX_D2R * 0.5f);
				m_bottom = -m_top;
			}
			m_projMatrix.setFrustum(m_left, m_right, m_bottom, m_top, m_znear, m_zfar);

			m_projNoReflectionAdjust = m_projMatrix;

			if (m_isReflection) {
				adjustProjMatrixForReflection();
			}
		}
	}

	void Camera::calcViewPort() {
		// calculate viewport
		Rect rect = m_target->getRect();

		m_clientSize.set(rect.width, rect.height);
		m_viewport[0] = m_viewRect.x;

		if (m_viewRect.y < 0)
			m_viewport[1] = - m_viewRect.y;
		else
			m_viewport[1] = m_clientSize.y - m_viewRect.y - m_viewRect.height;

		m_viewport[2] = m_viewRect.width;
		m_viewport[3] = m_viewRect.height;

		m_viewportDX = m_viewport;
		m_viewportDX.y = m_clientSize.y - m_viewportDX.y - m_viewRect.height;
	}

	void Camera::calcFrustum() {
		m_vp = m_projMatrix * m_viewMatrix;
		m_vpInverse = m_vp.getInverse();
//		m_vpIT = m_vpInverse.getTranspose();
//		m_vpInverseNormal= m_vp.getTranspose();

		Matrix4 notranslate = m_viewMatrix;
		notranslate.removeTranslate();
		m_vpNoTranslate = m_projNoReflectionAdjust * notranslate;

		m_convex.initFromCamera(*this);
		return;

		Vector3 p;
#if 0
		m_frustum[0] = m_vpInverseNormal.transformNormal(Vector3(-1, 0, 0));
		m_frustum[0].normal().normalize();
		p = m_vpInverse * Vector3(1, 0, 0);
		m_frustum[0].fitThroughPoint(p);

		m_frustum[1] = m_vpInverseNormal.transformNormal(Vector3(1, 0, 0));
		m_frustum[1].normal().normalize();
		p = m_vpInverse * Vector3(-1, 0, 0);
		m_frustum[1].fitThroughPoint(p);

		m_frustum[2] = m_vpInverseNormal.transformNormal(Vector3(0, 1, 0));
		m_frustum[2].normal().normalize();
		p = m_vpInverse * Vector3(0, -1, 0);
		m_frustum[2].fitThroughPoint(p);

		m_frustum[3] = m_vpInverseNormal.transformNormal(Vector3(0, -1, 0));
		m_frustum[3].normal().normalize();
		p = m_vpInverse * Vector3(0, 1, 0);
		m_frustum[3].fitThroughPoint(p);

		m_frustum[4] = m_vpInverseNormal.transformNormal(Vector3(0, 0, -1));
		m_frustum[4].normal().normalize();
		p = m_vpInverse * Vector3(0, 0, 1);
		m_frustum[4].fitThroughPoint(p);

		m_frustum[5] = m_vpInverseNormal.transformNormal(Vector3(0, 0, 1));
		m_frustum[5].normal().normalize();
		p = m_vpInverse * Vector3(0, 0, -1);
		m_frustum[5].fitThroughPoint(p);
#endif
	}

	Camera Camera::createMirrorCamera(const Plane& mirror_plane) const {
		Camera result = *this;
		result.enableReflection(mirror_plane);
		return result;
	}

	void Camera::enableReflection(const Plane& mirror_plane) {
		m_isReflection = true;
		m_reflectionPlane = mirror_plane;

		// recalc matrix
		calcViewMatrix();
		calcFrustum();
	}

	void Camera::disableReflection() {
		m_isReflection = false;

		// recalc matrix
		calcViewMatrix();
		calcViewPort();
		calcProjectionMatrix();
		calcFrustum();
	}

	bool Camera::isReflectionEnabled() {
		return m_isReflection;
	}

	void Camera::getTrapezoidalMatrix(Matrix4& lightMatrix, Matrix4& tsmMatrix, const Vector3& lightdir, float focusdistance) const
	{
		if (m_isOrthoProjection) {
			Errorf("%s", __func__);
		}
		// shear xy, let lightdir is up

		//
		// calculate light matrix
		//

		// find view pos
		Vector3 viewpos = m_viewOrg + m_viewAxis[0] *(m_zfar + m_znear) * 0.5f;
		viewpos.z += 2000;

		Angles angle(90, 0, 0);
		Matrix3 matrix = angle.toMatrix3() * m_viewAxis;

		// ortho matrix
		Matrix4 ortho;
		ortho.setOrtho(-100, 100, -100, 100, -100, 100);
		

		// adjust light matrix's XZ plane to trapezoidal space
	}

	//
	// return four point in
	//			1 3
	//			0 2
	// order
	//
	void Camera::calcPointsAlongZdist(Vector3 result[4], float zdist) const {
		if (m_isOrthoProjection) {
			Errorf("eee");
		}
		AX_ASSERT(!m_isOrthoProjection);

		Vector3 center = m_viewAxis[0] * m_znear;
		float scale = zdist / m_znear;

		result[0] = m_viewOrg +(center + m_viewAxis[1] * m_left - m_viewAxis[2] * m_top) * scale;
		result[1] = m_viewOrg +(center + m_viewAxis[1] * m_left + m_viewAxis[2] * m_top) * scale;
		result[2] = m_viewOrg +(center - m_viewAxis[1] * m_left - m_viewAxis[2] * m_top) * scale;
		result[3] = m_viewOrg +(center - m_viewAxis[1] * m_left + m_viewAxis[2] * m_top) * scale;
	}


	float Camera::calcPerspectiveZ(float cameraZ) const {
		return (m_zfar + m_znear) / (m_zfar - m_znear) + 1 / cameraZ *(-2 * m_zfar * m_znear) /(m_zfar - m_znear);
	}

	inline float sgn(float a) {
		if (a > 0.0F) return (1.0F);
		if (a < 0.0F) return (-1.0F);
		return (0.0F);
	}

	inline float Dot(Vector4 v1, Vector4 v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	}

	static void ModifyProjectionMatrix(const Vector4& clipPlane, float matrix[16]) {
		//		float       matrix[16];
		Vector4    q;

		// Grab the current projection matrix from OpenGL
		//		glGetFloatv(GL_PROJECTION_MATRIX, matrix);

		// Calculate the clip-space corner point opposite the clipping plane
		// as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
		// transform it into camera space by multiplying it
		// by the inverse of the projection matrix

		q.x = (sgn(clipPlane.x) + matrix[8]) / matrix[0];
		q.y = (sgn(clipPlane.y) + matrix[9]) / matrix[5];
		q.z = -1.0F;
		q.w = (1.0F + matrix[10]) / matrix[14];

		// Calculate the scaled plane vector
		Vector4 c = clipPlane * (2.0F / Dot(clipPlane, q));

		// Replace the third row of the projection matrix
		matrix[2] = c.x;
		matrix[6] = c.y;
		matrix[10] = c.z + 1.0F;
		matrix[14] = c.w;

		// Load it back into OpenGL
		//		glMatrixMode(GL_PROJECTION);
		//		glLoadMatrixf(matrix);
	}

	void Camera::adjustProjMatrixForReflection()
	{
		Plane plane = getReflectionPlane();
		plane = -plane;
		plane = getViewMatrix().transformPlane(plane);
		Vector4 cplane = *(Vector4*)&plane;
		if (cplane.w > 0) {
			cplane *= -1;
		}
		ModifyProjectionMatrix(cplane, &m_projMatrix[0][0]);
	}

	void Camera::createCubemapCameras(const Camera& main, const AffineMat& mtx, Camera result[6], float znear/*=0.5f*/, float zfar/*=16.0f */)
	{
		static float faces[6][3] = {
			{ 1,2,3 },		// PX
			{ -1,-2,3 },	// NX
			{ 2,-1,3 },		// PY
			{ -2,1,3 },		// NY
			{ 3,1,2 },		// PZ
			{ -3,-1,2 },	// NZ
		};

		const Matrix3& axis = mtx.axis;

		for (int i=0; i<6; i++) {
			result[i] = main;
			result[i].setOrigin(mtx.origin);

			Matrix3 myaxis(
				axis[abs(faces[i][0])-1] * Math::sign(faces[i][0]),
				axis[abs(faces[i][1])-1] * Math::sign(faces[i][1]),
				axis[abs(faces[i][2])-1] * Math::sign(faces[i][2])
				);

			result[i].setViewAxis(myaxis);

			result[i].setFov(90, 90, znear, zfar);
		}
	}


	void Camera::createCubemapCameras( const AffineMat& mtx, Camera result[6], float znear/*=0.5f*/, float zfar/*=16.0f*/ )
	{
		static float faces[6][3] = {
			{ 1,2,3 },		// PX
			{ -1,-2,3 },	// NX
			{ 2,-1,3 },		// PY
			{ -2,1,3 },		// NY
			{ 3,1,2 },		// PZ
			{ -3,-1,2 },	// NZ
		};

		const Matrix3& axis = mtx.axis;

		for (int i=0; i<6; i++) {
			result[i].setOrigin(mtx.origin);

			Matrix3 myaxis(
				axis[abs(faces[i][0])-1] * Math::sign(faces[i][0]),
				axis[abs(faces[i][1])-1] * Math::sign(faces[i][1]),
				axis[abs(faces[i][2])-1] * Math::sign(faces[i][2])
				);

			result[i].setViewAxis(myaxis);

			result[i].setFov(90, 90, znear, zfar);
		}
	}

	void Convex::initFromCamera( const Camera& camera )
	{
		Vector3 p;

		m_numPlanes = 6;
#if 0
		m_numVertexes = 8;
		m_numEdges = 12;
#endif
		Matrix4 m_vpInverseNormal= camera.m_vp.getTranspose();

		// init planes
		m_planes[0] = m_vpInverseNormal.transformNormal(Vector3(-1, 0, 0));
		m_planes[0].normal().normalize();
		p = camera.m_vpInverse * Vector3(1, 0, 0);
		m_planes[0].fitThroughPoint(p);

		m_planes[1] = m_vpInverseNormal.transformNormal(Vector3(1, 0, 0));
		m_planes[1].normal().normalize();
		p = camera.m_vpInverse * Vector3(-1, 0, 0);
		m_planes[1].fitThroughPoint(p);

		m_planes[2] = m_vpInverseNormal.transformNormal(Vector3(0, 1, 0));
		m_planes[2].normal().normalize();
		p = camera.m_vpInverse * Vector3(0, -1, 0);
		m_planes[2].fitThroughPoint(p);

		m_planes[3] = m_vpInverseNormal.transformNormal(Vector3(0, -1, 0));
		m_planes[3].normal().normalize();
		p = camera.m_vpInverse * Vector3(0, 1, 0);
		m_planes[3].fitThroughPoint(p);

		m_planes[4] = m_vpInverseNormal.transformNormal(Vector3(0, 0, -1));
		m_planes[4].normal().normalize();
		p = camera.m_vpInverse * Vector3(0, 0, 1);
		m_planes[4].fitThroughPoint(p);

		m_planes[5] = m_vpInverseNormal.transformNormal(Vector3(0, 0, 1));
		m_planes[5].normal().normalize();
		p = camera.m_vpInverse * Vector3(0, 0, -1);
		m_planes[5].fitThroughPoint(p);
#if 0
		// init vertexes
		int count = 0;
		for (float i = -1; i <= 1; i += 2) {
			for (float j = -1; j <= 1; j += 2) {
				for (float k = -1; k <= 1; k += 2) {
					m_vertexes[count++] = camera.m_vpInverse * Vector3(i,j,-k);
				}
			}
		}

		// init edges
		static int edges[] = {
			0, 1, 1, 2,
			1, 6, 5, 2,
			6, 4, 0, 2,
			4, 0, 4, 2,

			0, 2, 4, 1,
			1, 3, 1, 5,
			6, 7, 5, 0,
			4, 5, 0, 4,

			2, 3, 3, 1,
			3, 7, 3, 5,
			7, 5, 3, 0,
			5, 2, 3, 2
		};
		AX_ASSERT(sizeof(edges) == m_numEdges*sizeof(Edge));
		memcpy(m_edges, edges, sizeof(edges));
#endif
	}

	void Convex::initFromCamera( const Camera& shadowCamera, const Camera& visCamera )
	{
		const Convex& visConvex = visCamera.m_convex;

		initFromCamera(shadowCamera);
		return;

		if (!r_csmClipCamera->getBool())
			return;

		if (shadowCamera.isOrthoProjection()) {
			const Vector3& forward = shadowCamera.getViewAxis()[0];
			bool planevis[MAX_PLANES];

			for (int i = 0; i < visConvex.m_numPlanes; i++) {
				float dot = forward | visConvex.m_planes[i].getNormal();
				if (dot > 0) {
					planevis[i] = false;
					continue;
				}
				planevis[i] = true;
				m_planes[m_numPlanes++] = visConvex.m_planes[i];
			}
#if 0
			for (int i = 0; i < visConvex.m_numEdges; i++) {
				const Edge& edge = visConvex.m_edges[i];
				if (planevis[edge.p[0]] == planevis[edge.p[1]])
					continue;

				// found new silhouette, add new plane
				Vector3 normal = (visConvex.m_vertexes[edge.v[0]] - visConvex.m_vertexes[edge.v[1]]) ^ forward;

				if (!planevis[edge.p[0]]) {
					normal = -normal;
				}
				normal.normalize();
				Plane p(visConvex.m_vertexes[edge.v[0]], normal);
				m_planes[m_numPlanes++] = p;
			}
#endif
		} else {
			initFromCamera(shadowCamera);
		}

	}

}} // namespace Axon::Render


/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"


AX_BEGIN_NAMESPACE

	/////////////////////////////////////////////////////////////////////////////
	// Macros

#define INTERPOLATE(a, b, amt) (amt * ((b) - (a)) + (a))
#define RANDOM(a, b) (INTERPOLATE(a, b, (float(rand() % 100000) * 0.00001f))) 


	/////////////////////////////////////////////////////////////////////////////
	// Constants

	static  const   float   c_fPi = 3.14159265358979323846f;
	static  const   float   c_fHalfPi = c_fPi * 0.5f;
	static  const   float   c_fQuarterPi = c_fPi * 0.25f;
	static  const   float   c_fTwoPi = 2.0f * c_fPi;
	static  const   float   c_fRad2Deg = 57.29578f;
	static  const   float   c_fAxisScalar = 0.25f;
	static  const   float   c_fMaxControllableDeltaTime = 0.03f;


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::WindController::WindController

	RenderWind::WindController::WindController(void) :
		m_wantedValue(0.0f),
		m_currentValue(0.0f),
		m_lastDelta(0.0f),
		m_lastDeltaTime(0.0f)
	{}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::WindController::Advance

	void RenderWind::WindController::advance(float fDeltaTime, float fP, float fMaxA) {
		if (fDeltaTime > c_fMaxControllableDeltaTime)
			fDeltaTime = c_fMaxControllableDeltaTime;

		float fDelta = fP * fDeltaTime * (m_wantedValue - m_currentValue);

		if (fMaxA != 0.0f)
		{
			if (fabs(fDelta) > fabs(m_lastDelta) && fDeltaTime > 0.0f && m_lastDeltaTime > 0.0f)
			{
				float fAccel = fDelta / fDeltaTime - m_lastDelta / m_lastDeltaTime;
				if (fAccel < -fMaxA)
					fAccel = -fMaxA;
				if (fAccel > fMaxA)
					fAccel = fMaxA;

				fDelta = m_lastDelta + fAccel * fDeltaTime;
			}

			m_lastDelta = fDelta;
			m_lastDeltaTime = fDeltaTime;
		}

		m_currentValue += fDelta;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::WindController::Reset

	void RenderWind::WindController::reset(float fValue) {
		m_currentValue = fValue;
		m_wantedValue = fValue;
		m_lastDelta = 0.0f;
		m_lastDeltaTime = 0.0f;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::OscillationParams::OscillationParams

	RenderWind::OscillationParams::OscillationParams(float fLowWindAngle, float fHighWindAngle, float fLowWindSpeed, float fHighWindSpeed) :
		m_lowAngle(fLowWindAngle),
		m_highAngle(fHighWindAngle),
		m_lowSpeed(fLowWindSpeed),
		m_highSpeed(fHighWindSpeed),
		m_adjustedTime(0.0f)
	{
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::OscillationParams::Advance

	void RenderWind::OscillationParams::advance(float fDeltaTime, float fWindStrendth) {
		float fAngle = INTERPOLATE(m_lowAngle, m_highAngle, fWindStrendth);
		float fSpeed = INTERPOLATE(m_lowSpeed, m_highSpeed, fWindStrendth);
		m_adjustedTime += fDeltaTime * fSpeed;

		unsigned int uiSize = (unsigned int) m_outputs.size();
		for (unsigned int i = 0; i < uiSize; ++i)
			m_outputs[i] = fAngle * sinf(m_adjustedTime + (float)i);
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::RenderWind

	RenderWind::RenderWind() {
		m_lastTime = 0;

		setQuantities(NUM_WIND_MATRIXES, NUM_LEAF_ANGLES);
		setWindResponse(1, 0.01f);
		m_maxBendAngle = 35;
		m_branchExponent = 1;
		m_leafExponent = 1.07f;
		setGusting(0.4f, 0.1f, 5);
		setBranchHorizontal(8, 0.5, 12, 1);
		setBranchVertical(8, 0.5, 12, 2);
		setLeafRocking(10, 0.7f, 5, 1);
		setLeafRustling(20, 1.2f, 5, 30);

		reset();

		setWindStrengthAndDirection(0.02f, Vector3(-0.5, -0.5, 0.0f));
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::~RenderWind

	RenderWind::~RenderWind()
	{}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::SetQuantities

	void RenderWind::setQuantities(int iNumWindMatrices, int iNumLeafAngles) {
		if (iNumWindMatrices < 1)
			iNumWindMatrices = 1;

		if (iNumLeafAngles < 1)
			iNumLeafAngles = 1;

		m_windMatrices.clear();
		m_windMatrices.resize(iNumWindMatrices);

		m_branchHorizontal.m_outputs.clear();
		m_branchVertical.m_outputs.clear();
		m_branchHorizontal.m_outputs.resize(iNumWindMatrices);
		m_branchVertical.m_outputs.resize(iNumWindMatrices);

		m_leafRocking.m_outputs.clear();
		m_leafRustling.m_outputs.clear();
		m_leafAngleMatrices.clear();
		m_leafRocking.m_outputs.resize(iNumLeafAngles);
		m_leafRustling.m_outputs.resize(iNumLeafAngles);
		m_leafAngleMatrices.resize(iNumLeafAngles);
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::SetWindResponse

	void RenderWind::setWindResponse(float fResponse, float fReponseLimit) {
		m_windResponse = fResponse;
		m_windResponseLimit = fReponseLimit;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::SetWindStrengthAndDirection

	void RenderWind::setWindStrengthAndDirection(float strength, const Vector3& direction) {
		m_windStrength = Math::saturate(strength);

		Vector3 dir = direction.getNormalized();

		m_finalWindDirectionX.m_wantedValue = dir.x;
		m_finalWindDirectionY.m_wantedValue = dir.y;
		m_finalWindDirectionZ.m_wantedValue = dir.z;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::SetMaxBendAngle

	void RenderWind::setMaxBendAngle(float fMaxBendAngle) {
		m_maxBendAngle = fMaxBendAngle;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::SetExponents

	void RenderWind::setExponents(float fBranchExponent, float fLeafExponent) {
		m_branchExponent = fBranchExponent;
		m_leafExponent = fLeafExponent;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::SetGusting

	void RenderWind::setGusting(float fGustStrength, float fGustFrequency, float fGustDuration) {
		m_gustingStrength = fGustStrength;
		m_gustingFrequency = fGustFrequency;
		m_gustingDuration = fGustDuration;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::SetBranchHorizontal

	void RenderWind::setBranchHorizontal(float fLowWindAngle, float fHighWindAngle, float fLowWindSpeed, float fHighWindSpeed) {
		m_branchHorizontal.m_lowAngle = fLowWindAngle;
		m_branchHorizontal.m_highAngle = fHighWindAngle;
		m_branchHorizontal.m_lowSpeed = fLowWindSpeed;
		m_branchHorizontal.m_highSpeed = fHighWindSpeed;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::SetBranchVertical

	void RenderWind::setBranchVertical(float fLowWindAngle, float fHighWindAngle, float fLowWindSpeed, float fHighWindSpeed) {
		m_branchVertical.m_lowAngle = fLowWindAngle;
		m_branchVertical.m_highAngle = fHighWindAngle;
		m_branchVertical.m_lowSpeed = fLowWindSpeed;
		m_branchVertical.m_highSpeed = fHighWindSpeed;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::SetLeafRocking

	void RenderWind::setLeafRocking(float fLowWindAngle, float fHighWindAngle, float fLowWindSpeed, float fHighWindSpeed) {
		m_leafRocking.m_lowAngle = fLowWindAngle;
		m_leafRocking.m_highAngle = fHighWindAngle;
		m_leafRocking.m_lowSpeed = fLowWindSpeed;
		m_leafRocking.m_highSpeed = fHighWindSpeed;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::SetLeafRustling

	void RenderWind::setLeafRustling(float fLowWindAngle, float fHighWindAngle, float fLowWindSpeed, float fHighWindSpeed) {

		m_leafRustling.m_lowAngle = fLowWindAngle;
		m_leafRustling.m_highAngle = fHighWindAngle;
		m_leafRustling.m_lowSpeed = fLowWindSpeed;
		m_leafRustling.m_highSpeed = fHighWindSpeed;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::Advance

	void RenderWind::advance(float fCurrentTime, bool bUpdateMatrices, bool bUpdateLeafAngleMatrices, float fCameraX, float fCameraY, float fCameraZ) {
		fCurrentTime *= 0.1f;
		float fDeltaTime = fCurrentTime - m_lastTime;
		m_lastTime = fCurrentTime;

		// stop current gust if needed
		if (fCurrentTime > m_gustStopTime) {
			m_currentGustingStrength = 0.0f;
	    
			// make new gusts
			if (m_gustingFrequency * fDeltaTime > (float)rand() / (float)RAND_MAX) {
				m_currentGustingStrength = m_gustingStrength * ((float)rand() / (float)RAND_MAX + 0.25f);
				m_gustStopTime = fCurrentTime + m_gustingDuration * 0.5f * ((float)rand() / (float)RAND_MAX + 1.0f);
			}
		}

		// compute the actual wind strength due to gusting
		m_finalWindStrength.m_wantedValue = m_windStrength + m_currentGustingStrength;
		if (m_finalWindStrength.m_wantedValue > 1.0f)
			m_finalWindStrength.m_wantedValue = 1.0f;
		if (m_finalWindStrength.m_wantedValue < 0.0f)
			m_finalWindStrength.m_wantedValue = 0.0f;
		m_finalWindStrength.advance(fDeltaTime, m_windResponse, m_windResponseLimit);
		if (m_finalWindStrength.m_currentValue > 1.0f)
			m_finalWindStrength.m_currentValue = 1.0f;
		if (m_finalWindStrength.m_currentValue < 0.0f)
			m_finalWindStrength.m_currentValue = 0.0f;

		// smooth out the direction
		m_finalWindDirectionX.advance(fDeltaTime, m_windResponse, m_windResponseLimit);
		m_finalWindDirectionY.advance(fDeltaTime, m_windResponse, m_windResponseLimit);
		m_finalWindDirectionZ.advance(fDeltaTime, m_windResponse, m_windResponseLimit);

		// compute the branch and leaf wind strengths due to exponents
		float fBranchStrength = powf(m_finalWindStrength.m_currentValue, m_branchExponent);
		float fLeafStrength = powf(m_finalWindStrength.m_currentValue, m_leafExponent);

		// update the main tree bend angle (and compensate for unnormalized, over the pole movement)
		float fSum = m_finalWindDirectionX.m_currentValue * m_finalWindDirectionX.m_currentValue + 
						m_finalWindDirectionY.m_currentValue * m_finalWindDirectionY.m_currentValue + 
						m_finalWindDirectionZ.m_currentValue * m_finalWindDirectionZ.m_currentValue;
		if (fSum != 0.0f)
			fSum = sqrt(fSum);
		m_finalBendAngle = fBranchStrength * m_maxBendAngle * fSum;

		// update the oscillating parts
		m_branchHorizontal.advance(fDeltaTime, fBranchStrength);
		m_branchVertical.advance(fDeltaTime, fBranchStrength);
		m_leafRocking.advance(fDeltaTime, fLeafStrength);
		m_leafRustling.advance(fDeltaTime, fLeafStrength);

		if (bUpdateMatrices) {
			// build wind matrices
			float fWindAngle = -static_cast<float>(atan2(m_finalWindDirectionX.m_currentValue, m_finalWindDirectionY.m_currentValue));
			float fXRot = -cosf(fWindAngle);
			float fYRot = -sinf(fWindAngle);

			Matrix4 cTemp;
			unsigned int uiSize = (unsigned int)m_windMatrices.size();
			for (unsigned int i = 0; i < uiSize; ++i) {
				// vertical oscillation and bend angle are in the same direction, so do them at the same time
				float angle = m_finalBendAngle + m_branchVertical.m_outputs[i];
				m_windMatrices[i].setRotation(fXRot * angle, fYRot * angle, 0.0f);

				// handle horizontal oscillation
				angle = m_branchHorizontal.m_outputs[i];
				cTemp.setRotation(-fYRot * angle, fXRot * angle, 0.0f);

				// final matrix
				m_windMatrices[i] = m_windMatrices[i] * cTemp;
			}
		}

		if (bUpdateLeafAngleMatrices) {
			// build leaf angle matrices
			float afAdjustedCameraDir[3];

			afAdjustedCameraDir[0] = fCameraX;
			afAdjustedCameraDir[1] = fCameraY;
			afAdjustedCameraDir[2] = fCameraZ;

			float fAzimuth = atan2f(afAdjustedCameraDir[1], afAdjustedCameraDir[0]) * c_fRad2Deg;
			float fPitch = -asinf(afAdjustedCameraDir[2]) * c_fRad2Deg;

			unsigned int uiSize = (unsigned int)m_leafAngleMatrices.size();
			for (unsigned int i = 0; i < uiSize; ++i)
			{
				Matrix4& cMatrix = m_leafAngleMatrices[i];

				cMatrix.setIdentity();
				cMatrix.rotate(0, 0, fAzimuth);
				cMatrix.rotate(0, fPitch, 0);
				cMatrix.rotate(0, 0, m_leafRustling.m_outputs[i]);
				cMatrix.rotate(m_leafRocking.m_outputs[i], 0, 0);
			}
		}
	}


#if 0
	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::UpdateTree

	void Wind::updateTree(CSpeedTreeRT* pTree) {
		pTree->SetWindStrengthAndLeafAngles(m_finalWindStrength.m_currentValue, 
											&(m_leafRocking.m_outputs[0]), 
											&(m_leafRustling.m_outputs[0]), 
											int(m_leafRocking.m_outputs.size()));
	}

	/////////////////////////////////////////////////////////////////////////////
	// Wind::UpdateSpeedTreeRT

	void Wind::updateSpeedTreeRT(void)
	{
		unsigned int uiSize = (unsigned int) m_vWindMatrices.size();
		for (unsigned int i = 0; i < uiSize; ++i)
			CSpeedTreeRT::SetWindMatrix(i, (const float*)m_vWindMatrices[i].m_afData);
	}
#endif

	/////////////////////////////////////////////////////////////////////////////
	// Wind::GetFinalStrength

	float RenderWind::getFinalStrength(void) {
		return m_finalWindStrength.m_currentValue;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetNumWindMatrices

	unsigned int RenderWind::getNumWindMatrices(void) const {
		return (unsigned int) m_windMatrices.size();
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetWindMatrix

	float* RenderWind::getWindMatrix(unsigned int uiIndex) const {
		float* pReturn = NULL;

		if (uiIndex < m_windMatrices.size())
			pReturn = ((float*)&m_windMatrices[uiIndex]);

		return pReturn;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetNumWindMatrices

	unsigned int RenderWind::getNumLeafAngleMatrices(void) const {
		return (unsigned int) m_leafAngleMatrices.size();
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetLeafAngleMatrix

	float* RenderWind::getLeafAngleMatrix(unsigned int uiIndex) const {
		float* pReturn = NULL;

		if (uiIndex < m_windMatrices.size())
			pReturn = ((float*)&m_leafAngleMatrices[uiIndex]);

		return pReturn;
	}



	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::Reset

	void RenderWind::reset(void) {
		unsigned int i = 0;
		unsigned int uiSize = (unsigned int) m_windMatrices.size();
		for (i = 0; i < uiSize; ++i)
			m_windMatrices[i].setIdentity();

		uiSize = (unsigned int) m_leafAngleMatrices.size();
		for (i = 0; i < uiSize; ++i)
			m_leafAngleMatrices[i].setIdentity();

		m_finalWindStrength.reset(m_windStrength);
		m_finalWindDirectionX.reset(m_finalWindDirectionX.m_wantedValue);
		m_finalWindDirectionY.reset(m_finalWindDirectionY.m_wantedValue);
		m_finalWindDirectionZ.reset(m_finalWindDirectionZ.m_wantedValue);
	}

#if 0
	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetQuantities

	void Wind::getQuantities(int& iNumWindMatrices, int& iNumLeafAngles)
	{
		iNumWindMatrices = (int)m_vWindMatrices.size();
		iNumLeafAngles = (int)m_vLeafAngleMatrices.size();
	}
#endif

	/////////////////////////////////////////////////////////////////////////////
	// Wind::GetWindResponse

	void RenderWind::getWindResponse(float& fResponse, float& fReponseLimit) {
		fResponse = m_windResponse;
		fReponseLimit = m_windResponseLimit;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetWindStrengthAndDirection

	void RenderWind::getWindStrengthAndDirection(float& fWindStrength, float& fWindDirectionX, float& fWindDirectionY, float& fWindDirectionZ) {
		fWindStrength = m_windStrength;
		fWindDirectionX = m_finalWindDirectionX.m_wantedValue;
		fWindDirectionY = m_finalWindDirectionY.m_wantedValue;
		fWindDirectionZ = m_finalWindDirectionZ.m_wantedValue;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetMaxBendAngle

	float RenderWind::getMaxBendAngle(void) {
		return m_maxBendAngle;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetExponents

	void RenderWind::getExponents(float& fBranchExponent, float& fLeafExponent) {
		fBranchExponent = m_branchExponent;
		fLeafExponent = m_leafExponent;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetGusting

	void RenderWind::getGusting(float& fGustStrength, float& fGustFrequency, float& fGustDuration) {
		fGustStrength = m_gustingStrength;
		fGustFrequency = m_gustingFrequency;
		fGustDuration = m_gustingDuration;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetBranchHorizontal

	void RenderWind::getBranchHorizontal(float& fLowWindAngle, float& fHighWindAngle, float& fLowWindSpeed, float& fHighWindSpeed) {
		fLowWindAngle = m_branchHorizontal.m_lowAngle;
		fHighWindAngle = m_branchHorizontal.m_highAngle;
		fLowWindSpeed = m_branchHorizontal.m_lowSpeed;
		fHighWindSpeed = m_branchHorizontal.m_highSpeed;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetBranchVertical

	void RenderWind::getBranchVertical(float& fLowWindAngle, float& fHighWindAngle, float& fLowWindSpeed, float& fHighWindSpeed) {
		fLowWindAngle = m_branchVertical.m_lowAngle;
		fHighWindAngle = m_branchVertical.m_highAngle;
		fLowWindSpeed = m_branchVertical.m_lowSpeed;
		fHighWindSpeed = m_branchVertical.m_highSpeed;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetLeafRocking

	void RenderWind::getLeafRocking(float& fLowWindAngle, float& fHighWindAngle, float& fLowWindSpeed, float& fHighWindSpeed) {
		fLowWindAngle = m_leafRocking.m_lowAngle;
		fHighWindAngle = m_leafRocking.m_highAngle;
		fLowWindSpeed = m_leafRocking.m_lowSpeed;
		fHighWindSpeed = m_leafRocking.m_highSpeed;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetLeafRustling

	void RenderWind::getLeafRustling(float& fLowWindAngle, float& fHighWindAngle, float& fLowWindSpeed, float& fHighWindSpeed) {
		fLowWindAngle = m_leafRustling.m_lowAngle;
		fHighWindAngle = m_leafRustling.m_highAngle;
		fLowWindSpeed = m_leafRustling.m_lowSpeed;
		fHighWindSpeed = m_leafRustling.m_highSpeed;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetRustleAngles

	bool RenderWind::getRustleAngles(FloatSeq& vAngles) const {
		bool bSuccess = false;

		if (vAngles.size() == m_leafRustling.m_outputs.size())
		{
			memcpy(&vAngles[0], &m_leafRustling.m_outputs[0], vAngles.size() * sizeof(float));
			bSuccess = true;
		}

		return bSuccess;
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::GetRockAngles

	bool RenderWind::getRockAngles(FloatSeq& vAngles) const {
		bool bSuccess = false;

		if (vAngles.size() == m_leafRocking.m_outputs.size())
		{
			memcpy(&vAngles[0], &m_leafRocking.m_outputs[0], vAngles.size() * sizeof(float));
			bSuccess = true;
		}

		return bSuccess;
	}

	void RenderWind::getWindMatrix(Matrix4 output[]) const {
		for (int i = 0; i < NUM_WIND_MATRIXES; i++) {
			output[i] = m_windMatrices[i];
		}
	}

	void RenderWind::getLeafAngles(Vector4 output[]) const {
		for (int i = 0; i < NUM_LEAF_ANGLES; i++) {
			output[i].x = Math::d2r(m_leafRocking.m_outputs[i]);
			output[i].y = Math::d2r(m_leafRustling.m_outputs[i]);
			output[i].z = 0;
			output[i].w = 0;
		}
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::InterpolateParameters

	void RenderWind::interpolateParameters(RenderWind* pWind1, RenderWind* pWind2, float fInterpolation) {
	#define INTERPOLATE_WIND_PARAM(a) a = INTERPOLATE(pWind1->a, pWind2->a, fInterpolation)

		if (pWind1 != NULL && pWind2 != NULL)
		{
			INTERPOLATE_WIND_PARAM(m_windStrength);
			INTERPOLATE_WIND_PARAM(m_finalWindDirectionX.m_wantedValue);
			INTERPOLATE_WIND_PARAM(m_finalWindDirectionY.m_wantedValue);
			INTERPOLATE_WIND_PARAM(m_finalWindDirectionZ.m_wantedValue);

			// normalize direction
			float fSum = m_finalWindDirectionX.m_wantedValue * m_finalWindDirectionX.m_wantedValue + 
						m_finalWindDirectionY.m_wantedValue * m_finalWindDirectionY.m_wantedValue + 
						m_finalWindDirectionZ.m_wantedValue * m_finalWindDirectionZ.m_wantedValue;
			if (fSum != 0.0f)
			{
				fSum = sqrt(fSum);
				m_finalWindDirectionX.m_wantedValue /= fSum;
				m_finalWindDirectionY.m_wantedValue /= fSum;
				m_finalWindDirectionZ.m_wantedValue /= fSum;
			}

			INTERPOLATE_WIND_PARAM(m_branchHorizontal.m_lowAngle);
			INTERPOLATE_WIND_PARAM(m_branchHorizontal.m_highAngle);
			INTERPOLATE_WIND_PARAM(m_branchHorizontal.m_lowSpeed);
			INTERPOLATE_WIND_PARAM(m_branchHorizontal.m_highSpeed);

			INTERPOLATE_WIND_PARAM(m_branchVertical.m_lowAngle);
			INTERPOLATE_WIND_PARAM(m_branchVertical.m_highAngle);
			INTERPOLATE_WIND_PARAM(m_branchVertical.m_lowSpeed);
			INTERPOLATE_WIND_PARAM(m_branchVertical.m_highSpeed);

			INTERPOLATE_WIND_PARAM(m_leafRocking.m_lowAngle);
			INTERPOLATE_WIND_PARAM(m_leafRocking.m_highAngle);
			INTERPOLATE_WIND_PARAM(m_leafRocking.m_lowSpeed);
			INTERPOLATE_WIND_PARAM(m_leafRocking.m_highSpeed);

			INTERPOLATE_WIND_PARAM(m_leafRustling.m_lowAngle);
			INTERPOLATE_WIND_PARAM(m_leafRustling.m_highAngle);
			INTERPOLATE_WIND_PARAM(m_leafRustling.m_lowSpeed);
			INTERPOLATE_WIND_PARAM(m_leafRustling.m_highSpeed);

			INTERPOLATE_WIND_PARAM(m_maxBendAngle);
			INTERPOLATE_WIND_PARAM(m_windResponse);
			INTERPOLATE_WIND_PARAM(m_windResponseLimit);
			INTERPOLATE_WIND_PARAM(m_gustingStrength);
			INTERPOLATE_WIND_PARAM(m_gustingFrequency);
			INTERPOLATE_WIND_PARAM(m_gustingDuration);
			INTERPOLATE_WIND_PARAM(m_branchExponent);
			INTERPOLATE_WIND_PARAM(m_leafExponent);
	        
		}

	#undef INTERPOLATE_WIND_PARAM
	}


	/////////////////////////////////////////////////////////////////////////////
	// RenderWind::BlendParameters

	void RenderWind::blendParameters(RenderWind** pWinds, float* pWeights, unsigned int uiNumWinds) {
		// zero out our data
		m_windStrength = 0.0f;
		m_finalWindDirectionX.m_wantedValue = 0.0f;
		m_finalWindDirectionY.m_wantedValue = 0.0f;
		m_finalWindDirectionZ.m_wantedValue = 0.0f;

		m_branchHorizontal.m_lowAngle = 0.0f;
		m_branchHorizontal.m_highAngle = 0.0f;
		m_branchHorizontal.m_lowSpeed = 0.0f;
		m_branchHorizontal.m_highSpeed = 0.0f;

		m_branchVertical.m_lowAngle = 0.0f;
		m_branchVertical.m_highAngle = 0.0f;
		m_branchVertical.m_lowSpeed = 0.0f;
		m_branchVertical.m_highSpeed = 0.0f;

		m_leafRocking.m_lowAngle = 0.0f;
		m_leafRocking.m_highAngle = 0.0f;
		m_leafRocking.m_lowSpeed = 0.0f;
		m_leafRocking.m_highSpeed = 0.0f;

		m_leafRustling.m_lowAngle = 0.0f;
		m_leafRustling.m_highAngle = 0.0f;
		m_leafRustling.m_lowSpeed = 0.0f;
		m_leafRustling.m_highSpeed = 0.0f;

		m_maxBendAngle = 0.0f;
		m_windResponse = 0.0f;
		m_windResponseLimit = 0.0f;
		m_gustingStrength = 0.0f;
		m_gustingFrequency = 0.0f;
		m_gustingDuration = 0.0f;
		m_branchExponent = 0.0f;
		m_leafExponent = 0.0f;

	    
	#define SUM_WIND_PARAM(a) a = pWinds[i]->a * pWeights[i]

		// fill data based on winds and weights
		for (unsigned int i = 0; i < uiNumWinds; ++i)
		{
			SUM_WIND_PARAM(m_windStrength);
			SUM_WIND_PARAM(m_finalWindDirectionX.m_wantedValue);
			SUM_WIND_PARAM(m_finalWindDirectionY.m_wantedValue);
			SUM_WIND_PARAM(m_finalWindDirectionZ.m_wantedValue);

			SUM_WIND_PARAM(m_branchHorizontal.m_lowAngle);
			SUM_WIND_PARAM(m_branchHorizontal.m_highAngle);
			SUM_WIND_PARAM(m_branchHorizontal.m_lowSpeed);
			SUM_WIND_PARAM(m_branchHorizontal.m_highSpeed);

			SUM_WIND_PARAM(m_branchVertical.m_lowAngle);
			SUM_WIND_PARAM(m_branchVertical.m_highAngle);
			SUM_WIND_PARAM(m_branchVertical.m_lowSpeed);
			SUM_WIND_PARAM(m_branchVertical.m_highSpeed);

			SUM_WIND_PARAM(m_leafRocking.m_lowAngle);
			SUM_WIND_PARAM(m_leafRocking.m_highAngle);
			SUM_WIND_PARAM(m_leafRocking.m_lowSpeed);
			SUM_WIND_PARAM(m_leafRocking.m_highSpeed);

			SUM_WIND_PARAM(m_leafRustling.m_lowAngle);
			SUM_WIND_PARAM(m_leafRustling.m_highAngle);
			SUM_WIND_PARAM(m_leafRustling.m_lowSpeed);
			SUM_WIND_PARAM(m_leafRustling.m_highSpeed);

			SUM_WIND_PARAM(m_maxBendAngle);
			SUM_WIND_PARAM(m_windResponse);
			SUM_WIND_PARAM(m_windResponseLimit);
			SUM_WIND_PARAM(m_gustingStrength);
			SUM_WIND_PARAM(m_gustingFrequency);
			SUM_WIND_PARAM(m_gustingDuration);
			SUM_WIND_PARAM(m_branchExponent);
			SUM_WIND_PARAM(m_leafExponent);
		}

	#undef SUM_WIND_PARAM

		// normalize direction
		float fSum = m_finalWindDirectionX.m_wantedValue * m_finalWindDirectionX.m_wantedValue + 
					m_finalWindDirectionY.m_wantedValue * m_finalWindDirectionY.m_wantedValue + 
					m_finalWindDirectionZ.m_wantedValue * m_finalWindDirectionZ.m_wantedValue;
		if (fSum != 0.0f)
		{
			fSum = sqrt(fSum);
			m_finalWindDirectionX.m_wantedValue /= fSum;
			m_finalWindDirectionY.m_wantedValue /= fSum;
			m_finalWindDirectionZ.m_wantedValue /= fSum;
		}
	}

AX_END_NAMESPACE


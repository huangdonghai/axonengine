/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_WIND_H
#define AX_RENDER_WIND_H

namespace Axon { namespace Render {

	class Wind {
	public:
		enum {
			NUM_WIND_MATRIXES = 3,
			NUM_LEAF_ANGLES = 8
		};

		Wind();
		~Wind();

		void setWindStrengthAndDirection(float strength, const Vector3& direction);

		// parameter setting
		void reset(void);
		void setQuantities(int iNumWindMatrices, int iNumLeafAngles);
		void setWindResponse(float fResponse, float fReponseLimit);
		void setMaxBendAngle(float fMaxBendAngle);
		void setExponents(float fBranchExponent, float fLeafExponent);
		void setGusting(float fGustStrength, float fGustFrequency, float fGustDuration);
		void setBranchHorizontal(float fLowWindAngle, float fHighWindAngle, float fLowWindSpeed, float fHighWindSpeed);
		void setBranchVertical(float fLowWindAngle, float fHighWindAngle, float fLowWindSpeed, float fHighWindSpeed);
		void setLeafRocking(float fLowWindAngle, float fHighWindAngle, float fLowWindSpeed, float fHighWindSpeed);
		void setLeafRustling(float fLowWindAngle, float fHighWindAngle, float fLowWindSpeed, float fHighWindSpeed);

		// parameter getting
		void getWindResponse(float& fResponse, float& fReponseLimit);
		void getWindStrengthAndDirection(float& fWindStrength, float& fWindDirectionX, float& fWindDirectionY, float& fWindDirectionZ);
		float getMaxBendAngle(void);
		void getExponents(float& fBranchExponent, float& fLeafExponent);
		void getGusting(float& fGustStrength, float& fGustFrequency, float& fGustDuration);
		void getBranchHorizontal(float& fLowWindAngle, float& fHighWindAngle, float& fLowWindSpeed, float& fHighWindSpeed);
		void getBranchVertical(float& fLowWindAngle, float& fHighWindAngle, float& fLowWindSpeed, float& fHighWindSpeed);
		void getLeafRocking(float& fLowWindAngle, float& fHighWindAngle, float& fLowWindSpeed, float& fHighWindSpeed);
		void getLeafRustling(float& fLowWindAngle, float& fHighWindAngle, float& fLowWindSpeed, float& fHighWindSpeed);

		// render interface
		void advance(float fCurrentTime, bool bUpdateMatrices = true, bool bUpdateLeafAngleMatrices = false, float fCameraX = 0.0f, float fCameraY = 0.0f, float fCameraZ = 1.0f);
		void updateSpeedTreeRT(void);
#if 0
		void updateTree(CSpeedTreeRT* pTree);
#endif
		float getFinalStrength(void);
		unsigned int getNumWindMatrices(void) const;
		float* getWindMatrix(unsigned int uiIndex) const;
		unsigned int getNumLeafAngleMatrices(void) const;
		float* getLeafAngleMatrix(unsigned int uiIndex) const;
		bool getRustleAngles(FloatSeq& vAngles) const;  // assumes vector is already appropriately sized
		bool getRockAngles(FloatSeq& vAngles) const;    // assumes vector is already appropriately sized
		void getWindMatrix(Matrix4 output[]) const;
		void getLeafAngles(Vector4 output[]) const;

		// blending SpeedWinds into this one
		void interpolateParameters(Wind* pWind1, Wind* pWind2, float fInterpolation);
		void blendParameters(Wind** pWinds, float* pWeights, unsigned int uiNumWinds);

	protected:
		struct OscillationParams {
			OscillationParams(float fLowWindAngle = 10.0f, float fHighWindAngle = 4.0f, float fLowWindSpeed = 0.01f, float fHighWindSpeed = 0.3f);

			void advance(float fDeltaTime, float fWindStrendth);

			float m_lowAngle;                    // The amount of movement at 0.0 wind strength
			float m_highAngle;                   // The amount of movement at 1.0 wind strength
			float m_lowSpeed;                    // The speed of movement at 0.0m wind strength
			float m_highSpeed;                   // The speed of movement at 1.0 wind strength
			float m_adjustedTime;                    // Internal concept of time
			FloatSeq m_outputs;                         // Output angles for this oscillator
		};

		class WindController {
		public:
			WindController();

			void advance(float fDeltaTime, float fP, float fMaxA);
			void reset(float fValue);

		public:
			float m_wantedValue;                     // The target output value for this controller
			float m_currentValue;                    // The current output value of this controller
			float m_lastDelta;                       // The change made on the last time step
			float m_lastDeltaTime;                   // The duration of the last time step
		};

		// SpeedWind2 Parameters
		OscillationParams m_branchHorizontal;                // Horizontal (to the wind direction) branch movement oscillator
		OscillationParams m_branchVertical;                  // Vertical (to the wind direction) branch movement oscillator
		OscillationParams m_leafRocking;                     // Leaf rocking oscillator
		OscillationParams m_leafRustling;                    // Leaf rustling oscillator
		float m_maxBendAngle;                    // The maximum bend angle of the tree
		float m_windResponse;                    // The speed of the wind strength controller
		float m_windResponseLimit;               // The acceleration limiter on the wind strength controller
		float m_gustingStrength;                 // The maximum strength of wind gusts
		float m_gustingFrequency;                // The approximate frequency of wind gusts (in gusts per second)
		float m_gustingDuration;                 // The maximum duration of a gust once it has started
		float m_branchExponent;                  // The exponent placed on the wind strength before branch oscillation
		float m_leafExponent;                    // The exponent placed on the wind strength before leaf rocking/rustling

		// SpeedWind2 Internals
		float m_lastTime;                        // The last time Advance was called
		float m_windStrength;                    // The current wind strength (no gusting)
		float m_currentGustingStrength;          // The current gusting strength
		float m_gustStopTime;                    // The time when the current gust will stop

		// SpeedWind2 Outputs
		WindController m_finalWindStrength;               // The wind strength controller
		WindController m_finalWindDirectionX;             // The wind direction X controller
		WindController m_finalWindDirectionY;             // The wind direction Y controller
		WindController m_finalWindDirectionZ;             // The wind direction Z controller
		float m_finalBendAngle;                  // The current bend angle of the tree

		Sequence<Matrix4>	m_windMatrices;                    // The wind matrices
		Sequence<Matrix4>	m_leafAngleMatrices;               // The leaf angle matrices
	};

}} // Axon::Render

#endif // end guardian


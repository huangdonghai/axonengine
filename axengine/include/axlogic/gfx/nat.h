/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GFX_NAT_H
#define AX_GFX_NAT_H

namespace Axon {

	// interpolation functions
	template<class T>
	inline T interpolate(const float r, const T &v1, const T &v2)
	{
		return static_cast<T>(v1*(1.0f - r) + v2*r);
	}

	template<class T>
	inline T interpolateHermite(const float r, const T &v1, const T &v2, const T &in, const T &out)
	{
		// basis functions
		float h1 = 2.0f*r*r*r - 3.0f*r*r + 1.0f;
		float h2 = -2.0f*r*r*r + 3.0f*r*r;
		float h3 = r*r*r - 2.0f*r*r + r;
		float h4 = r*r*r - r*r;

		// interpolation
		return static_cast<T>(v1*h1 + v2*h2 + in*h3 + out*h4);
	}

	// "linear" interpolation for quaternions should be slerp by default
	template<>
	inline Quaternion interpolate<Quaternion>(const float r, const Quaternion &v1, const Quaternion &v2)
	{
		return v1.slerp(v2, r);
	}

	template <class T>
	class Nat
	{
	public:
		enum InterpolateType
		{
			InterpolateType_None,
			InterpolateType_Linear,
			InterpolateType_Cubic,
			InterpolateType_Hermite,
		};

		typedef struct {
			int time;
			T val;
		} KeyValue;

		T getValue(int time);

		InterpolateType m_interpolateType;
		FixedString m_name;
		Sequence<KeyValue> m_keyValues;
		bool m_loop;
	};

	class IAnimatable
	{
	public:
		enum Type { kTrack, kGroup, kObject };

		enum InterpolateType
		{
			InterpolateType_None,
			InterpolateType_Linear,
			InterpolateType_Cubic,
			InterpolateType_Hermite,
		};

		virtual ~IAnimatable() {}

		virtual String getName();
		virtual DataType getDataType();
		virtual InterpolateType getInterpolateType();
		virtual void setInterpolateType(InterpolateType it);
		virtual int numSubAnims();
		virtual IAnimatable* getSubAnim(int index);
		virtual int numKeys();
		virtual int getKeyTime(int index);
		virtual int getKeyIndex(int ms);
		virtual void getValue(void *value, int ticks) = 0;
		virtual void setValue(void *value, int ticks) = 0;
	};

	class FloatTrack : public IAnimatable
	{
	public:


	private:
		struct FloatKey
		{
			float value;
			int ticks;
		};
	};

	class VectorTrack : public IAnimatable
	{
	public:

	private:
		struct VectorKey
		{
			Vector3 value;
			int ticks;
		};
	};

	class ColorTrack : public VectorTrack
	{
	public:

	private:
	};

	class ObjectTrack : public IAnimatable
	{
	public:
		ObjectTrack(Object* obj);
		virtual ~ObjectTrack() {}


	private:
	};

} // namespace Axon

#endif

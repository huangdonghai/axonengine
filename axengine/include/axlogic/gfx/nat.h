/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GFX_NAT_H
#define AX_GFX_NAT_H

AX_BEGIN_NAMESPACE

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

//--------------------------------------------------------------------------
struct KeyBase
{
	int ticks;
};

//--------------------------------------------------------------------------
struct TcbKey : public KeyBase
{
	float tension, continuity, bias, easeIn, easeOut;
};

//--------------------------------------------------------------------------
struct FloatKey : public KeyBase
{
	float val;
};

//--------------------------------------------------------------------------
struct VectorKey : public KeyBase
{
	Vector3 val;
};

//--------------------------------------------------------------------------
struct ColorKey : public VectorKey
{
	float alpha;
};


//--------------------------------------------------------------------------
class Control
{
public:
	enum ControlClass {
		kFloat,
		kVector,
		kColor,
		kPosition,
		kRotation,
		kScale,
		kText,
	};

	enum ControlType {
		kStep,
		kLinear,
		kCatmullRom,
		kSimpleMax = kCatmullRom,	// simple key flag
		kTcb,
		kBezier,
		kProcedural,	// not keyframed, procedural generated
	};

	virtual ~Control() {}

	virtual ControlClass getControlClass() const = 0;
	virtual ControlType getControlType() const = 0;

	virtual void step(int delta) = 0;

#if 0
	virtual int getNumKeys ();
	virtual void setNumKeys (int n);
	virtual void getKey(int i, KeyBase *key);
	virtual void setKey(int i, KeyBase *key);
	virtual int appendKey(KeyBase *key);
	virtual void sortKeys();
	virtual DWORD &getTrackFlags ();
	virtual int getKeySize ();
#endif

	bool isKeyable() const { return getControlType() != kProcedural; }
	bool isSimple() const { return getControlType() <= kSimpleMax; }

private:
	Object *m_object;
	Member *m_target;
};

class Animator
{
public:
	enum CycleType {
		CycleType_Loop,
		CycleType_Reverse,
		CycleType_Clamp
	};
	virtual ~Animator() {}

	CycleType getCycleType() const { return m_cycleType; }
	void setCycleType(CycleType val) { m_cycleType = val; }

	Control *findControl(Object *obj, Handle handle) const;
	Control *findControl(Object *obj, const char *name) const;
	Control *findControl(Object *obj, Member *member) const;

private:
	Sequence<Control*> m_controls;
	CycleType m_cycleType;
};

//--------------------------------------------------------------------------
class FloatTrack : public Control
{
public:
	FloatTrack(const String &name);
	virtual ~FloatTrack();

private:
	String m_name;
	Sequence<FloatKey> m_keyValues;
};

//--------------------------------------------------------------------------
class VectorTrack : public Control
{
public:
	VectorTrack(const String &name);
	virtual ~VectorTrack();

private:
	String m_name;
	Sequence<VectorKey> m_keyValues;
};

//--------------------------------------------------------------------------
class ColorTrack : public Control
{
public:
	ColorTrack(const String &name);
	virtual ~ColorTrack();

private:
};

AX_END_NAMESPACE

#endif

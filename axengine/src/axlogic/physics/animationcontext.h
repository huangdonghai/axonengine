/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_PHYSICS_ANIMATIONCONTEXT_H
#define AX_PHYSICS_ANIMATIONCONTEXT_H

AX_BEGIN_NAMESPACE

// forward declaration
class AnimationContext;

//--------------------------------------------------------------------------
// class AnimationChannel
//--------------------------------------------------------------------------

class AnimationChannel
{
public:
	friend class AnimationContext;

	AnimationChannel(AnimationContext *context, int index, const std::string &name, const StringSeq &bones);
	~AnimationChannel();

	void step(int msec);

protected:
	void initAnimations();
	void clear();

	// script call
	void playCycle(int anim);
	void playAnim(int anim);
	bool isAnimDone(float timeleft);
	void switchState(const std::string &state, float easein);

private:
	// assets
	AnimationContext *m_context;
	int m_index;
	std::string m_channelName;
	StringSeq m_bones;
	HavokAnimator *m_animator;
	HavokPose *m_pose;
	std::vector<HavokAnimation*> m_animations;

	// runtime
	hkReal m_currentTime;
	std::string m_state;
	float m_easeinDuration;
	HavokAnimation *m_currentAnimation;
};

//--------------------------------------------------------------------------
// class AnimationContext
//--------------------------------------------------------------------------

class AnimationContext : public Object
{
	AX_DECLARE_CLASS(AnimationContext, Object)
		AX_METHOD(playCycle)
		AX_METHOD(playCycleSync)
		AX_METHOD(playAnim)
		AX_METHOD(isAnimDone)
		AX_METHOD(switchState)
	AX_END_CLASS()
public:
	friend class AnimationChannel;

	AnimationContext();
	virtual ~AnimationContext();

	void initFromLua(const std::string &luaobj);

	HavokPose *getPose() const;

	void step(int msec);

protected:
	// init
//	void initChannel(const LuaTable &table);
	void initBoneChannelMap();
	void initAnimations();

	// script method
	void playCycle(const std::string &anim);
	void playCycleSync(const std::string &anim, int syncState); // play cycled sync to other state
	void playAnim(const std::string &anim);

	bool isAnimDone(float timeleft);

	void switchState(const std::string &state, float easein);

	void clear();

private:
	enum { MAX_CHANNELS = 4, MAX_BONES = 256 };

	std::string m_lua;
	int m_numChannels;
	AnimationChannel *m_channels[MAX_CHANNELS];

	// assets
	HavokRig *m_rig;
	int m_boneChannelMap[MAX_BONES]; // identify each bones channel
	Dict<std::string,int> m_animDict; // map animations name and filename
	StringSeq m_animations;

	// runtime
	hkReal m_currentTime;
	int m_currentChannel;
};


AX_END_NAMESPACE

#endif // end guardian


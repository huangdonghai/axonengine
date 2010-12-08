/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class AnimationContext
//--------------------------------------------------------------------------

AnimationChannel::AnimationChannel(AnimationContext *context, int index, const std::string &name, const StringSeq &bones)
	: m_context(context)
	, m_index(index)
	, m_channelName(name)
	, m_bones(bones)
{
	m_currentAnimation = 0;
	m_easeinDuration = 0.1f;

	initAnimations();
}

AnimationChannel::~AnimationChannel()
{

}

void AnimationChannel::playCycle(int anim)
{
	if (m_currentAnimation) {
		m_currentAnimation->easeOut(m_easeinDuration);
	}
	m_animations[anim]->setMasterWeight(1.0f);
	m_animations[anim]->setLocalTime(0);
	m_animations[anim]->easeIn(m_easeinDuration);
	m_currentAnimation = m_animations[anim];
}

void AnimationChannel::playAnim(int anim)
{

}

void AnimationChannel::initAnimations()
{
	m_animator = new HavokAnimator(m_context->m_rig);
	m_pose = new HavokPose(m_context->m_rig);

	const StringSeq &anim_names = m_context->m_animations;

	m_animations.resize(anim_names.size());

	for (size_t i = 0; i < anim_names.size(); i++) {
		m_animations[i] = new HavokAnimation(anim_names[i]);

		AX_ASSURE(m_animations[i]->isValid())

		m_animations[i]->setMasterWeight(0.0f);
		m_animator->addAnimation(m_animations[i]);
	}
}

void AnimationChannel::clear()
{

}

bool AnimationChannel::isAnimDone(float timeleft)
{
	if (!m_currentAnimation) return true;
	return m_currentAnimation->isAnimDone(timeleft);
}

void AnimationChannel::step(int msec)
{
	std::string update = m_context->m_lua + '.' + m_channelName + '.' + m_state + '.' + "update";
#if 0
	g_scriptSystem->invokeLuaMethod(update.c_str(), Variant(m_context));
#endif
	m_animator->step(msec);
	m_animator->renderToPose(m_pose);
}

void AnimationChannel::switchState(const std::string &state, float easein)
{
	if (m_state == state) {
		return;
	}

	m_state = state;
	m_easeinDuration = easein;

	// enterstate
	std::string enterstate = m_context->m_lua + '.' + m_channelName + '.' + m_state + '.' + "enterState";
#if 0
	g_scriptSystem->invokeLuaMethod(enterstate.c_str(), Variant(m_context));
#endif
}

//--------------------------------------------------------------------------
// class AnimationContext
//--------------------------------------------------------------------------

AnimationContext::AnimationContext()
{
	m_currentTime = 0;
	m_currentChannel = -1;

	TypeZeroArray(m_channels);
}

AnimationContext::~AnimationContext()
{

}

void AnimationContext::initFromLua(const std::string &luaobj)
{
	m_lua = luaobj;

#if 0
	g_scriptSystem->beginRead();

	LuaTable table = g_scriptSystem->readField(luaobj.c_str());

	table.beginRead();

	{
		LuaTable assets = table.get("assets");

		assets.beginRead();

		// rig
		String rig = assets.get("rig");
		m_rig = new HavokRig(rig);

		if (!m_rig->isValid()) {
			Errorf("not an valid rig");
		}

		// animations
		{
			LuaTable animations = assets.get("animations");
			animations.beginIterator();

			Variant k, v;
			while (animations.nextIterator(k,v)) {
				AX_ASSURE(k.getTypeId() == Variant::kString);
				AX_ASSURE(v.getTypeId() == Variant::kString);

				// check duplicated
				AX_ASSURE(m_animDict.find(k.toString()) == m_animDict.end())

				m_animDict[k.toString()] = m_animations.size();
				m_animations.push_back(v.toString());
			}

			animations.endIterator();
		}

		// channels
		{
			LuaTable channels = assets.get("channels");
			channels.beginRead();

			int n = channels.getLength();
			AX_ASSURE(n < MAX_CHANNELS);

			m_numChannels = n;

			for (int i = 0; i < n; i++) {
				LuaTable channel = channels.get(i);
				initChannel(channel);
			}

			channels.endRead();
			initBoneChannelMap();
		}

		assets.endRead();
	}

	table.endRead();

	g_scriptSystem->endRead();
#endif
	// start
	for (int i = 0; i < m_numChannels; i++) {
		m_currentChannel = i;
		m_channels[i]->switchState("start", 0.1f);
	}
	m_currentChannel = -1;
}

void AnimationContext::playCycle(const std::string &anim)
{
	AX_ASSERT(m_currentChannel != -1);

	Dict<std::string,int>::const_iterator it = m_animDict.find(anim);

	if (it == m_animDict.end())
		Errorf("can't find animation to play");

	m_channels[m_currentChannel]->playCycle(it->second);
}

void AnimationContext::playCycleSync(const std::string &anim, int syncState)
{

}

void AnimationContext::playAnim(const std::string &anim)
{
	AX_ASSERT(m_currentChannel != -1);

	Dict<std::string,int>::const_iterator it = m_animDict.find(anim);

	if (it == m_animDict.end())
		Errorf("can't find animation to play");

	m_channels[m_currentChannel]->playAnim(it->second);
}

bool AnimationContext::isAnimDone(float timeleft)
{
	AX_ASSERT(m_currentChannel != -1);
	return m_channels[m_currentChannel]->isAnimDone(timeleft);
}

void AnimationContext::switchState(const std::string &state, float easyin)
{
	AX_ASSERT(m_currentChannel != -1);
	m_channels[m_currentChannel]->switchState(state, easyin);
}

void AnimationContext::step(int msec)
{
	for (int i=0; i<m_numChannels; i++) {
		m_currentChannel = i;
		m_channels[i]->step(msec);
	}

	m_currentChannel = -1;
}

#if 0
void AnimationContext::initChannel(const LuaTable &table)
{
	// read from lua first
	table.beginRead();

	int index = table.get(0);
	String name = table.get(1);
	LuaTable bones = table.get(2);

	StringSeq boneseq;

	bones.beginRead();
	int numbones = bones.getLength();
	boneseq.resize(numbones);
	for (int i = 0; i < numbones; i++) {
		boneseq[i] = bones.get(i);
	}
	bones.endRead();

	table.endRead();

	// create object
	AX_ASSURE(index < m_numChannels);
	AX_ASSURE(m_channels[index] == 0);

	m_channels[index] = new AnimationChannel(this, index, name, boneseq);
}
#endif

void AnimationContext::initBoneChannelMap()
{
	int numbones = m_rig->m_havokSkeleton->m_numBones;
	AX_ASSURE(numbones < MAX_BONES);

	// init to -1 first
	for (int i = 0; i < numbones; i++) {
		m_boneChannelMap[i] = -1;
	}

	for (int i = 0; i < numbones; i++) {
		hkaBone *bone = m_rig->m_havokSkeleton->m_bones[i];

		for (int j = 0; j < m_numChannels; j++) {
			const StringSeq &channel_bones = m_channels[j]->m_bones;

			// find bone name in channel bone def
			for (size_t k = 0; k < channel_bones.size(); k++) {
				const std::string &channel_bone = channel_bones[k];
				if (channel_bone == bone->m_name) {
					m_boneChannelMap[i] = j;
					break;
				}
			}
		}

		// if not find, find parent bone
		if (m_boneChannelMap[i] == -1) {
			m_boneChannelMap[i] = m_boneChannelMap[m_rig->m_havokSkeleton->m_parentIndices[i]];
		}

		if (m_boneChannelMap[i] == -1) {
			Errorf("can't find bone's channel");
		}
	}
}

void AnimationContext::clear()
{

}

HavokPose *AnimationContext::getPose() const
{
	return m_channels[0]->m_pose;
}

AX_END_NAMESPACE

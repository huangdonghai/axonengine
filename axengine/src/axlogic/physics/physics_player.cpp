/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

	static const float WALK_SPEED = 1.42f;
	static const float RUN_SPEED = 6.0f;

	PhysicsPlayer::PhysicsPlayer()
	{
		m_viewType = ViewType_FirstPerson;
		m_viewDist = 2.0f;

		m_moveType = MoveType_Normal;
		m_speed = WALK_SPEED;

		hkpCapsuleShape *characterShape = 0;
		{
			const hkReal totalHeight = 2.0f;
			const hkReal radius = 0.8f;
			const hkReal capsulePoint = totalHeight*0.5f - radius;
			const hkReal tolerance = 0.05f;

			hkVector4 vertexA(0, 0, capsulePoint * 2 + radius-tolerance);
			hkVector4 vertexB(0, 0, radius + tolerance);

			// Create a capsule to represent the character standing
			characterShape = new hkpCapsuleShape(vertexA, vertexB, radius);

			// Construct a Shape Phantom
			hkpSimpleShapePhantom *phantom = new hkpSimpleShapePhantom(characterShape, hkTransform::getIdentity(), hkpGroupFilter::calcFilterInfo(PhysicsWorld::LAYER_PROXY,0));
			characterShape->removeReference();

			// Add the phantom to the world
//			m_world->addPhantom(phantom);

			// Construct a character proxy
			hkpCharacterProxyCinfo cpci;
			cpci.m_position.set(10, 10, 5);
			cpci.m_staticFriction = 0.0f;
			cpci.m_dynamicFriction = 1.0f;
			cpci.m_up.set(0, 0, 1);
			cpci.m_userPlanes = 4;
			cpci.m_maxSlope = HK_REAL_PI / 3;
			cpci.m_shapePhantom = phantom;
			cpci.m_characterMass = 75;

			m_characterProxy = new hkpCharacterProxy(cpci);
			phantom->removeReference();
		}

		m_inputStates = 0;
		m_bodyAnimContext.initFromLua("Physics.Male");
	}

	PhysicsPlayer::~PhysicsPlayer()
	{

	}

	void PhysicsPlayer::setActive(bool activate) {}

	bool PhysicsPlayer::isActive() const {
		return false;
	}

	void PhysicsPlayer::setAutoDeactive(bool val) {}

	void PhysicsPlayer::setMatrix(const AffineMat &matrix) {
		m_characterProxy->setPosition(x2h(matrix.origin));
		m_viewAxis = matrix.axis;
		m_viewAngles = m_viewAxis.toAngles();
	}

	AffineMat PhysicsPlayer::getMatrix() const {
		Angles objangle = m_viewAngles;
		objangle.pitch = 0;
		objangle.roll = 0;
		return AffineMat(objangle, h2x(m_characterProxy->getPosition()));
	}

	void PhysicsPlayer::bind(PhysicsWorld *world) {
		world->m_havokWorld->addPhantom(m_characterProxy->getShapePhantom());
	}

	void PhysicsPlayer::unbind(PhysicsWorld *world) {
		world->m_havokWorld->removePhantom(m_characterProxy->getShapePhantom());
	}

	void PhysicsPlayer::updateViewAxis()
	{
		if (m_moveType == MoveType_Dead) {

		} else {
			m_viewAngles += Angles::trShort(m_userInput.angles);
			m_viewAngles.normalize180();
			m_viewAngles.pitch = Math::clamp(m_viewAngles.pitch, -89.0f, 89.0f);
			m_viewAxis.fromAngles(m_viewAngles);
		}

		// check triggers
		if (m_userInput.trigger == UserInput::Trigger_ToggleView) {
			if (m_viewType == ViewType_FirstPerson)
				m_viewType = ViewType_ThirdPerson;
			else
				m_viewType = ViewType_FirstPerson;

			return;
		}

		if (m_userInput.trigger == UserInput::Trigger_ZoomIn) {
			m_viewDist *= 0.8f;
		} else if (m_userInput.trigger == UserInput::Trigger_ZoomOut) {
			m_viewDist /= 0.8f;
		}

		m_viewDist = Math::clamp(m_viewDist, 0.5f, 10.0f);
	}

	float PhysicsPlayer::getInputScale()
	{
		return 1.0;
	}

	bool PhysicsPlayer::slideMove(bool gravity, bool stepUp, bool stepDown, bool push)
	{
		return false;
	}

	void PhysicsPlayer::waterMove(void)
	{

	}

	void PhysicsPlayer::flyMove(void)
	{

	}

	void PhysicsPlayer::airMove(void)
	{
		hkVector4 velocity = m_characterProxy->getLinearVelocity();
		hkVector4 gravity = m_world->m_havokWorld->getGravity();
		gravity.mul4(2);

		velocity.addMul4(m_frameSec, gravity);
		m_characterProxy->setLinearVelocity(velocity);
	}

	void PhysicsPlayer::walkMove(void)
	{
		Vector3 velocity = h2x(m_characterProxy->getLinearVelocity());

		float f = m_userInput.forwardmove / 127.0f;
		float r = m_userInput.rightmove / 127.0f;

		float scale = Math::rsqrt(f*f + r*r);
		f *= scale;
		r *= scale;

		Vector3 forward = m_viewAxis[0];
		Vector3 right = -m_viewAxis[1];

		forward.z = 0;
		right.z = 0;

		forward.normalize();
		right.normalize();

		if (m_userInput.buttons & UserInput::Rush) {
			m_speed = RUN_SPEED;
			m_inputStates.set(IN_RUSH);
		} else {
			m_speed = WALK_SPEED;
			m_inputStates.unset(IN_RUSH);
		}

		velocity = forward * f + right * r;
		velocity *= m_speed;

		m_characterProxy->setLinearVelocity(x2h(velocity));

		m_inputStates.unset(IN_FORWARD);
		m_inputStates.unset(IN_BACKWARD);
		m_inputStates.unset(IN_RIGHT);
		m_inputStates.unset(IN_LEFT);

		if (m_userInput.forwardmove > 10) {
			m_inputStates.set(IN_FORWARD);
		} else if (m_userInput.forwardmove < -10) {
			m_inputStates.set(IN_BACKWARD);
		}

		if (m_userInput.rightmove > 10) {
			m_inputStates.set(IN_RIGHT);
		} else if (m_userInput.rightmove < -10) {
			m_inputStates.set(IN_LEFT);
		}
	}

	void PhysicsPlayer::deadMove(void)
	{

	}

	void PhysicsPlayer::noclipMove(void)
	{

	}

	void PhysicsPlayer::spectatorMove(void)
	{

	}

	void PhysicsPlayer::ladderMove(void)
	{

	}

	void PhysicsPlayer::checkGround()
	{
		m_characterProxy->checkSupport(hkVector4(0,0,-1), m_ground);
#if 0
		Printf("SUPPORT: %d\n", m_ground.m_supportedState);
#endif
	}


	void PhysicsPlayer::runFrame(const UserInput &userinput, int msec) {
		m_userInput = userinput;
		m_frameMsec = msec;
		m_frameSec = msec / 1000.0f;

		updateViewAxis();

		checkGround();

		if (m_ground.m_supportedState == hkpSurfaceInfo::SUPPORTED) {
			walkMove();
		} else {
			airMove();
		}

		hkStepInfo si;
		si.m_deltaTime = m_frameSec;
		si.m_invDeltaTime = 1.0f / m_frameSec;

		hkVector4 gravity = m_world->m_havokWorld->getGravity();
		gravity.mul4(2);
		m_characterProxy->integrate(si, gravity);

		m_bodyAnimContext.setInputStates(m_inputStates);
		m_bodyAnimContext.step(msec);
	}

	void PhysicsPlayer::setLinearVelocity(const Vector3 &velocity)
	{
		m_characterProxy->setLinearVelocity(x2h(velocity));
	}

	AffineMat PhysicsPlayer::getThirdPerson()
	{
		Vector3 org = h2x(m_characterProxy->getPosition());
		if (m_viewType == ViewType_ThirdPerson) {
			org.z += 1.9f;
			Vector3 torg = org - m_viewAxis[0] * m_viewDist - m_viewAxis[1] * 0.5f;

			return AffineMat(m_viewAxis, torg);
		} else {
			org.z += 1.9f;
			Vector3 forward = m_viewAxis[0];
			float z = Math::abs(forward.z);
			forward.z = 0;
			forward.normalize();

			org += forward * (0.25f + z * 0.25);

			return AffineMat(m_viewAxis, org);
		}
	}

	PhysicsPlayer::PlayerAnimContext::PlayerAnimContext()
	{

	}

	PhysicsPlayer::PlayerAnimContext::~PlayerAnimContext()
	{

	}

	void PhysicsPlayer::PlayerAnimContext::setInputStates(InputStates inputstates)
	{
		setRuntime("IN_FORWARD", inputstates.isSet(PhysicsPlayer::IN_FORWARD));
		setRuntime("IN_BACKWARD", inputstates.isSet(PhysicsPlayer::IN_BACKWARD));
		setRuntime("IN_LEFT", inputstates.isSet(PhysicsPlayer::IN_LEFT));
		setRuntime("IN_RIGHT", inputstates.isSet(PhysicsPlayer::IN_RIGHT));
		setRuntime("IN_RUSH", inputstates.isSet(PhysicsPlayer::IN_RUSH));
	}

AX_END_NAMESPACE

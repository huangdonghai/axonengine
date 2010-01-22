#include "../private.h"
#if 0
//#include <axeditor/public.h>

namespace Axon{namespace Game{

using namespace PARTICLESYSTEM;

Particle::Particle()
{
	m_particle = nullptr;

	m_modelName.clear();
	m_needFree = true;

	set_modelName("models/particle/fire.ps");
}

Particle::~Particle()
{
	if (m_needFree)
	    DestroyParticleModule(m_particle);
}

void Particle::set_modelName(const String &val)
{
	if (m_modelName == val) {
		return;
	}

	if (PathUtil::getExt(val) != PARTICLE_FILE_EXTENSION)
		return; 

	m_modelName = val;

	if (m_modelName.empty())
		return;

	if (m_renderModel){
		if (m_world)
			m_world->getRenderWorld()->removeActor(m_renderModel);

		SafeDelete(m_renderModel);
	}

	m_renderModel = new renderModel(m_modelName);

	if (m_world)
		m_world->getRenderWorld()->addActor(m_renderModel);

	ParticleModelInstance *instance = dynamic_cast<ParticleModelInstance*>(m_renderModel->getInstance());

	//get particle
	if (instance)
	{
	    m_particle = instance->getParticleEffect();
		_SourceParticleCenterPos = m_particle->getCenterPos();
           // m_particle->setCenterPos(getMatrix().getOrigin());
	}
}

String Particle::get_modelName() const
{
	return m_modelName;
}

void Particle::setParticleModel(ParticleEffect *particle,bool needFree /* = true */)
{
        if (particle == NULL)
		return;

	m_needFree = needFree;
	if (m_renderModel){
		//m_world->getRenderWorld()->removeActor(m_renderModel);
		SafeDelete(m_renderModel);
		m_modelName.clear();
	}

	m_renderModel = new renderModel();

	ParticleModelInstance *particleInstance = new ParticleModelInstance(particle);

	m_renderModel->setInstance(particleInstance,true);

	//m_world->getRenderWorld()->addActor(m_renderModel);

    m_particle = particle;
	_SourceParticleCenterPos = m_particle->getCenterPos();
}

ParticleEffect *Particle::getParticleEffect()
{
	return m_particle;
}

void Particle::doThink()
{
#if 1
	if (m_renderModel && m_world)
	{
		if (getState() == Hide && m_renderModel->isPresented())
		{
			m_world->getRenderWorld()->removeActor(m_renderModel);
		}
		else if (getState() == Active && !m_renderModel->isPresented())
		{
			m_world->getRenderWorld()->addActor(m_renderModel);
		}

//			if (m_renderModel->isPresented())
//				m_world->getRenderWorld()->updateActor(m_renderModel);
	}

	if (m_particle)
		m_particle->setCenterPos(getOrigin() + _SourceParticleCenterPos);
#else
	if (!m_renderModel->isPresented()) {
		if (m_particle)
			m_particle->setCenterPos(getOrigin() + m_particle->getCenterPos());
		m_world->getRenderWorld()->addActor(m_renderModel);
	} else {
		m_world->getRenderWorld()->updateActor(m_renderModel);
	}
#endif

	//update particle
	int frametime = m_world->getFrameTime();

	if (m_particle)
	    m_particle->update(frametime / 1000.0f);
}
}}
#endif
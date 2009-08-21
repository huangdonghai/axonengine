#ifndef AX_GAME_PARTICLE_H
#define AX_GAME_PARTICLE_H

#if 0
namespace Axon{namespace Game{
	using namespace PARTICLESYSTEM;

	class Particle : public Entity
	{
	public:
		AX_DECLARE_CLASS(Particle, Entity, "Game.Effect.Particle");
		    AX_PROP(modelName)
		AX_END_CLASS()

		Particle();

		virtual ~Particle();

		// properties
		String get_modelName() const;
		void set_modelName(const String& val);

		// implement Entity
		virtual void doThink();

		void               setParticleModel(ParticleEffect* particle,bool needFree = true);
		PARTICLESYSTEM::ParticleEffect* getParticleEffect();

	protected:
        String              m_modelName;
		bool                m_needFree;
		PARTICLESYSTEM::ParticleEffect*  m_particle;
		Vector3 _SourceParticleCenterPos;
	};
}}
#endif

#endif
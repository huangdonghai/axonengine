#include "gfx_local.h"

namespace Axon {

	//--------------------------------------------------------------------------
	FloatTrack::FloatTrack( const String& name )
		: m_name(name)
	{}

	FloatTrack::~FloatTrack()
	{

	}

	int FloatTrack::numKeys()
	{
		return m_keyValues.size();
	}

	int FloatTrack::getKeyTime( int index )
	{
		return m_keyValues[index].ticks;
	}

	int FloatTrack::getKeyIndex( int ms )
	{
		// wait for implement
		return -1;
	}

	void FloatTrack::getValue( void *value, int ticks )
	{

	}

	void FloatTrack::setValue( void *value, int ticks )
	{

	}

	//--------------------------------------------------------------------------

	VectorTrack::VectorTrack( const String& name )
		: m_name(name)
	{}

	VectorTrack::~VectorTrack()
	{}

	int VectorTrack::numKeys()
	{
		return m_keyValues.size();
	}

	int VectorTrack::getKeyTime(int index)
	{
		return m_keyValues[index].ticks;
	}

	int VectorTrack::getKeyIndex(int ms)
	{
		return -1;
	}

	void VectorTrack::getValue(void *value, int ticks)
	{

	}

	void VectorTrack::setValue(void *value, int ticks)
	{

	}


	//--------------------------------------------------------------------------
	ObjAnimatable::ObjAnimatable()
	{
		m_object = dynamic_cast<Object*>(this);
		AX_ASSERT(m_object);
	}

	ObjAnimatable::~ObjAnimatable()
	{

	}

	String ObjAnimatable::getAnimName()
	{
		return m_object->get_objectName();
	}

} // namespace Axon

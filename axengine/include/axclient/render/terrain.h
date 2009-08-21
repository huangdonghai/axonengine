/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_TERRAIN
#define AX_RENDER_TERRAIN

namespace Axon { namespace Render {

	class TerrainChunk : public Entity {
	public:
	};

	class AX_API Terrain : public Entity, public IObservable {
	public:
		enum { HeightfieldSetted = 1 };

		Terrain() : Entity(kTerrain) {}
		virtual ~Terrain() {}

		virtual Kind getType() const { return Entity::kTerrain; }
		virtual void getHeightinfo(ushort_t*& datap, int& size, float& tilemeters) = 0;
	};

}} // namespace Axon::Render

#endif // end guardian


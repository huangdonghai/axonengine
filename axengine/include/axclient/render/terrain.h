/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_TERRAIN
#define AX_RENDER_TERRAIN

AX_BEGIN_NAMESPACE

	class TerrainChunk : public RenderEntity {
	public:
	};

	class AX_API RenderTerrain : public RenderEntity, public IObservable {
	public:
		enum { HeightfieldSetted = 1 };

		RenderTerrain() : RenderEntity(kTerrain) {}
		virtual ~RenderTerrain() {}

		virtual Kind getType() const { return RenderEntity::kTerrain; }
		virtual void getHeightinfo(ushort_t*& datap, int& size, float& tilemeters) = 0;
	};

AX_END_NAMESPACE

#endif // end guardian


/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_MAP_TERRAIN_H
#define AX_MAP_TERRAIN_H

namespace Axon { namespace Map {

	// forward declare
	class Chunk;
	class Zone;
	class Terrain;

	struct TerrainEvent {
		enum Type {
			HeightChanged, UpdateLayer, LayerPainted, CalcLod, GetViewed, Select,
			UpdatePrimitive, UpdateColorMapLod, UpdateNormalMapLod
		};

		Type type;
		Rect rect;
		Primitives primSeq;
		const RenderCamera* camera;
	};

	//--------------------------------------------------------------------------
	// class LayerGen
	//--------------------------------------------------------------------------

	class AlphaBlock {
	public:
		enum {
			AlphaFormat = TexFormat::A8
		};

		typedef byte_t Data[Map::ChunkPixels][Map::ChunkPixels];

		static AlphaBlock* const One;
		static AlphaBlock* const Zero;

		AlphaBlock();
		~AlphaBlock();

		const Data& getData() const;
		Data& lock();
		void unlock();
		void setAll(byte_t d);
		Texture* getTexture();

	protected:
		void updateTexture();

	private:
		byte_t m_data[Map::ChunkPixels][Map::ChunkPixels];
		TexturePtr m_texture;
		bool m_isDirty;
	};

	class AX_API LayerGen {
	public:
		LayerGen(Terrain* terrain, int layerId);
		~LayerGen();

		void update();
		void load(File* f);
		void save(File* f);
		Terrain* getTerrain() const;
		int getLayerId() const;
		Map::LayerDef* getLayerDef() const;
		Image* getColorTemplate() const;
		AlphaBlock* getBlock(const Point& index) const;
		bool isAlphaZero(const Point& index) const;
		bool isAlphaOne(const Point& index) const;
		void autoGenerate();
		Material* getDetailMat() const;
		Vector2 getDetailScale() const;
		bool isVerticalProjection() const;

		float getAlpha(int x, int y) const;
		byte_t getByteAlpha(int x, int y) const;
		void setAlpha(int x, int y, float alpha);
		void setByteAlpha(int x, int y, byte_t alpha);

		Image* copyAlpha(const Rect& r) const;
		void writeAlpha(const Rect& r, Image* image);

	protected:
		void generateBlock(const Point& index);
		void setBlock(const Point& index, AlphaBlock* block);
		void purifyBlock();

	private:
		Terrain* m_terrain;
		int m_layerId;
		int m_pixelIndexOffset;
		Image* m_colorTemplate;
		AlphaBlock** m_alphaBlocks;
		MaterialPtr m_detailMat;
	};

	inline Material* LayerGen::getDetailMat() const {
		return m_detailMat.get();
	}

	//--------------------------------------------------------------------------
	// class ColorGen
	//--------------------------------------------------------------------------

	struct ColorBlock {
		Bgra data[Map::ChunkPixels][Map::ChunkPixels];
	};

	class ColorGen {
	public:
		ColorGen(Terrain* terrain);
		~ColorGen();

	private:
		Terrain* m_terrain;
		ColorBlock* m_colorIndexes;
	};

	typedef BlockAlloc<AlphaBlock>	AlphaBlockAllocator;
	typedef BlockAlloc<ColorBlock>	ColorBlockAllocator;

	//--------------------------------------------------------------------------
	// class Chunk
	//--------------------------------------------------------------------------

	class AX_API Chunk {
	public:
		friend class Zone;
		friend class Terrain;

		Chunk();
		~Chunk();

		void initialize(Zone* zone, int x, int y);
		void finalize();

		Vector4 getChunkRect() const;

		sbyte_t getLod() const;
		const BoundingBox& getBoundingBox() const;
		BoundingRange getAltitudeRange() const;
		BoundingRange getSlopeRange() const;
		void allocatePrimitive();
		void updatePrimitive();
		void updateLayers();
		void updateColorTexture();

		void doEvent(TerrainEvent* e);

	protected:
		// event
		void onHeightChanged();
		void onCalculateLOD(TerrainEvent* e);
		void onUpdateLayer();
		void onLayerPainted();
		void onSelect(TerrainEvent* e);
		void onGetViewedPrims(TerrainEvent* e);
		void onUpdatePrimitive(TerrainEvent* e);

	private:
		union NeighborLod {
			sbyte_t lods[4];
			int i;
		};
		Terrain* m_terrain;
		Zone* m_zone;
		Point m_index;			// in global
		Rect m_tilerect;		// in global
		BoundingBox m_bbox;
		BoundingRange m_heightRange;
		BoundingRange m_slopeRange;
		bool m_heightChanged;
		MaterialPtr m_material;

		// lod
		float m_maxerror;
		float m_averageError;
		float lodErrors[Map::NumChunkLod];
		sbyte_t m_lod;
		sbyte_t m_lastLod;
		NeighborLod m_neighborLod;
		NeighborLod m_lastNeighborLod;
		bool m_needDrawLayer;
		bool m_useZonePrim;

		// slope range

		RenderChunk* m_prim;
	};

	inline sbyte_t Chunk::getLod() const {
		return m_lod;
	}

	inline const BoundingBox& Chunk::getBoundingBox() const {
		return m_bbox;
	}

	inline BoundingRange Chunk::getAltitudeRange() const {
		return m_heightRange;
	}

	inline BoundingRange Chunk::getSlopeRange() const {
		return m_slopeRange;
	}

	//--------------------------------------------------------------------------
	// class Zone
	//--------------------------------------------------------------------------

	class AX_API Zone {
	public:
		friend class Chunk;
		friend class Terrain;

		Zone();
		~Zone();

		void initialize(Terrain* terrain, int x, int y);
		void finalize();
		inline Terrain* getTerrain() { return m_terrain; }
		inline Point getZoneIndex() const { return m_index; }
		Chunk* getChunk(const Point& global_index);
		const Chunk* getChunk(const Point& global_index) const;

		inline Point chunkIndexMapToGlobal(const Point& idx) const;
		inline Point chunkIndexMapToZone(const Point& idx) const;

		void updatePrimVertexes(const Rect& rect);
		void updateNormalTexture(const Rect& rect);

		void doEvent(TerrainEvent* e);
		void addChunkToPrim(Chunk* chunk);

		Vector4 getZoneRect() const;

		Texture* getNormalTexture() const;
		Texture* getColorTexture() const;
		void setColorTexture(Texture* tex);
		void uploadColorTexture(const Point& chunk_idx, const byte_t* pixelbuf);

	protected:
		void updateColorTextureLod();
		void updateNormalTextureLod();

		void onHeightChanged(TerrainEvent* e);
		void onCalculateLOD(TerrainEvent* e);
		void onGetPrimitive(TerrainEvent* e);
		void onUpdatePrimitive(TerrainEvent* e);
		void onSelection(TerrainEvent* e);
		void forwardEventToChunks(TerrainEvent* e);

	private:
		Terrain* m_terrain;
		Point m_index;
		Rect m_tilerect;
		BoundingBox m_bbox;
		MaterialPtr m_material;
		RenderChunk* m_prim;

		Point m_chunkIndexOffset;
		float m_maxerror;
		float m_averageError;
		sbyte_t m_lod;
		sbyte_t m_lastLod;
		sbyte_t m_zonePrimLod;
		Chunk* m_chunks[Map::ZoneChunks][Map::ZoneChunks];

		TexturePtr m_normalTexture;
		TexturePtr m_colorTexture;
	};

	inline Chunk* Zone::getChunk(const Point& global_index) {
		Point index = global_index - m_chunkIndexOffset;
		AX_ASSERT(index.x >= 0 && index.x < Map::ZoneChunks);
		AX_ASSERT(index.y >= 0 && index.y < Map::ZoneChunks);
		return m_chunks[index.y][index.x];
	}

	inline const Chunk* Zone::getChunk(const Point& global_index) const {
		Point index = global_index - m_chunkIndexOffset;
		AX_ASSERT(index.x >= 0 && index.x < Map::ZoneChunks);
		AX_ASSERT(index.y >= 0 && index.y < Map::ZoneChunks);
		return m_chunks[index.y][index.x];
	}

	inline Texture* Zone::getNormalTexture() const {
		return m_normalTexture.get();
	}

	inline Texture* Zone::getColorTexture() const {
		return m_colorTexture.get();
	}

	inline void Zone::setColorTexture(Texture* tex) {
		m_colorTexture = tex;
		if (m_prim) {
			m_prim->setColorTexture(getColorTexture());
		}
	}

	//--------------------------------------------------------------------------
	// class Terrain
	//--------------------------------------------------------------------------

	class AX_API Terrain : public RenderTerrain {
	public:
		friend class Chunk;
		friend class Zone;

		Terrain();
		virtual ~Terrain();

		void init(int tiles, int tilemeters);
		void initFromXml(const String& map_name, const TiXmlElement* elem);
		void clear();
		void writeXml(File* f, int indent=0);

		// GrassManager
		GrassManager* getGrassManager() { return m_grassManager; }
		RiverManager* getRiverManager() { return m_riverManager; }

		float getTileMeters() const;
		float getMeterPixels() const;
		Rect getTileRect() const;
		Vector4 getTerrainRect() const;

		// some index
		int getChunkIndexOffset() const;
		int getNumChunks() const;
		int getZoneIndexOffset() const;
		int getNumZones() const;

		// height map
		float getHeight(int x, int y, bool local = false) const;
		float getOldHeight(int x, int y, bool local = false) const;
		float getPixelHeight(int x, int y) const;
		float getHeightByPos(const Vector3& pos) const;
		void setHeight(int x, int y, float h, bool local = false);
		Image* copyHeight(const Rect& rect) const;
		Image* copyOldHeight(const Rect& rect) const;
		void writeHeight(const Rect& rect, Image* image);
		void writeOldHeight(const Rect& rect, Image* image);
		Image* copyFloatHeight(int size) const;	// need be freed by caller

		sbyte_t getChunkLod(const Point& global_index) const;
		Zone* getZone(const Point& global_index);
		const Zone* getZone(const Point& global_index) const;
		Chunk* getChunk(const Point& global_index);
		const Chunk* getChunk(const Point& global_index) const;
		const Chunk* getPosChunk(const Vector3& pos) const;

		void doHeightChanged(const Rect& tilerect);
		void doUpdateNormalTextureLod(const Rect& tilerect);
		void doUpdateLayer(const Rect& pixelrect);
		void doLayerPainted(const Rect& pixelrect);
		void doUpdateColorTextureLod(const Rect& pixelrect);
		void doCalculateLOD();

		void doEvent(TerrainEvent* e);
		void doSelect(const RenderCamera& camera);

		Primitives getPrimsByCircle(float x, float y, float radius);

		// surface and material
		Map::MaterialDef* getMaterialDef() const;
		void setMaterialDef(Map::MaterialDef* matdef, bool undoable = true);
		AlphaBlock* allocAlphaBlock();
		void freeAlphaBlock(AlphaBlock*& block);
		Image* getSlopeImage() const;
		Image* copySlopeImage(int size) const;	// need be freed by caller
		byte_t getSlope(int x, int y, bool local = false) const;
		float getPixelSlope(int x, int y) const;
		void setSlope(int x, int y, byte_t slope, bool local = false);
		void generateZoneColor(bool doprogress = false);
		void generateLayerAlpha(bool doprogress = false, int id = -1);
		int getNumLayer() const;
		LayerGen* getLayerGen(int layer) const;
		LayerGen* getLayerGenById(int id) const;
		MaterialPtr getLayerMat(int layer) const;
		AlphaBlock* getAlphaBlock(int layer, const Point& index) const;

		// implement renderActor
		virtual BoundingBox getLocalBoundingBox() { return m_bbox; }
		virtual BoundingBox getBoundingBox() { return m_bbox; }
		virtual Kind getType() const { return RenderEntity::kTerrain; }
		virtual void doUpdate(QueuedScene* qscene);

		virtual void issueToQueue(QueuedScene* qscene);

		virtual Primitives getAllPrimitives();
#if 0
		virtual Primitives getViewedPrimitives();
		virtual Primitives getLightedPrimitives(Render::QueuedLight* light);
#endif

		// implement renderTerrain
		virtual void getHeightinfo(ushort_t*& datap, int& size, float& tilemeters);

	protected:
		bool loadColorTexture(const String& map_name);

	private:
		Image* m_heightmap;
		Image* m_oldHeightmap;
		Image* m_slopeImage;
		Image* m_colorImage;
		Texture* m_normalTex;
		bool m_isHeightDirty;
		ushort_t* m_heighData;
		Vector2 m_heightBound;
		Rect m_tilerect;
		int m_tiles;
		int m_tilemeters;
		BoundingBox m_bbox;
		int m_zoneCount;
		int m_zoneIndexOffset;
		int m_chunkIndexOffset;
		int m_tileOffset;
		int m_terrainZones;
		Zone** m_zones;

		Vector3 m_lastViewOrigin;
		bool m_heightDirtyLastView;

		// terrain materials
		typedef Dict<int,LayerGen*> LayerGenHash;

		Map::MaterialDef* m_materialDef;
		AlphaBlockAllocator m_alphaAllocator;
		ColorBlockAllocator m_colorAllocator;
		int m_numLayerGens;
		LayerGen* m_layerGens[Map::MaxLayers];
		LayerGenHash m_layerGenHash;
		ColorGen* m_colorGen;

		GrassManager* m_grassManager;
		RiverManager* m_riverManager;
	};

	inline float Terrain::getTileMeters() const {
		return m_tilemeters;
	}

	inline float Terrain::getMeterPixels() const {
		return Map::TilePixels / m_tilemeters;
	}


	inline Rect Terrain::getTileRect() const {
		return m_tilerect;
	}

	inline Vector4 Terrain::getTerrainRect() const {
		Vector4 result(m_tilerect.x, m_tilerect.y, m_tilerect.width, m_tilerect.height);

		return result * m_tilemeters;
	}


	inline int Terrain::getChunkIndexOffset() const {
		return m_chunkIndexOffset;
	}

	inline int Terrain::getNumChunks() const {
		return m_chunkIndexOffset * 2;
	}

	inline int Terrain::getZoneIndexOffset() const {
		return m_zoneIndexOffset;
	}

	inline int Terrain::getNumZones() const {
		return m_zoneIndexOffset * 2;
	}

	inline float Terrain::getHeight(int x, int y, bool local) const {
		if (!local) {
			x += m_tileOffset; y += m_tileOffset;
		}
		x = Math::clamp(x, 0, m_tiles-1);
		y = Math::clamp(y, 0, m_tiles-1);

		ushort_t h = m_heighData[y*m_tiles + x];
		return (float)h / std::numeric_limits<ushort_t>::max() * (m_heightBound.y - m_heightBound.x) + m_heightBound.x;
	}

	inline float Terrain::getOldHeight(int x, int y, bool local) const {
		const ushort_t* oldheightdata = (const ushort_t*)m_oldHeightmap->getData(0);

		if (!local) {
			x += m_tileOffset; y += m_tileOffset;
		}
		x = Math::clamp(x, 0, m_tiles-1);
		y = Math::clamp(y, 0, m_tiles-1);

		ushort_t h = oldheightdata[y*m_tiles + x];
		return (float)h / std::numeric_limits<ushort_t>::max() * (m_heightBound.y - m_heightBound.x) + m_heightBound.x;
	}

	inline float Terrain::getPixelHeight(int x, int y) const {
		float fx = x; float fy = y;
		fx /= Map::TilePixels; fy /= Map::TilePixels;

		int x0 = floorf(fx); int x1 = ceilf(fx);
		int y0 = floorf(fy); int y1 = ceilf(fy);

		float xfrac = fx - x0;
		float yfrac = fy - y0;
		float p00 = getHeight(x0, y0);
		float p01 = getHeight(x0, y1);
		float p10 = getHeight(x1, y0);
		float p11 = getHeight(x1, y1);

		float p0 = (p01 - p00) * yfrac + p00;
		float p1 = (p11 - p10) * yfrac + p10;
		float p = (p1 - p0) * xfrac + p0;
		return p;
	}

	inline float Terrain::getHeightByPos(const Vector3& pos) const {
		float fx = pos.x / m_tilemeters; float fy = pos.y / m_tilemeters;

		int x0 = floorf(fx); int x1 = ceilf(fx);
		int y0 = floorf(fy); int y1 = ceilf(fy);

		float xfrac = fx - x0;
		float yfrac = fy - y0;
		float p00 = getHeight(x0, y0);
		float p01 = getHeight(x0, y1);
		float p10 = getHeight(x1, y0);
		float p11 = getHeight(x1, y1);

		float p0 = (p01 - p00) * yfrac + p00;
		float p1 = (p11 - p10) * yfrac + p10;
		float p = (p1 - p0) * xfrac + p0;
		return p;
	}

	inline void Terrain::setHeight(int x, int y, float h, bool local) {
		if (!local) {
			x += m_tileOffset; y += m_tileOffset;
		}
		if (x < 0 || x >= m_tiles) return;
		if (y < 0 || y >= m_tiles) return;

		h = (h - m_heightBound.x) / (m_heightBound.y - m_heightBound.x);
		h = Math::clamp(h, 0.0f, 1.0f);

		ushort_t uh = (h * std::numeric_limits<ushort_t>::max() + 0.5f);
		m_heighData[y*m_tiles + x] = uh;
	}

	inline byte_t Terrain::getSlope(int x, int y, bool local) const {
		if (!local) {
			x += m_tileOffset; y += m_tileOffset;
		}
		x = Math::clamp(x, 0, m_tiles-1);
		y = Math::clamp(y, 0, m_tiles-1);

		return *m_slopeImage->getPixel(0, x, y);
	}

	inline float Terrain::getPixelSlope(int x, int y) const {
		float fx = x; float fy = y;
		fx /= Map::TilePixels; fy /= Map::TilePixels;

		int x0 = floorf(fx); int x1 = ceilf(fx);
		int y0 = floorf(fy); int y1 = ceilf(fy);

		float xfrac = fx - x0;
		float yfrac = fy - y0;
		float p00 = getSlope(x0, y0);
		float p01 = getSlope(x0, y1);
		float p10 = getSlope(x1, y0);
		float p11 = getSlope(x1, y1);

		float p0 = (p01 - p00) * yfrac + p00;
		float p1 = (p11 - p10) * yfrac + p10;
		float p = (p1 - p0) * xfrac + p0;
		return p;
	}


	inline void Terrain::setSlope(int x, int y, byte_t slope, bool local) {
		if (!local) {
			x += m_tileOffset; y += m_tileOffset;
		}
		x = Math::clamp(x, 0, m_tiles-1);
		y = Math::clamp(y, 0, m_tiles-1);

		m_slopeImage->setPixel(x, y, &slope);
	}


	inline sbyte_t Terrain::getChunkLod(const Point& global_index) const {
		const Chunk* c = getChunk(global_index);
		if (c)
			return c->getLod();

		return 0;
	}

	// if out of bound, return null
	inline Zone* Terrain::getZone(const Point& global_index) {
		if (global_index.x < -m_zoneIndexOffset)
			return nullptr;
		if (global_index.x >= m_zoneIndexOffset)
			return nullptr;
		if (global_index.y < -m_zoneIndexOffset)
			return nullptr;
		if (global_index.y >= m_zoneIndexOffset)
			return nullptr;

		Point index = global_index + m_zoneIndexOffset;
		int i = index.y * m_terrainZones + index.x;

		return m_zones[i];
	}

	inline const Zone* Terrain::getZone(const Point& global_index) const {
		if (global_index.x < -m_zoneIndexOffset)
			return nullptr;
		if (global_index.x >= m_zoneIndexOffset)
			return nullptr;
		if (global_index.y < -m_zoneIndexOffset)
			return nullptr;
		if (global_index.y >= m_zoneIndexOffset)
			return nullptr;

		Point index = global_index + m_zoneIndexOffset;
		int i = index.y * m_terrainZones + index.x;

		return m_zones[i];
	}

	inline Chunk* Terrain::getChunk(const Point& global_index) {
		if (global_index.x < -m_chunkIndexOffset)
			return nullptr;
		if (global_index.x >= m_chunkIndexOffset)
			return nullptr;
		if (global_index.y < -m_chunkIndexOffset)
			return nullptr;
		if (global_index.y >= m_chunkIndexOffset)
			return nullptr;

		Point index = (global_index + m_chunkIndexOffset) / Map::ZoneChunks;
		index -= m_zoneIndexOffset;
		Zone* z = getZone(index);

		if (z == nullptr)
			return nullptr;

		return z->getChunk(global_index);
	}

	inline const Chunk* Terrain::getChunk(const Point& global_index) const {
		if (global_index.x < -m_chunkIndexOffset)
			return nullptr;
		if (global_index.x >= m_chunkIndexOffset)
			return nullptr;
		if (global_index.y < -m_chunkIndexOffset)
			return nullptr;
		if (global_index.y >= m_chunkIndexOffset)
			return nullptr;

#if 0
		Point index = (global_index + m_chunkIndexOffset) / Map::ZoneChunks;
#else
		Point index = global_index + m_chunkIndexOffset; // 11.17, zsh modify
		index.x /= Map::ZoneChunks;
		index.y /= Map::ZoneChunks;
#endif

		index -= m_zoneIndexOffset;
		const Zone* z = getZone(index);

		if (z == nullptr)
			return nullptr;

		return z->getChunk(global_index);
	}

	inline const Chunk* Terrain::getPosChunk(const Vector3& pos) const {
		Point index;

		index.x = floorf(pos.x / (Map::ChunkTiles * m_tilemeters));
		index.y = floorf(pos.y / (Map::ChunkTiles * m_tilemeters));

		return getChunk(index);
	}

	inline Map::MaterialDef* Terrain::getMaterialDef() const {
		return m_materialDef;
	}

	inline AlphaBlock* Terrain::allocAlphaBlock() {
		return m_alphaAllocator.alloc();
	}

	inline void Terrain::freeAlphaBlock(AlphaBlock*& block) {
		return m_alphaAllocator.free(block);
	}

	inline int Terrain::getNumLayer() const {
		return m_numLayerGens;
	}

	inline LayerGen* Terrain::getLayerGen(int layer) const {
		if (layer >= m_numLayerGens)
			return nullptr;

		return m_layerGens[layer];
	}

	inline LayerGen* Terrain::getLayerGenById(int id) const {
		for (int i = 0; i < m_numLayerGens; i++) {
			if (m_layerGens[i]->getLayerId() == id)
				return m_layerGens[i];
		}

		return nullptr;
	}

	inline MaterialPtr Terrain::getLayerMat(int layer) const {
		if (layer >= m_numLayerGens)
			return MaterialPtr();

		return m_layerGens[layer]->getDetailMat();
	}

	inline AlphaBlock* Terrain::getAlphaBlock(int layer, const Point& index) const {
		if (layer >= m_numLayerGens)
			return nullptr;

		return m_layerGens[layer]->getBlock(index);
	}

}} // namespace Axon::Map

#endif // AX_EDITOR_TERRAIN_H


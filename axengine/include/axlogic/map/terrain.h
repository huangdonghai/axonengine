/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_MAP_TERRAIN_H
#define AX_MAP_TERRAIN_H

AX_BEGIN_NAMESPACE

	// forward declare
	class MapChunk;
	class MapZone;
	class MapTerrain;

	struct MapEvent {
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
	// class MapLayerGen
	//--------------------------------------------------------------------------

	class MapAlphaBlock {
	public:
		enum {
			AlphaFormat = TexFormat::A8
		};

		typedef byte_t Data[Map::ChunkPixels][Map::ChunkPixels];

		static MapAlphaBlock* const One;
		static MapAlphaBlock* const Zero;

		MapAlphaBlock();
		~MapAlphaBlock();

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

	class AX_API MapLayerGen {
	public:
		MapLayerGen(MapTerrain* terrain, int layerId);
		~MapLayerGen();

		void update();
		void load(File* f);
		void save(File* f);
		MapTerrain* getTerrain() const;
		int getLayerId() const;
		MapLayerDef* getLayerDef() const;
		Image* getColorTemplate() const;
		MapAlphaBlock* getBlock(const Point& index) const;
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
		void setBlock(const Point& index, MapAlphaBlock* block);
		void purifyBlock();

	private:
		MapTerrain* m_terrain;
		int m_layerId;
		int m_pixelIndexOffset;
		Image* m_colorTemplate;
		MapAlphaBlock** m_alphaBlocks;
		MaterialPtr m_detailMat;
	};

	inline Material* MapLayerGen::getDetailMat() const {
		return m_detailMat.get();
	}

	//--------------------------------------------------------------------------
	// class MapColorGen
	//--------------------------------------------------------------------------

	struct MapColorBlock {
		Bgra data[Map::ChunkPixels][Map::ChunkPixels];
	};

	class MapColorGen {
	public:
		MapColorGen(MapTerrain* terrain);
		~MapColorGen();

	private:
		MapTerrain* m_terrain;
		MapColorBlock* m_colorIndexes;
	};

	typedef BlockAlloc<MapAlphaBlock>	AlphaBlockAllocator;
	typedef BlockAlloc<MapColorBlock>	ColorBlockAllocator;

	//--------------------------------------------------------------------------
	// class MapChunk
	//--------------------------------------------------------------------------

	class AX_API MapChunk {
	public:
		friend class MapZone;
		friend class MapTerrain;

		MapChunk();
		~MapChunk();

		void initialize(MapZone* zone, int x, int y);
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

		void doEvent(MapEvent* e);

	protected:
		// event
		void onHeightChanged();
		void onCalculateLOD(MapEvent* e);
		void onUpdateLayer();
		void onLayerPainted();
		void onSelect(MapEvent* e);
		void onGetViewedPrims(MapEvent* e);
		void onUpdatePrimitive(MapEvent* e);

	private:
		union NeighborLod {
			sbyte_t lods[4];
			int i;
		};
		MapTerrain* m_terrain;
		MapZone* m_zone;
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

		ChunkPrim* m_prim;
	};

	inline sbyte_t MapChunk::getLod() const {
		return m_lod;
	}

	inline const BoundingBox& MapChunk::getBoundingBox() const {
		return m_bbox;
	}

	inline BoundingRange MapChunk::getAltitudeRange() const {
		return m_heightRange;
	}

	inline BoundingRange MapChunk::getSlopeRange() const {
		return m_slopeRange;
	}

	//--------------------------------------------------------------------------
	// class MapZone
	//--------------------------------------------------------------------------

	class AX_API MapZone {
	public:
		friend class MapChunk;
		friend class MapTerrain;

		MapZone();
		~MapZone();

		void initialize(MapTerrain* terrain, int x, int y);
		void finalize();
		inline MapTerrain* getTerrain() { return m_terrain; }
		inline Point getZoneIndex() const { return m_index; }
		MapChunk* getChunk(const Point& global_index);
		const MapChunk* getChunk(const Point& global_index) const;

		inline Point chunkIndexMapToGlobal(const Point& idx) const;
		inline Point chunkIndexMapToZone(const Point& idx) const;

		void updatePrimVertexes(const Rect& rect);
		void updateNormalTexture(const Rect& rect);

		void doEvent(MapEvent* e);
		void addChunkToPrim(MapChunk* chunk);

		Vector4 getZoneRect() const;

		Texture* getNormalTexture() const;
		Texture* getColorTexture() const;
		void setColorTexture(Texture* tex);
		void uploadColorTexture(const Point& chunk_idx, const byte_t* pixelbuf);

	protected:
		void updateColorTextureLod();
		void updateNormalTextureLod();

		void onHeightChanged(MapEvent* e);
		void onCalculateLOD(MapEvent* e);
		void onGetPrimitive(MapEvent* e);
		void onUpdatePrimitive(MapEvent* e);
		void onSelection(MapEvent* e);
		void forwardEventToChunks(MapEvent* e);

	private:
		MapTerrain* m_terrain;
		Point m_index;
		Rect m_tilerect;
		BoundingBox m_bbox;
		MaterialPtr m_material;
		ChunkPrim* m_prim;

		Point m_chunkIndexOffset;
		float m_maxerror;
		float m_averageError;
		sbyte_t m_lod;
		sbyte_t m_lastLod;
		sbyte_t m_zonePrimLod;
		MapChunk* m_chunks[Map::ZoneChunks][Map::ZoneChunks];

		TexturePtr m_normalTexture;
		TexturePtr m_colorTexture;
	};

	inline MapChunk* MapZone::getChunk(const Point& global_index) {
		Point index = global_index - m_chunkIndexOffset;
		AX_ASSERT(index.x >= 0 && index.x < Map::ZoneChunks);
		AX_ASSERT(index.y >= 0 && index.y < Map::ZoneChunks);
		return m_chunks[index.y][index.x];
	}

	inline const MapChunk* MapZone::getChunk(const Point& global_index) const {
		Point index = global_index - m_chunkIndexOffset;
		AX_ASSERT(index.x >= 0 && index.x < Map::ZoneChunks);
		AX_ASSERT(index.y >= 0 && index.y < Map::ZoneChunks);
		return m_chunks[index.y][index.x];
	}

	inline Texture* MapZone::getNormalTexture() const {
		return m_normalTexture.get();
	}

	inline Texture* MapZone::getColorTexture() const {
		return m_colorTexture.get();
	}

	inline void MapZone::setColorTexture(Texture* tex) {
		m_colorTexture = tex;
		if (m_prim) {
			m_prim->setColorTexture(getColorTexture());
		}
	}

	//--------------------------------------------------------------------------
	// class MapTerrain
	//--------------------------------------------------------------------------

	class AX_API MapTerrain : public RenderTerrain {
	public:
		friend class MapChunk;
		friend class MapZone;

		MapTerrain();
		virtual ~MapTerrain();

		void init(int tiles, int tilemeters);
		void initFromXml(const String& map_name, const TiXmlElement* elem);
		void clear();
		void writeXml(File* f, int indent=0);

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
		MapZone* getZone(const Point& global_index);
		const MapZone* getZone(const Point& global_index) const;
		MapChunk* getChunk(const Point& global_index);
		const MapChunk* getChunk(const Point& global_index) const;
		const MapChunk* getPosChunk(const Vector3& pos) const;

		void doHeightChanged(const Rect& tilerect);
		void doUpdateNormalTextureLod(const Rect& tilerect);
		void doUpdateLayer(const Rect& pixelrect);
		void doLayerPainted(const Rect& pixelrect);
		void doUpdateColorTextureLod(const Rect& pixelrect);
		void doCalculateLOD();

		void doEvent(MapEvent* e);
		void doSelect(const RenderCamera& camera);

		Primitives getPrimsByCircle(float x, float y, float radius);

		// surface and material
		MapMaterialDef* getMaterialDef() const;
		void setMaterialDef(MapMaterialDef* matdef, bool undoable = true);
		MapAlphaBlock* allocAlphaBlock();
		void freeAlphaBlock(MapAlphaBlock*& block);
		Image* getSlopeImage() const;
		Image* copySlopeImage(int size) const;	// need be freed by caller
		byte_t getSlope(int x, int y, bool local = false) const;
		float getPixelSlope(int x, int y) const;
		void setSlope(int x, int y, byte_t slope, bool local = false);
		void generateZoneColor(bool doprogress = false);
		void generateLayerAlpha(bool doprogress = false, int id = -1);
		int getNumLayer() const;
		MapLayerGen* getLayerGen(int layer) const;
		MapLayerGen* getLayerGenById(int id) const;
		MaterialPtr getLayerMat(int layer) const;
		MapAlphaBlock* getAlphaBlock(int layer, const Point& index) const;

		// implement renderActor
		virtual BoundingBox getLocalBoundingBox() { return m_bbox; }
		virtual BoundingBox getBoundingBox() { return m_bbox; }
		virtual Kind getType() const { return RenderEntity::kTerrain; }
		virtual void frameUpdate(QueuedScene* qscene);

		virtual void issueToQueue(QueuedScene* qscene);

#if 0
		virtual Primitives getAllPrimitives();
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
		MapZone** m_zones;

		Vector3 m_lastViewOrigin;
		bool m_heightDirtyLastView;

		// terrain materials
		typedef Dict<int,MapLayerGen*> LayerGenHash;

		MapMaterialDef* m_materialDef;
		AlphaBlockAllocator m_alphaAllocator;
		ColorBlockAllocator m_colorAllocator;
		int m_numLayerGens;
		MapLayerGen* m_layerGens[Map::MaxLayers];
		LayerGenHash m_layerGenHash;
		MapColorGen* m_colorGen;
	};

	inline float MapTerrain::getTileMeters() const {
		return m_tilemeters;
	}

	inline float MapTerrain::getMeterPixels() const {
		return Map::TilePixels / m_tilemeters;
	}


	inline Rect MapTerrain::getTileRect() const {
		return m_tilerect;
	}

	inline Vector4 MapTerrain::getTerrainRect() const {
		Vector4 result(m_tilerect.x, m_tilerect.y, m_tilerect.width, m_tilerect.height);

		return result * m_tilemeters;
	}


	inline int MapTerrain::getChunkIndexOffset() const {
		return m_chunkIndexOffset;
	}

	inline int MapTerrain::getNumChunks() const {
		return m_chunkIndexOffset * 2;
	}

	inline int MapTerrain::getZoneIndexOffset() const {
		return m_zoneIndexOffset;
	}

	inline int MapTerrain::getNumZones() const {
		return m_zoneIndexOffset * 2;
	}

	inline float MapTerrain::getHeight(int x, int y, bool local) const {
		if (!local) {
			x += m_tileOffset; y += m_tileOffset;
		}
		x = Math::clamp(x, 0, m_tiles-1);
		y = Math::clamp(y, 0, m_tiles-1);

		ushort_t h = m_heighData[y*m_tiles + x];
		return (float)h / std::numeric_limits<ushort_t>::max() * (m_heightBound.y - m_heightBound.x) + m_heightBound.x;
	}

	inline float MapTerrain::getOldHeight(int x, int y, bool local) const {
		const ushort_t* oldheightdata = (const ushort_t*)m_oldHeightmap->getData(0);

		if (!local) {
			x += m_tileOffset; y += m_tileOffset;
		}
		x = Math::clamp(x, 0, m_tiles-1);
		y = Math::clamp(y, 0, m_tiles-1);

		ushort_t h = oldheightdata[y*m_tiles + x];
		return (float)h / std::numeric_limits<ushort_t>::max() * (m_heightBound.y - m_heightBound.x) + m_heightBound.x;
	}

	inline float MapTerrain::getPixelHeight(int x, int y) const {
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

	inline float MapTerrain::getHeightByPos(const Vector3& pos) const {
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

	inline void MapTerrain::setHeight(int x, int y, float h, bool local) {
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

	inline byte_t MapTerrain::getSlope(int x, int y, bool local) const {
		if (!local) {
			x += m_tileOffset; y += m_tileOffset;
		}
		x = Math::clamp(x, 0, m_tiles-1);
		y = Math::clamp(y, 0, m_tiles-1);

		return *m_slopeImage->getPixel(0, x, y);
	}

	inline float MapTerrain::getPixelSlope(int x, int y) const {
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


	inline void MapTerrain::setSlope(int x, int y, byte_t slope, bool local) {
		if (!local) {
			x += m_tileOffset; y += m_tileOffset;
		}
		x = Math::clamp(x, 0, m_tiles-1);
		y = Math::clamp(y, 0, m_tiles-1);

		m_slopeImage->setPixel(x, y, &slope);
	}


	inline sbyte_t MapTerrain::getChunkLod(const Point& global_index) const {
		const MapChunk* c = getChunk(global_index);
		if (c)
			return c->getLod();

		return 0;
	}

	// if out of bound, return null
	inline MapZone* MapTerrain::getZone(const Point& global_index) {
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

	inline const MapZone* MapTerrain::getZone(const Point& global_index) const {
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

	inline MapChunk* MapTerrain::getChunk(const Point& global_index) {
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
		MapZone* z = getZone(index);

		if (z == nullptr)
			return nullptr;

		return z->getChunk(global_index);
	}

	inline const MapChunk* MapTerrain::getChunk(const Point& global_index) const {
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
		const MapZone* z = getZone(index);

		if (z == nullptr)
			return nullptr;

		return z->getChunk(global_index);
	}

	inline const MapChunk* MapTerrain::getPosChunk(const Vector3& pos) const {
		Point index;

		index.x = floorf(pos.x / (Map::ChunkTiles * m_tilemeters));
		index.y = floorf(pos.y / (Map::ChunkTiles * m_tilemeters));

		return getChunk(index);
	}

	inline MapMaterialDef* MapTerrain::getMaterialDef() const {
		return m_materialDef;
	}

	inline MapAlphaBlock* MapTerrain::allocAlphaBlock() {
		return m_alphaAllocator.alloc();
	}

	inline void MapTerrain::freeAlphaBlock(MapAlphaBlock*& block) {
		return m_alphaAllocator.free(block);
	}

	inline int MapTerrain::getNumLayer() const {
		return m_numLayerGens;
	}

	inline MapLayerGen* MapTerrain::getLayerGen(int layer) const {
		if (layer >= m_numLayerGens)
			return nullptr;

		return m_layerGens[layer];
	}

	inline MapLayerGen* MapTerrain::getLayerGenById(int id) const {
		for (int i = 0; i < m_numLayerGens; i++) {
			if (m_layerGens[i]->getLayerId() == id)
				return m_layerGens[i];
		}

		return nullptr;
	}

	inline MaterialPtr MapTerrain::getLayerMat(int layer) const {
		if (layer >= m_numLayerGens)
			return MaterialPtr();

		return m_layerGens[layer]->getDetailMat();
	}

	inline MapAlphaBlock* MapTerrain::getAlphaBlock(int layer, const Point& index) const {
		if (layer >= m_numLayerGens)
			return nullptr;

		return m_layerGens[layer]->getBlock(index);
	}

AX_END_NAMESPACE

#endif // AX_EDITOR_TERRAIN_H


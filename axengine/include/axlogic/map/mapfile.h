/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_MAP_FILE_H
#define AX_MAP_FILE_H

AX_BEGIN_NAMESPACE

namespace Map {
	enum Constant {
		ChunkTilesBits = 5,
		ChunkTiles = 1 << ChunkTilesBits,	// 32
		ChunkTilesMask = ChunkTiles - 1,		// 31
		ChunkTilesP1 = ChunkTiles + 1,
		ChunkTilesP3 = ChunkTiles + 3,		// 35
		NumChunkLod = ChunkTilesBits,		// 5

		ZoneTilesBits = 9,
		ZoneTiles = 1 << ZoneTilesBits,	// 512
		ZoneChunks = ZoneTiles / ChunkTiles,

		TilePixels = 4,
		ChunkPixels = ChunkTiles*TilePixels,	// 32*4 = 128
		ZonePixels = ZoneTiles*TilePixels,		// 512*4 = 2048

		MaxTiles = 4 * 1024,
		MaxZones = MaxTiles / ZoneTiles,

		MaxDetails = 8,
		MaxLayers = 16,
		MaxChunkLayers = 4,

		AlphaZeroIndex = 0,
		AlphaOneIndex = 1
	};
}

class AX_API MapLayerDef
{
	friend class MapMaterialDef;

public:
	const int id;
	String name;
	SurfaceType surfaceType;
	Rgba color;

	// base image
	String baseImage;
	bool genBaseFromDetail;

	// detail info
	bool isUseDetail;
	String detailMat;
	Vector2 uvScale;
	bool isVerticalProjection;

	// autogen info
	bool isAutoGen;
	Vector2 altitudeRange;
	Vector2 slopeRange;

private:
	MapLayerDef(int _id);
	~MapLayerDef();

	MapLayerDef *clone() const;
};

class AX_API MapMaterialDef
{
public:
	MapMaterialDef();
	~MapMaterialDef();

	int getNumLayers() const;
	MapLayerDef *getLayerDef(int idx) const;
	MapLayerDef *findLayerDefById(int id) const;
	MapLayerDef *createLayerDef();
	void deleteLayerDef(MapLayerDef *l);
	void moveUpLayerDef(MapLayerDef *l);
	void moveDownLayerDef(MapLayerDef *l);

	// set layer for index. the layer must create by this materialDef, or
	// the id will be corrupted
	void setLayer(int idx, MapLayerDef *l);
	MapMaterialDef *clone() const;

	void parseXml(const TiXmlElement *node);
	void writeToFile(File *f, int indent=0);

protected:
	int findLayerIndex(MapLayerDef *l);

private:
	int m_numLayers;
	MapLayerDef *m_layerDefs[Map::MaxLayers];
	int m_maxLayerId;
};

class AX_API MapEnvDef : public Object
{
public:
	AX_DECLARE_CLASS(MapEnvDef, Object)
		AX_SIMPLEPROP(fogColor)
		AX_SIMPLEPROP(fogDensity)
		AX_SIMPLEPROP(haveSkyBox)
		AX_SIMPLEPROP(textureSkyBox)
		AX_SIMPLEPROP(haveSkyModel)
		AX_SIMPLEPROP(skyModel)
		AX_SIMPLEPROP(haveGlobalLight)
		AX_SIMPLEPROP(castShadow)
		AX_SIMPLEPROP(sunColor)
		AX_SIMPLEPROP(sunColorX)
		AX_SIMPLEPROP(skyColor)
		AX_SIMPLEPROP(skyColorX)
		AX_SIMPLEPROP(envColor)
		AX_SIMPLEPROP(envColorX)
		AX_SIMPLEPROP(latitude)
		AX_SIMPLEPROP(dayTime)
		AX_SIMPLEPROP(haveOcean)
		AX_SIMPLEPROP(materialOcean)
		AX_SIMPLEPROP(shoreSize)
		AX_SIMPLEPROP(oceanFogColor)
		AX_SIMPLEPROP(oceanFogDensity)
		AX_SIMPLEPROP(caustics)
	AX_END_CLASS()

	MapEnvDef();
	~MapEnvDef();

	// member method
	void parseXml(const TiXmlElement *node);
	void writeToFile(File *f, int indent=0);


public:		// let public just because for easy use
	// fog
	Color3 m_fogColor;
	float m_fogDensity;
	float m_viewDist;

	// sky
	bool m_haveSkyBox;
	String m_textureSkyBox;
	bool m_haveSkyModel;
	String m_skyModel;

	// lights
	bool m_haveGlobalLight;
	bool m_castShadow;
	Color3 m_sunColor;
	float m_sunColorX;
	float m_latitude;
	float m_dayTime;
	Color3 m_skyColor;		// for sky ambient
	float m_skyColorX;
	Color3 m_envColor;
	float m_envColorX;

	// ocean
	bool m_haveOcean;
	String m_materialOcean;
	float m_shoreSize;
	Color3 m_oceanFogColor;
	float m_oceanFogDensity;
	bool m_caustics;
};

AX_END_NAMESPACE

#endif // AX_MAP_FILE_H

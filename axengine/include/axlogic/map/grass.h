/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_MAP_GRASS_H
#define AX_MAP_GRASS_H

namespace Axon { namespace Map {

	class Terrain;

	//--------------------------------------------------------------------------
	// class GrassDef
	//--------------------------------------------------------------------------

	class AX_API GrassDef
	{
	public:
		GrassDef()
			: m_type(Criss)
			, m_incidence(10)
			, m_mat(NULL)
			, m_mesh(NULL)
			, m_tex(NULL)
			, m_model(NULL)
			, m_width(0.5f)
			, m_height(1.0f)
			, m_minScale(0.5f)
			, m_maxScale(2.0f)
			, m_color(0, 255, 0)
			, m_name("GrassDef")
			, m_transfer(0.0f, 0.0f, 0.0f)
		{

		}

		~GrassDef()
		{
			/*SafeDelete(m_mat);
			SafeDelete(m_mesh);
			SafeDelete(m_tex);
			SafeDelete(m_model);*/

			m_modelVertexes.clear();
			m_modelIndexes.clear();
		}

		// 十字形, 三角交叉形, 星形, 模型
		enum Type{Criss, Triangle, Star, Model}; 

		void setType(Type type) { m_type = type;}
		Type getType() { return m_type;}

		void setIncidence(int incidence) { m_incidence = incidence;}
		int getIncidence() { return m_incidence;}

		void setMinScale(float scale) { m_minScale = scale; }
		float getMinScale() { return m_minScale; }
		void setMaxScale(float scale) { m_maxScale = scale; }
		float getMaxScale() { return m_maxScale; }

		void setWidth(float width) { m_width = width; }
		float getWidth() { return m_width; }
		void setHeight(float height) { m_height = height; }
		float getHeight() { return m_height; }

		void setMaterial(Material* mat) { m_mat = mat;}
		Material* getMaterial() { return m_mat.get();}

		void setTextureName(const String& texName);
		String getTextureName()	{ return m_texName; }
		Texture* getTexture() { return m_tex.get(); }

		RenderMesh* getMesh() { return m_mesh; };
		void setMesh(RenderMesh* mesh) { m_mesh = mesh; }

		String getName() { return m_name; }
		void setName(const String& name) { m_name = name; }

		Rgba getColor() { return m_color; }
		void setColor(const Rgba& color) { m_color = color; }

		void setModelFilename(const String& name);
		const String& getModelFilename() { return m_modelFilename; }

		HavokModel* getModel() { return (m_type == Model) ? NULL : m_model; };
		const std::vector<Vertex>& getModelVertexes() { return m_modelVertexes; }
		const std::vector<int>& getModelIndexes() { return m_modelIndexes; }

		void setTransfer(const Vector3& vector) { m_transfer = vector; }
		Vector3 getTransfer() { return m_transfer; }
		
	private:
		Type m_type;
		//int m_density;				// 密度级别, 从1~16,共16个级别
		int m_incidence;				// 渲染的概率

		Vector3 m_transfer;				// 位置的调整

		MaterialPtr m_mat;					// 材质
		TexturePtr m_tex;					// 纹理
		String m_texName;				// 纹理图片名
		RenderMesh* m_mesh;					// 网格

		String m_modelFilename;			// 模型文件名
		HavokModel* m_model;					// 模型 (仅当类型为Model时可用)
		std::vector<Vertex> m_modelVertexes;			// 模型的顶点列表
		std::vector<int>	m_modelIndexes;			// 模型的索引列表

		Rgba m_color;					// 颜色
		String m_name;

		float m_width, m_height;		// 宽高
		float m_minScale, m_maxScale;	// 缩放范围
	};
	
	//--------------------------------------------------------------------------
	// class GrassManager
	//--------------------------------------------------------------------------

	class AX_API GrassManager
	{
	public:
		GrassManager(Terrain* terrain);
		virtual ~GrassManager();

		// 草每个级别的数量
		int getNumGrassPerLevel() { return m_numGrassPerLevel; }
		void setNumGrassPerLevel(int num) { m_numGrassPerLevel = num; }

		void setTileLength(int len) { m_tileLen = len; }
		int getTileLenght() { return m_tileLen; }

		void setAdjustLength(int len) { m_adjustLen = len; }
		int getAdjustLength() { return m_adjustLen; }

		int getMaxGrassLevel() { return m_maxGrassLevel; }

		Rect getIndexByCoor(Vector4 rect);
		Vector4 getCoorByIndex(Rect indexRect);

		void setLevelByIndex(int r, int c, int level, bool add = false);
		void setLevelByIndex(Rect indexRect, int level, bool add = false);
		int getLevelByIndex(int r, int c);

		void setLevelByCoor(float x, float y, int level, bool add = false);
		Rect setLevelByCircle(const Vector2& center, float radius, int level, bool add = false);
		void setLevelByCoor(Vector4 rect, int level, bool add = false);
		int getLevelByCoor(float x, float y);

		void update(QueuedScene* qscene);
		void update(const Rect& rect, bool forceUpdate = false);	// 第二个参数为是否强制更新
		void uploadRenderData(Primitives* primSeq);
		void clear();
		void init();

		int getTotalNumGrasses(const Rect& rect);	// 获取草的总数

		// grassDef
		void addGrassDef(const GrassDef& grassDef) { m_grassDefSet.push_back(grassDef); }
		void deleteGrassDef(int index) { m_grassDefSet.erase(m_grassDefSet.begin() + index); }
		GrassDef& getGrassDef(int index)		{ return m_grassDefSet[index]; };
		int getNumGrassDef()	{ return (int) m_grassDefSet.size(); }

		// save / load
		bool saveFile(const String& filename, File* file, int indent);
		bool loadFile(const String& filename, const TiXmlElement* elem);

		byte_t* getDensityDataCopy(const Rect& rect);
		void setDensityData(const Rect& rect, const byte_t* const data);
		byte_t* getOldDensityDataCopy(const Rect& rect);
		void setOldDensityData(const Rect& rect, const byte_t* const data);

	private:
		void generateGeometricData(const Rect& rect);
		// 根据中心点和右向量,生成面片数据, 结果保存在data里, data的容量必须>=4
		void generateBillboardData(const Vector3& center, const Vector3& right, float width, float height, Vector3* data);
		void uploadBillboardData(ushort_t* indexes, Vertex* vertexes, int& curIndex, int& curVertex, Vector3* data, const Rgba& color);
		Rect calcUpdateRect(const Vector3& cameraPos);

		void saveGrassInfo(File* file, int indent);
		void loadGrassInfo(const String& filename, const TiXmlElement* elem);

		byte_t* m_densityBuf;		// 密度图
		byte_t* m_oldDensityBuf;		// 密度图副本
		int m_bufSize;			// 密度图大小
		static const int m_bufStepSize = 100;	// 每次增加的buf大小

		int m_maxR, m_maxC;
		int m_tileLen;			// 密度图格子的大小

		int m_numGrass;			// 草的总数
		int m_numGrassPerLevel;	// 一个级别草的数量
		//int m_maxNumGrassPerTile; // 每格子草的总数
		static const int m_maxGrassLevel = 16;// 最大的级别数

		Rect m_updateRect;		// 更新数据的范围
		int m_adjustLen;			// 缓冲的更新范围长度
		Vector3 m_updatedPos;		// 更新数据时摄像机的位置
		//Vector3 m_camPos;			// 摄像机位置
		
		Terrain* m_terrain;
		Vector4 m_terrainRect;

		std::vector<GrassDef>	m_grassDefSet;
	};


}} // namespace Axon::Map

#endif

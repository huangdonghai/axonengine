/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_MAP_RIVER_H
#define AX_MAP_RIVER_H

namespace Axon { namespace Map {

	class Terrain;

	//--------------------------------------------------------------------------
	// class RiverDef
	//--------------------------------------------------------------------------

	class AX_API RiverDef
	{
	public:
		friend class RiverManager;

		RiverDef()
			: m_width(2.0f)
			, m_dirty(true)
			, m_depth(2.0f)
			, m_fogColor(0, 0, 255, 255)
			, m_material(NULL)
			, m_mesh(NULL)
		{
			m_spline.setWidth(m_width);
			m_spline.setDirty(true);
		}

		void setName(const String& name) { m_name = name; }
		String getName() const { return m_name; }
		void setDirty(bool dirty) { m_dirty = dirty; }
		bool isDirty() const { return m_dirty; }

		float getDepth() const { return m_dirty; }
		void setDepth(float depth) 
							{ 
								m_depth = depth;
								m_dirty = true;
								m_spline.setDirty(true);
							}

		float getWidth() const { return m_width; }
		void setWidth(float width) 
							{ 
								m_width = width; 
								m_dirty = true; 
								m_spline.setWidth(m_width); 
							}

		void setFogColor(const Rgba& color) { m_fogColor = color; }
		Rgba getFogColor() const { return m_fogColor; }

		void setSpline(const Spline& spline) { m_spline = spline; }
		Spline& getSpline() { return m_spline; }
		const Spline& getSpline() const { return m_spline; }

		Render::Material* getMaterial() const { return m_material; }
		void setMaterial(Render::Material* material) { m_material = material; }

		Render::Mesh* getMesh() const { return m_mesh; }
		void setMesh(Render::Mesh* mesh) { m_mesh = mesh; }

	protected:
		String m_name;	
		bool m_dirty;		// 是否需要重新构造Mesh数据

		float m_width;		// 河流宽度
		float m_depth;		// 河流深度
		Rgba m_fogColor;	// 雾颜色(即水的颜色)

		Spline m_spline;	// 样条曲线(控制点在里边)

		Render::Material* m_material;	// 材质
		Render::Mesh* m_mesh;
	};

	//--------------------------------------------------------------------------
	// class RiverDef
	//--------------------------------------------------------------------------

	class AX_API RiverManager
	{
	public:
		RiverManager(Terrain* terrain);
		virtual ~RiverManager();

		// RiverDef
		void addRiverDef(const RiverDef& riverDef)	{ m_riverDefSet.push_back(riverDef); }
		void deleteRiverDef(int index)	{ m_riverDefSet.erase(m_riverDefSet.begin() + index); }
		/*void deleteRiverDef(RiverDef* river)	
		{ 
			for (int i=0; i<(int)m_riverDefSet.size(); ++i)
			{
				if (&(m_riverDefSet[i]) == river)
				{
					m_riverDefSet.erase(m_riverDefSet.begin() + i);
					return ;
				}
			}
		}*/
		RiverDef& getRiverDef(int index)		{ return m_riverDefSet[index]; };
		int getNumRiverDef()	{ return (int) m_riverDefSet.size(); }

		// operate
		void update();
		void uploadRenderData(RenderPrims* primSeq);
		void clear();
		void init();

		// save / load
		bool saveFile(const String& filename, File* file, int indent);
		bool loadFile(const String& filename, const TiXmlElement* elem);

	protected:
		void saveRiverInfo(File* file, int indent);
		void loadRiverInfo(const String& filename, const TiXmlElement* elem);

		std::vector<RiverDef>	m_riverDefSet;
		Terrain* m_terrain;
		MaterialPtr m_defaultMtr;
	};


}} // namespace Axon::Map

#endif

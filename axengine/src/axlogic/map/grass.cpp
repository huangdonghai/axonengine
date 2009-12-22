/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

namespace Axon { namespace Map {
	
	GrassManager::GrassManager(Terrain* terrain)
		: m_densityBuf(NULL)
		, m_oldDensityBuf(NULL)
		, m_maxR(0)
		, m_maxC(0)
		, m_bufSize(0)
	{
		m_terrain = terrain;
		
		m_tileLen = 4;
		m_numGrass = 0;
		m_numGrassPerLevel = 3;
		//m_adjustLen = m_tileLen * 2;
		m_adjustLen = 5/* r_grassAdjust->getInteger()*/;
	}

	GrassManager::~GrassManager()
	{
		clear();
	}

	void GrassManager::init()
	{
		//clear();

		m_terrainRect = m_terrain->getTerrainRect();

		m_maxR = (int) m_terrainRect.z / m_tileLen;
		m_maxC = (int) m_terrainRect.w / m_tileLen;
		
		if (m_densityBuf == NULL)
		{
			m_bufSize = m_maxR * m_maxC;
			m_densityBuf = new byte_t[m_bufSize];
			memset(m_densityBuf,0,m_bufSize);
		}
		else
		{
			if (m_bufSize != m_maxR * m_maxC)
			{
				Errorf("The density size is not equal m_maxR * m_maxC");
			}
		}

		m_updateRect = Rect(0, 0, 0, 0);
		m_updatedPos = Vector3(-100000, -100000, -100000);

		/*
		// ========== just  for test ========
		for (int i=0; i<m_bufSize; ++i)
		{
			m_densityBuf[i] = 2;
		}
		memset(m_densityBuf, 0, m_bufSize * sizeof(byte_t));
		
		if (getNumGrassDef() == 0)
		{
			GrassDef grassDef;

			grassDef.setMaterial(Material::load("grass2", (intptr_t)"grass2"));
			grassDef.setMaterialName("grass2");
			addGrassDef(grassDef);

			grassDef.setMaterial(Material::load("grass3", (intptr_t)"grass3"));
			grassDef.setMaterialName("grass3");
			addGrassDef(grassDef);
		}
		// ===============================
		*/
	}

	void GrassManager::update(const Rect& rect, bool forceUpdate/* = false*/)
	{
#if 0
		bool gonnaUpdate = false;
		/*gRenderSystem->getMainCamera().getOrigin()*/
		
		// there is no gEditorContext when it run in game mode.
		Vector3 camPos = gEditorContext->getActiveView()->getCamera().getOrigin();	

		// 如果强制更新
		if (forceUpdate)
		{
			gonnaUpdate = true;
		}
		// 如果摄像机移动的距离超过缓冲距离
		else if (abs(camPos.x - m_updatedPos.x) > m_adjustLen || abs(camPos.y - m_updatedPos.y) > m_adjustLen)
		{
			gonnaUpdate = true;
		}
		// 最后判断更新的包围盒是否在缓冲矩形内,是的话才更新,否则不更新
		else
		{
			if (rect.isEmpty())
			{
				return ;
			}

			Rect inRect = rect.intersect(m_updateRect);
			if (inRect.isEmpty() == false)
			{
				gonnaUpdate = true;
			}
		}
		
		if (gonnaUpdate)
		{
			m_updateRect = calcUpdateRect(camPos);
			m_updatedPos = camPos;

			generateGeometricData(m_updateRect);	
		}
#endif
	}

	void GrassManager::update(QueuedScene* qscene)
	{
		bool gonnaUpdate = false;
		/*gRenderSystem->getMainCamera().getOrigin()*/

		// there is no gEditorContext when it run in game mode.
		Vector3 camPos = qscene->camera.getOrigin();	

		// 如果强制更新
		if (abs(camPos.x - m_updatedPos.x) > m_adjustLen || abs(camPos.y - m_updatedPos.y) > m_adjustLen)
		{
			gonnaUpdate = true;
		}

		if (gonnaUpdate)
		{
			m_updateRect = calcUpdateRect(camPos);
			m_updatedPos = camPos;

			generateGeometricData(m_updateRect);	
		}
	}

	Rect GrassManager::calcUpdateRect(const Vector3& cameraPos)
	{
		Rect rect;

		int camC = (cameraPos.x - m_terrainRect.x) / m_tileLen;
		int camR = (cameraPos.y - m_terrainRect.y) / m_tileLen;
		// r_grassDistance是摄像机能看到草的距离
		int halfUpdateDis = (r_grassDistance->getInteger() + (m_adjustLen /*= r_grassAdjust->getInteger()*/)) / m_tileLen;	
		
		rect.x = camC - halfUpdateDis;
		rect.y = camR - halfUpdateDis;
		rect.width = rect.height = 2 * halfUpdateDis;
		
		// 矫正边界
		if (rect.x < 0)
		{
			rect.x = 0;
		}
		if (rect.y < 0)
		{
			rect.y = 0;
		}
		if (rect.x + rect.width > m_maxC)
		{
			rect.width = m_maxC - rect.x;
		}
		if (rect.y + rect.height > m_maxR)
		{
			rect.height = m_maxR - rect.y;
		}

		return rect;
	}

	int GrassManager::getTotalNumGrasses(const Rect& rect)
	{
		/*if (m_numGrass > 0)
		{
			return m_numGrass;
		}*/

		if (m_densityBuf == NULL)
		{
			return 0;
		}

		m_numGrass = 0;
		int index;
		int maxNumGrassPerTile = m_numGrassPerLevel * m_maxGrassLevel;

		for (int r=rect.y; r<rect.y+rect.height; ++r)
		{
			for (int c=rect.x; c<rect.x+rect.width; ++c)
			{
				index = r * m_maxC + c;
				m_numGrass += (m_densityBuf[index] / 255.0f) * maxNumGrassPerTile;
			}
		}
		
		return m_numGrass;
	}

	void GrassManager::generateGeometricData(const Rect& rect)
	{
		int totalNum = getTotalNumGrasses(rect);	// 总的草数
		int totalIncidence = 0;		// 总的概率数
		int numRenderedGrass = 0;	// 渲染的草数目

		for (int i=0; i<(int)m_grassDefSet.size(); ++i)
		{
			totalIncidence += m_grassDefSet[i].getIncidence();
		}

		// 判断是否没有草要渲染, 没有则返回.
		if (totalIncidence <= 0 || totalNum <= 0)
		{
			for (int i=0; i<(int)m_grassDefSet.size(); ++i)
			{
				if (m_grassDefSet[i].getMesh() != NULL)
				{
					m_grassDefSet[i].getMesh()->setActivedIndexes(0);
				}
			}

			g_statistic->setValue(stat_numGrasses, numRenderedGrass);
			return ;
		}
		
		float incidence = 0.0f;
		int numTileGrass = 0;
		RenderMesh* mesh = NULL;
		int maxNumGrassPerTile = m_numGrassPerLevel * m_maxGrassLevel;

		for (int i=0; i<(int)m_grassDefSet.size(); ++i)
		{
			mesh = m_grassDefSet[i].getMesh();

			// 生成该种草的概率为0,则返回
			if (m_grassDefSet[i].getIncidence() <= 0)
			{
				if (mesh != NULL)
				{
					mesh->setActivedIndexes(0);
				}

				continue ;
			}

			// 如果尚未创建mesh 
			if (mesh == NULL)
			{
				mesh = new RenderMesh(RenderMesh::Static);
				m_grassDefSet[i].setMesh(mesh);
			}

			// 计算每种草需要的顶点数和索引数
			incidence = m_grassDefSet[i].getIncidence() / (float) totalIncidence;	// 计算概率
			int totalNumPerGrass = (int) totalNum * incidence;						// 草的总科数

			int numVertex, numIndex;
			switch (m_grassDefSet[i].getType())
			{
			case GrassDef::Criss:
				numVertex = totalNumPerGrass * 8;
				numIndex = totalNumPerGrass * 12;
				break;

			case GrassDef::Triangle:
			case GrassDef::Star:
				numVertex = totalNumPerGrass * 12;
				numIndex = totalNumPerGrass * 18;
				break;

			case GrassDef::Model:
				numVertex = int (totalNumPerGrass * m_grassDefSet[i].getModelVertexes().size());
				numIndex = int (totalNumPerGrass * m_grassDefSet[i].getModelIndexes().size());
				break;
			}

			if (numIndex <= 0 || numVertex <= 0)
			{
				continue;
			}
			
			// 如果顶点不够,重新分配顶点和索引内存
			if (mesh->getNumVertexes() < numVertex)
			{
				mesh->initialize(numVertex + m_bufStepSize, numIndex + m_bufStepSize * 1.5);				
			}

			mesh->setActivedIndexes(0);

			ushort_t*    indexes  = mesh->lockIndexes();
			Vertex* vertexes = mesh->lockVertexes();
			int curVertex = 0;
			int curIndex = 0;		
			Rgba color = m_grassDefSet[i].getColor();
			//Rgba color = Rgba(0, 255, 0);

			Vector4 v0;			// 格子的起始点和终止点
			Vector3 center;		// 中心点, 在面片的下中,且位于地表
			Vector3 corner[4];	// 面片的四个角,依次是上左,上右,下右,下左
			float w, h;

			int index;
			for (int r=rect.y; r<rect.y+rect.height; ++r)
			{
				for (int c=rect.x; c<rect.x+rect.width; ++c)
				{
					index = r * m_maxC + c;
					
					// 每个格子需要生成该种草的数量
					numTileGrass = (m_densityBuf[index]/255.0f) * maxNumGrassPerTile * incidence;

					if (numTileGrass <= 0)
					{
						continue ;
					}

					srand(r * c * ((i+1)*(i+1))/*index * 10000 + i * m_bufSize*/);	// 播种种子

					// 格子的起始点和终止点
					v0.x = m_terrainRect.x + c * m_tileLen;
					v0.y = m_terrainRect.y + r * m_tileLen;
					v0.z = v0.y + m_tileLen;
					v0.w = v0.x + m_tileLen;

					// 根据草的类型构造渲染数据.
					for (int k=0; k<numTileGrass; ++k)
					{
						// 随机产生位置
#define RangeRandomFloat(fLow, fHigh) ((fHigh)-(fLow))*((float)(rand()/(float)RAND_MAX)) + (fLow)
						center.x = RangeRandomFloat(v0.x, v0.w);
						center.y = RangeRandomFloat(v0.y, v0.z);
						center.z = m_terrain->getHeightByPos(center);

						// 加上偏移量.
						center += m_grassDefSet[i].getTransfer();

						// 剔除低于水平面的草
						if (center.z < 0)
						{
							continue ;
						}

						++numRenderedGrass;

						// 宽高
						float scale = RangeRandomFloat(m_grassDefSet[i].getMinScale(), m_grassDefSet[i].getMaxScale());
						w = m_grassDefSet[i].getWidth() * scale;
						h = m_grassDefSet[i].getHeight() * scale;

						// 向右向量
						Vector3 direct;
						direct.x = RangeRandomFloat(-1, 1);
						direct.y = RangeRandomFloat(-1, 1);
						direct.z = 0;
						
						// 星形
						if (m_grassDefSet[i].getType() == GrassDef::Star)
						{
							generateBillboardData(center, direct, w, h, corner);
							uploadBillboardData(indexes, vertexes, curIndex, curVertex, corner, color);

							Quaternion qua;
							qua.fromAxisAngle(Vector3(0.0f, 0.0f, 1.0f), 60.0f);

							direct = qua * direct;
							generateBillboardData(center, direct, w, h, corner);
							uploadBillboardData(indexes, vertexes, curIndex, curVertex, corner, color);

							direct = qua * direct;
							generateBillboardData(center, direct, w, h, corner);
							uploadBillboardData(indexes, vertexes, curIndex, curVertex, corner, color);
						}
						// 三角形
						else if (m_grassDefSet[i].getType() == GrassDef::Triangle)
						{
							Vector3 tDir = direct;
							tDir.normalize();
							tDir *= w / 16.0f;

							generateBillboardData(center+tDir, Vector3(0.0, 0.0, 1.0) ^ tDir, w, h, corner);
							uploadBillboardData(indexes, vertexes, curIndex, curVertex, corner, color);

							Quaternion qua;
							qua.fromAxisAngle(Vector3(0.0f, 0.0f, 1.0f), 120.0f);

							tDir = qua * tDir;
							generateBillboardData(center+tDir, tDir ^ Vector3(0.0, 0.0, 1.0), w, h, corner);
							uploadBillboardData(indexes, vertexes, curIndex, curVertex, corner, color);

							tDir = qua * tDir;
							generateBillboardData(center+tDir, tDir ^ Vector3(0.0, 0.0, 1.0), w, h, corner);
							uploadBillboardData(indexes, vertexes, curIndex, curVertex, corner, color);
						}
						// 模型
						else if (m_grassDefSet[i].getType() == GrassDef::Model)
						{
							AffineMat mat;

							mat.setIdentity();
							// 注意转换的顺序
							mat.translateSelf(center);
							mat.scaleSelf(scale, scale, scale);
							mat.rotateSelf(0, 0, RangeRandomFloat(0, 360));

							int tNumVertex = curVertex;
							const std::vector<Vertex>& modelVertexes = m_grassDefSet[i].getModelVertexes();
							const std::vector<int>& modelIndexes = m_grassDefSet[i].getModelIndexes();
							for (int n=0; n<(int)modelVertexes.size(); ++n)
							{
								vertexes[curVertex].xyz = mat * modelVertexes[n].xyz;
								vertexes[curVertex].st = modelVertexes[n].st;
								vertexes[curVertex].normal = mat * modelVertexes[n].normal;
								vertexes[curVertex++].rgba = /*m_grassDefSet[i].getColor()*/modelVertexes[n].rgba;
							}

							for (int n=0; n<(int)modelIndexes.size(); ++n)
							{
								indexes[curIndex++] = modelIndexes[n] + tNumVertex;
							}
						}
						// 默认是十字交叉型草
						else /*(m_grassDefSet[i].getType() == GrassDef::Criss)*/
						{
#if 0
							// 向右向量
							Quaternion qua;
							Vector3 direct(1.0f, 0.0f, 0.0f) ;
							float angle = RandomFloat() * 360.0f;

							qua.fromAxisAngle(Vector3(0.0f, 0.0f, 1.0f), angle);
							direct = qua * direct;

							generateBillboardData(center, direct, w, h, corner);
							uploadBillboardData(indexes, vertexes, curIndex, curVertex, corner, Rgba(148, 148, 0, 255));

							qua.fromAxisAngle(Vector3(0.0f, 0.0f, 1.0f), 90.0f);
							direct = qua * direct;

							generateBillboardData(center, direct, w, h, corner);
							uploadBillboardData(indexes, vertexes, curIndex, curVertex, corner, Rgba(148, 148, 0, 255));
#else
							generateBillboardData(center, direct, w, h, corner);
							uploadBillboardData(indexes, vertexes, curIndex, curVertex, corner, color);

							generateBillboardData(center, direct ^ Vector3(0, 0, 1), w, h, corner);
							uploadBillboardData(indexes, vertexes, curIndex, curVertex, corner, color);

#endif
						} // if
					} // for
				} // for
			} // for

			mesh->setActivedIndexes(curIndex);

			mesh->unlockIndexes();
			mesh->unlockVertexes();
		} // for

		g_statistic->setValue(stat_numGrasses, numRenderedGrass);
	}

	void GrassManager::generateBillboardData(const Vector3& center, const Vector3& right,
		float width, float height, Vector3* data)
	{
		Vector3 dir = right;
		dir.normalize();
		dir *= width / 2.0f;

		// 下右
		data[2] = center + dir;
		data[3] = center - dir;
		data[0] = data[3] + (Vector3(0.0f, 0.0f, 1.0f) * height);
		data[1] = data[2] + (Vector3(0.0f, 0.0f, 1.0f) * height);
	}

	void GrassManager::uploadBillboardData(ushort_t* indexes, Vertex* vertexes, int& curIndex, int& curVertex, Vector3* data, const Rgba& color)
	{
		int tCurVertex = curVertex;

		vertexes[curVertex].xyz = data[0];
		vertexes[curVertex].st = Vector2(0.0f, 0.0f);
		vertexes[curVertex].normal = Vector3(0.0f, 0.0f, 1.0f);	// 法线向上
		vertexes[curVertex++].rgba = color;

		vertexes[curVertex].xyz = data[1];
		vertexes[curVertex].st = Vector2(1.0f, 0.0f);
		vertexes[curVertex].normal = Vector3(0.0f, 0.0f, 1.0f);	// 法线向上
		vertexes[curVertex++].rgba = color;

		vertexes[curVertex].xyz = data[2];
		vertexes[curVertex].st = Vector2(1.0f, 1.0f);
		vertexes[curVertex].normal = Vector3(0.0f, 0.0f, 1.0f);	// 法线向上
		vertexes[curVertex++].rgba = color;

		vertexes[curVertex].xyz = data[3];
		vertexes[curVertex].st = Vector2(0.0f, 1.0f);
		vertexes[curVertex].normal = Vector3(0.0f, 0.0f, 1.0f);	// 法线向上
		vertexes[curVertex++].rgba = color;

		indexes[curIndex++] = tCurVertex;
		indexes[curIndex++] = tCurVertex + 1;
		indexes[curIndex++] = tCurVertex + 2;
		indexes[curIndex++] = tCurVertex;
		indexes[curIndex++] = tCurVertex + 2;
		indexes[curIndex++] = tCurVertex + 3;
	}

	void GrassManager::uploadRenderData(Primitives* primSeq)
	{
		if (m_grassDefSet.empty() || m_terrain==NULL)
		{
			return ;
		}

		update(Rect(0, 0, 0, 0));
		
		for (int i=0; i<(int)m_grassDefSet.size(); ++i)
		{
			RenderMesh* mesh = m_grassDefSet[i].getMesh();

			if (mesh != NULL && mesh->getActivedIndexes() > 0)
			{
				Material* mat = m_grassDefSet[i].getMaterial();

				if (mat != NULL)
				{
					mesh->setMaterial(mat);
				}

				/*
				Texture* tex = m_grassDefSet[i].getTexture();
				if (tex)
				{
					m_material->setTexture(SamplerType::Diffuse, tex);
					mesh->setMaterial(m_material);
				}*/

				primSeq->push_back(mesh);
			}
		}
	}

	void GrassManager::clear()
	{
		m_grassDefSet.clear();

		SafeDeleteArray(m_densityBuf);
		SafeDeleteArray(m_oldDensityBuf);
		//ZeroMemory(m_densityBuf, m_bufSize);
	}

	bool GrassManager::saveFile(const String& filename, File* file, int indent)
	{
#define INDENT if (indent) file->printf("%s", ind.c_str());
		String ind(indent, '\t');		//String ind(indent*2, ' ');

		String filenameNoExt = PathUtil::removeExt(filename);

		if (!m_grassDefSet.empty())
		{
			INDENT;file->printf("<GrassInfo>\n");

			saveGrassInfo(file, indent+1);

			INDENT;file->printf("</GrassInfo>\n");
		}

		if (m_densityBuf != NULL && m_bufSize > 0)
		{
			g_fileSystem->writeFile(filenameNoExt + "_grass.density", m_densityBuf, m_bufSize * sizeof(byte_t));
		}
		else
		{
			return false;
		}

#undef INDENT

		return true;
	}

	bool GrassManager::loadFile(const String& filename, const TiXmlElement* elem)
	{
		clear();

		loadGrassInfo(filename, elem);

		String filepath = PathUtil::removeExt(filename) + "_grass.density";

		SafeDelete(m_densityBuf);
		m_bufSize = s2i(g_fileSystem->readFile(filepath, (void**)&m_densityBuf));
		if (m_densityBuf == NULL || m_densityBuf == 0)
		{
			return false;
		}

		return true;
	}

	void GrassManager::saveGrassInfo(File* file, int indent)
	{
#define INDENT if (indent) file->printf("%s", ind.c_str());
		String ind(indent, '\t');		//String ind(indent*2, ' ');

		INDENT; file->printf("<GrassManager\n");
		INDENT; file->printf("\tnumGrassPerLevel=\"%d\"\n", m_numGrassPerLevel);
		//INDENT; file->printf("\tmaxGrassLevel=\"%d\"\n",	m_maxGrassLevel);
		INDENT; file->printf("\tadjustLen=\"%d\"\n",		m_adjustLen);
		INDENT; file->printf("\ttileLen=\"%d\"\n",			m_tileLen);
		INDENT; file->printf("/>\n");

		for (size_t i=0; i<m_grassDefSet.size(); ++i)
		{
			INDENT; file->printf("<GrassDef\n");

			INDENT; file->printf("\ttype=\"%d\"\n",			m_grassDefSet[i].getType());
			INDENT; file->printf("\tincidence=\"%d\"\n",	m_grassDefSet[i].getIncidence());
			INDENT; file->printf("\twidth=\"%f\"\n",		m_grassDefSet[i].getWidth());
			INDENT; file->printf("\theight=\"%f\"\n",		m_grassDefSet[i].getHeight());
			INDENT; file->printf("\tminScale=\"%f\"\n",		m_grassDefSet[i].getMinScale());
			INDENT; file->printf("\tmaxScale=\"%f\"\n",		m_grassDefSet[i].getMaxScale());
			INDENT; file->printf("\tcolor=\"%s\"\n",		m_grassDefSet[i].getColor().toString().c_str());
			INDENT; file->printf("\ttexture=\"%s\"\n",		m_grassDefSet[i].getTextureName().c_str());
			INDENT; file->printf("\tname=\"%s\"\n",			m_grassDefSet[i].getName().c_str());
			INDENT; file->printf("\ttransfer=\"%s\"\n",		m_grassDefSet[i].getTransfer().toString().c_str());

			if (m_grassDefSet[i].getType() == GrassDef::Model)
			{
				INDENT; file->printf("\tmodel=\"%s\"\n",	m_grassDefSet[i].getModelFilename().c_str());
			}
			
			INDENT; file->printf("/>\n");
		}

#undef INDENT
	}

	void GrassManager::loadGrassInfo(const String& filename, const TiXmlElement* elemNode)
	{
		if (!elemNode)
		{
			return ;
		}

		const TiXmlAttribute* attr = NULL;
		const TiXmlElement* elem;
		const char* v(NULL);

		for (elem = elemNode->FirstChildElement(); elem; elem = elem->NextSiblingElement()) 
		{
			const String& value = elem->ValueTStr();
			GrassDef grassDef;

			if (value == "GrassManager") 
			{
				v = elem->Attribute("numGrassPerLevel", &m_numGrassPerLevel);
				/*if (!v)
				{
				return ;
				}*/
				//elem->Attribute("maxGrassLevel", &m_maxGrassLevel);
				elem->Attribute("adjustLen", &m_adjustLen);
				elem->Attribute("tileLen", &m_tileLen);
			}
			else if (value == "GrassDef")
			{
				int n;
				elem->Attribute("type", &n);
				grassDef.setType((GrassDef::Type) n);
				elem->Attribute("incidence", &n);
				grassDef.setIncidence(n);

				double d;
				elem->Attribute("width", &d);
				grassDef.setWidth(d);
				elem->Attribute("height", &d);
				grassDef.setHeight(d);

				elem->Attribute("minScale", &d);
				grassDef.setMinScale(d);
				elem->Attribute("maxScale", &d);
				grassDef.setMaxScale(d);

				v = elem->Attribute("color");
				Rgba color;
				color.fromString(v);
				grassDef.setColor(color);

				if (v = elem->Attribute("texture"))
				{
					grassDef.setTextureName(v);
				}

				if (v = elem->Attribute("name"))
				{
					grassDef.setName(v);
				}
				
				if (v = elem->Attribute("transfer"))
				{
					Vector3 trans;
					trans.fromString(v);
					grassDef.setTransfer(trans);
				}	

				if (grassDef.getType() == GrassDef::Model)
				{
					if (v = elem->Attribute("model"))
					{
						grassDef.setModelFilename(v);
					}
				}

				addGrassDef(grassDef);
			}
		} // for
	}

	void GrassDef::setTextureName(const String& texName) 
	{ 
		AX_ASSERT(!texName.empty());

		if (!m_mat)
		{
			m_mat = Material::load("grass");
		}

		m_texName = texName;
		m_tex = Texture::load(m_texName);
		AX_ASSERT(m_tex);

		m_mat->setTexture(SamplerType::Diffuse, m_tex.get());
	}

	void GrassDef::setModelFilename(const String& name)
	{
		AX_ASSERT(!name.empty());

		if (m_model != NULL)
		{
			SafeDelete(m_model);
		}

		m_modelVertexes.clear();
		m_modelIndexes.clear();

		m_model = new PhysicsModel(name);

		Primitives primitiveSeq =  m_model->getAllPrimitives();

		Primitives::iterator it;
		int numVertexes = 0;

		for (it = primitiveSeq.begin();it != primitiveSeq.end();++ it)
		{
			if ((*it)->getType() == Primitive::MeshType)
			{
				RenderMesh* mesh = dynamic_cast<RenderMesh*>(*it);

				Vertex* vertexData = mesh->lockVertexes();
				ushort_t*    indexData  = mesh->lockIndexes();
				int     vertexNum  = mesh->getNumVertexes();
				int     indexNum   = mesh->getNumIndexes();

				// push vertex data
				for (int i=0; i<vertexNum; ++i)
				{
					m_modelVertexes.push_back(vertexData[i]);
				}

				for (int i=0; i<indexNum; ++i)
				{
					m_modelIndexes.push_back(indexData[i] + numVertexes);
				}

				mesh->unlockVertexes();
				numVertexes += vertexNum;
			}
			else
			{
				AX_ASSERT(0 && "invalid mesh type!");
			}
		}

		SafeDelete(m_model);

		m_modelFilename = name;
	}

	void GrassManager::setLevelByIndex(int r, int c, int level, bool add)
	{
		AX_ASSERT(r >=0 && r<m_maxR && c >=0 && c<m_maxC /*&& level>=0 && level<=m_maxGrassLevel*/);

		int num = m_densityBuf[r * m_maxC + c];

		if (add)
		{
			num += level;
		}
		else
		{
			num = level;
		}
		
		if (num > 255)
		{
			num = 255;
		}
		else if (num < 0)
		{
			num = 0;
		}
		
		m_densityBuf[r * m_maxC + c] = num;
	}

	int GrassManager::getLevelByIndex(int r, int c)
	{
		AX_ASSERT(r >=0 && r<m_maxR && c >=0 && c<m_maxC);

		return m_densityBuf[r * m_maxC + c];
	}

	void GrassManager::setLevelByCoor(float x, float y, int level, bool add)
	{
		int r = (y - m_terrainRect.y) / m_tileLen;
		int c = (x - m_terrainRect.x) / m_tileLen;

		setLevelByIndex(r, c, level, add);

		m_numGrass = 0;
	}

	Rect GrassManager::setLevelByCircle(const Vector2& center, float radius, int level, bool add /*= false*/)
	{
		Rect indexRect = getIndexByCoor(Vector4(center.x - radius, center.y - radius, 
										2 * radius + m_tileLen, 2 * radius + m_tileLen));

		//indexRect = indexRect.intersect(Rect(0, 0, m_maxC, m_maxR));

		for (int r=indexRect.y; r<indexRect.y+indexRect.height; ++r)
		{
			for (int c=indexRect.x; c<indexRect.x+indexRect.width; ++c)
			{
				Vector2 tileCen = Vector2((c + 0.5) * m_tileLen + m_terrainRect.x, (r + 0.5) * m_tileLen + m_terrainRect.y);
				float dis = (center - tileCen).getLength();

				if (dis <= radius)
				{
					setLevelByIndex(r, c, level, add);
				}
				/*else if (dis < radius * 2)
				{

				}*/
			}
		}

		update(indexRect, true);

		return indexRect;
	}

	int GrassManager::getLevelByCoor(float x, float y)
	{
		int r = (y - m_terrainRect.y) / m_tileLen;
		int c = (x - m_terrainRect.x) / m_tileLen;

		return getLevelByIndex(r, c);
	}

	Rect GrassManager::getIndexByCoor(Vector4 rect)
	{
		Rect index;

		index.x = (rect.x - m_terrainRect.x) / m_tileLen;
		index.y = (rect.y - m_terrainRect.y) / m_tileLen;
		index.width = rect.z / m_tileLen;
		index.height = rect.w / m_tileLen;

		index = index.intersect(Rect(0, 0, m_maxC, m_maxR));

		return index;
	}

	Vector4 GrassManager::getCoorByIndex(Rect indexRect)
	{
		Vector4 vec;

		vec.x = indexRect.x * m_tileLen;
		vec.y = indexRect.y * m_tileLen;
		vec.w = indexRect.width * m_tileLen;
		vec.z = indexRect.height * m_tileLen;

		return vec;
	}

	void GrassManager::setLevelByIndex(Rect indexRect, int level, bool add/* = false*/)
	{
		for (int r=indexRect.y; r<indexRect.y+indexRect.height; ++r)
		{
			for (int c=indexRect.x; c<indexRect.x+indexRect.width; ++c)
			{
				setLevelByIndex(r, c, level, add);
			}
		}

		update(indexRect, true);
	}

	void GrassManager::setLevelByCoor(Vector4 rect, int level, bool add/* = false*/)
	{
		Rect indexRect = getIndexByCoor(rect);

		setLevelByIndex(indexRect, level, add);

		update(indexRect, true);
	}

	byte_t* GrassManager::getDensityDataCopy(const Rect& rect)
	{
		byte_t* data = new byte_t[rect.width * rect.height];
		for (int r=0; r<rect.height; ++r)
		{
			for (int c=0; c<rect.width; ++c)
			{
				data[r*rect.width + c] = m_densityBuf[(rect.y+r)*m_maxC + (rect.x+c)];
			}
		}

		return data;
	}

	void GrassManager::setDensityData(const Rect& rect, const byte_t* const data)
	{
		for (int r=0; r<rect.height; ++r)
		{
			for (int c=0; c<rect.width; ++c)
			{
				m_densityBuf[(rect.y+r)*m_maxC + (rect.x+c)] = data[r*rect.width + c];
			}
		}
	}

	byte_t* GrassManager::getOldDensityDataCopy(const Rect& rect)
	{
		if (m_oldDensityBuf == NULL)
		{
			if (m_bufSize > 0)
			{
				m_oldDensityBuf = new byte_t[m_bufSize];

				memcpy(m_oldDensityBuf, m_densityBuf, m_bufSize);
			}
			else
			{
				return NULL;
			}
		}
		
		byte_t* data = new byte_t[rect.width * rect.height];
		for (int r=0; r<rect.height; ++r)
		{
			for (int c=0; c<rect.width; ++c)
			{
				data[r*rect.width + c] = m_oldDensityBuf[(rect.y+r)*m_maxC + (rect.x+c)];
			}
		}
		
		return data;
	}

	void GrassManager::setOldDensityData(const Rect& rect, const byte_t* const data)
	{
		AX_ASSERT(m_oldDensityBuf != NULL);

		for (int r=0; r<rect.height; ++r)
		{
			for (int c=0; c<rect.width; ++c)
			{
				m_oldDensityBuf[(rect.y+r)*m_maxC + (rect.x+c)] = data[r*rect.width + c];
			}
		}
	}


}}

/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

namespace Axon { namespace Map {
	
	//--------------------------------------------------------------------------
	// class RiverManager
	//--------------------------------------------------------------------------

	RiverManager::RiverManager(Terrain* terrain)
	{
		m_terrain = terrain;
		
		init();
		// for test
		// m_riverDefSet.pu
		// test end
	}

	RiverManager::~RiverManager()
	{
		clear();
	}

	void RiverManager::update()
	{
		RenderMesh* mesh= NULL;

		for (int i=0; i<(int)m_riverDefSet.size(); ++i)
		{
			RiverDef& river = m_riverDefSet[i];

			if (river.getSpline().isDirty())
			{
				if (river.getSpline().getNumControlPoint() <= 1 || river.getSpline().getWidth() == 0.0f)
				{
					continue;
				}

				std::vector<Vector3> points;
				river.getSpline().getAllPoints(points);
				mesh = river.getMesh();

				if (mesh == NULL)
				{
					mesh = new RenderMesh(RenderMesh::Static);
					river.setMesh(mesh);
				}

				int numControlPoint = (int)points.size();
				mesh->initialize(numControlPoint * 2 / 3, numControlPoint * 2);

				Vertex* vertexes = mesh->lockVertexes();
				ushort_t* indexes = mesh->lockIndexes();
				int curVertex = 0;
				int curIndex = 0;
				int tNumVer;
				int texIndex = 0;
				float riverZ = points[0].z;	// 河流的高度(即z坐标)取第一个点的高度.

				for (int j=0; j<(int)points.size()-3; j+=3)
				{
					tNumVer = curVertex;

					vertexes[curVertex].xyz = points[j];
					vertexes[curVertex].xyz.z = riverZ;
					vertexes[curVertex++].st = Vector2(0, texIndex);

					vertexes[curVertex].xyz = points[j+2];
					vertexes[curVertex].xyz.z = riverZ;
					vertexes[curVertex++].st = Vector2(1, texIndex);

					++ texIndex;

					indexes[curIndex++] = tNumVer;
					indexes[curIndex++] = tNumVer + 1;
					indexes[curIndex++] = tNumVer + 3;

					indexes[curIndex++] = tNumVer;
					indexes[curIndex++] = tNumVer + 3;
					indexes[curIndex++] = tNumVer + 2;/**/
				}

				vertexes[curVertex].xyz = points[points.size()-3];
				vertexes[curVertex].xyz.z = riverZ;
				vertexes[curVertex++].st = Vector2(0, texIndex);

				vertexes[curVertex].xyz = points[points.size()-1];
				vertexes[curVertex].xyz.z = riverZ;
				vertexes[curVertex++].st = Vector2(1, texIndex);

				mesh->setActivedIndexes(curIndex);	
				mesh->unlockIndexes();
				mesh->unlockVertexes();
			}
		}
	}

	void RiverManager::uploadRenderData(Primitives* primSeq)
	{
		if (m_riverDefSet.empty() || m_terrain==NULL)
		{
			return ;
		}
		
		update();

		for (int i=0; i<(int)m_riverDefSet.size(); ++i)
		{
			RenderMesh* mesh = m_riverDefSet[i].getMesh();

			if (mesh != NULL && mesh->getActivedIndexes() > 0)
			{
				Material* mat = m_riverDefSet[i].getMaterial();

				if (mat != NULL)
				{
					mesh->setMaterial(mat);
				}
				else
				{
					mesh->setMaterial(m_defaultMtr.get());
				}

				primSeq->push_back(mesh);
			}
		}
	}

	void RiverManager::clear()
	{
		m_riverDefSet.clear();
	}

	void RiverManager::init()
	{
		m_defaultMtr = Material::load("materials/activeArea");
	}

	bool RiverManager::saveFile(const String& filename, File* file, int indent)
	{
		
#define INDENT if (indent) file->printf("%s", ind.c_str());
		String ind(indent, '\t');		//String ind(indent*2, ' ');

		String filenameNoExt = PathUtil::removeExt(filename);

		if (!m_riverDefSet.empty())
		{
			INDENT;file->printf("<RiverInfo>\n");

			saveRiverInfo(file, indent+1);

			INDENT;file->printf("</RiverInfo>\n");
		}

#undef INDENT

		return true;
	}

	bool RiverManager::loadFile(const String& filename, const TiXmlElement* elem)
	{
		clear();

		loadRiverInfo(filename, elem);

		return true;
	}

	void  RiverManager::saveRiverInfo(File* file, int indent)
	{
#define INDENT if (indent) file->printf("%s", ind.c_str());
		String ind(indent, '\t');		//String ind(indent*2, ' ');

		INDENT; file->printf("<RiverManager\n");
		INDENT; file->printf("/>\n");

		for (size_t i=0; i<m_riverDefSet.size(); ++i)
		{
			INDENT; file->printf("<RiverDef\n");

			INDENT; file->printf("\tname=\"%s\"\n",			m_riverDefSet[i].getName().c_str());
			INDENT; file->printf("\twidth=\"%f\"\n",		m_riverDefSet[i].getWidth());
			INDENT; file->printf("\tdepth=\"%f\"\n",		m_riverDefSet[i].getDepth());
			INDENT; file->printf("\tfogColor=\"%s\"\n",		m_riverDefSet[i].getFogColor().toString().c_str());

			// spline
			INDENT; file->printf("\tintervalLength=\"%f\"\n",m_riverDefSet[i].getSpline().getIntervalLength());
			INDENT; file->printf("\tclosed=\"%d\"\n",		m_riverDefSet[i].getSpline().isClosed());

			// control points
			String poinsStr;
			Spline& spline = m_riverDefSet[i].getSpline();
			for (int j=0; j<spline.getNumControlPoint(); ++j)
			{
				poinsStr += spline.getControlPoint(j).toString() + ' ';
			}
			INDENT; file->printf("\tcontrolPoints=\"%s\"\n", poinsStr.c_str());

			INDENT; file->printf("/>\n");
		}

#undef INDENT
	}

	void  RiverManager::loadRiverInfo(const String& filename, const TiXmlElement* elemNode)
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
			
			if (value == "RiverManager") 
			{
				
			}
			else if (value == "RiverDef")
			{
				addRiverDef(RiverDef());
				RiverDef& riverDef = m_riverDefSet[m_riverDefSet.size()-1];

				if (v = elem->Attribute("name"))
				{
					riverDef.setName(v);
				}

				double d;
				if (elem->Attribute("width", &d))
				{
					riverDef.setWidth(d);
				}

				if (elem->Attribute("depth", &d))
				{
					riverDef.setDepth(d);
				}

				if (v = elem->Attribute("fogColor"))
				{
					Rgba color;
					color.fromString(v);
					riverDef.setFogColor(color);
				}
				
				if (elem->Attribute("intervalLength", &d))
				{
					riverDef.getSpline().setIntervalLength(d);
				}

				int n;
				if (elem->Attribute("closed", &n))
				{
					riverDef.getSpline().setClosed(n != 0);
				}
				
				if (v = elem->Attribute("controlPoints"))
				{
					const char* pCh = v;
					Vector3 point;
					Spline& spline = riverDef.getSpline();
					String token;
					int pass=0;

					for (int i=0; v[i]!='\0'; ++i)
					{
						if (v[i] == ' ')
						{
							if (pass == 0)
							{
								point.x = (float) atof(token.c_str());
								++pass;
							}
							else if (pass == 1)
							{
								point.y = (float) atof(token.c_str());
								++pass;
							}
							else if (pass == 2)
							{
								point.z = (float) atof(token.c_str());
								spline.addControlPoint(point);
								pass = 0;
							}

							token.clear();
						}
						else
						{
							token += v[i];
						}
					}
				}
			}
		} // for
	}

}}

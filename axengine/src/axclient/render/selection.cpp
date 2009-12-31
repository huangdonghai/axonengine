/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/
#include "../private.h"

#define DEFAULT_ID -111111111
#define DEFAULT_NUM_VERTEX (129 * 129)
#define INVALID_POS -1

// clip plane flags: 裁剪区位码
#define LEFT_CLIP 0x0001
#define RIGHT_CLIP 0x0002
#define BOTTOM_CLIP 0x0004
#define TOP_CLIP 0x0008
#define NEAR_CLIP 0x0010
#define FAR_CLIP 0x0020

#ifdef _DEBUG
	#define CheckNumError(num)\
		if ((num) != (num))\
		{\
			Errorf(#num" is invalid!");\
		}	
#else
	#define CheckNumError(num)
#endif

AX_BEGIN_NAMESPACE

void aliasClipLeft (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out);
void aliasClipRight (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out);

void aliasClipTop (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out);
void aliasClipBottom (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out);

void aliasClipNear (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out);
void aliasClipFar (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out);

void resizeSelectionVertexBuffer(SelectionVertex** buffer, int num);

struct SelectionVertex 
{
	float x;		// 裁剪空间x坐标(-1.0f ~ 1.0f)
	float y;		// 裁剪空间y坐标(-1.0f ~ 1.0f)
	float z;		// 裁剪空间z坐标(-1.0f ~ 1.0f)

	Vector3 eyeCoor;// 视坐标系里的坐标

	int flags;	// 区域码信息
};

static SelectionVertex *gSelectionVertexs = new SelectionVertex[DEFAULT_NUM_VERTEX];
static int gNumVertex = DEFAULT_NUM_VERTEX;
static SelectionVertex gAliasVertex[2][8];

static float zNear, zFar;
static float epision = 0.000001f;
static int numCalled = 0;

RenderCamera Selection::m_selectionCamera;

Selection::Selection(void)
	: m_isSelectMode(false)
	, m_idPos(INVALID_POS)
	, m_currentTestId(DEFAULT_ID)
	, m_isActor(false)
{
	
}

Selection::~Selection(void)
{
	SafeDeleteArray(gSelectionVertexs);
}

void Selection::beginSelect(const RenderCamera &view)
{
	m_selectTime = OsUtil::milliseconds();

	m_selectionCamera = view;

	zFar = view.getZfar();
	zNear = view.getZnear();

	m_selectTime = OsUtil::milliseconds();
	m_isSelectMode = true;

	m_selectRecSeq.clear();
}

void Selection::loadSelectId(int id)
{
	m_currentTestId = id;
	m_idPos = INVALID_POS;
	m_isActor = false;
}

void Selection::testEntity(RenderEntity *re)
{
	// 顶点乘以该矩阵后,就是视坐标系里的坐标.
	m_selectModelViewMatrix = m_selectionCamera.getViewMatrix() * re->getModelMatrix();
	m_isActor = true;

	Primitives prims = re->getHitTestPrims();
	Primitives::iterator it;

	for (it = prims.begin(); it != prims.end(); ++it)
	{
		Primitive *prim = *it;

		testPrimitive(prim);
	}
}

void Selection::testPrimitive(Primitive *prim)
{
	if (prim == NULL)
	{
		return ;
	}
	
	// 如果不是Actor,则视矩阵不用乘以模型矩阵
	if (m_isActor == false)
	{
		m_selectModelViewMatrix = m_selectionCamera.getViewMatrix();
	}

	// 判断图元类型并处理
	if (prim->getType() == Primitive::LineType)
	{
		LinePrim *line = static_cast<LinePrim*> (prim);

		testLine(line);
	}
	else if (prim->getType() == Primitive::MeshType)
	{
		MeshPrim *mesh = static_cast<MeshPrim*> (prim);

		testMesh(mesh);
	}
	else if (prim->getType() == Primitive::ChunkType)
	{
		ChunkPrim *chunk = static_cast<ChunkPrim*> (prim);

		testChunk(chunk);
	}
}

void Selection::testPrimitive(Primitive *prim, const AffineMat &matrix)
{
	// 如果不是Actor,则视矩阵不用乘以模型矩阵
	m_selectModelViewMatrix = m_selectionCamera.getViewMatrix() * matrix.toMatrix4();

	// 判断图元类型并处理
	if (prim->getType() == Primitive::LineType)
	{
		LinePrim *line = static_cast<LinePrim*> (prim);

		testLine(line);
	}
	else if (prim->getType() == Primitive::MeshType)
	{
		MeshPrim *mesh = static_cast<MeshPrim*> (prim);

		testMesh(mesh);
	}
	else if (prim->getType() == Primitive::ChunkType)
	{
		ChunkPrim *chunk = static_cast<ChunkPrim*> (prim);

		testChunk(chunk);
	}
}

HitRecords Selection::endSelect()
{
	m_isSelectMode = false;
	m_currentTestId = DEFAULT_ID;
	m_idPos = INVALID_POS;
	m_isActor = false;

	// 检测并调整内存大小 
	++numCalled;

	// 每调用100次,检测一次内存
	if (numCalled >= 100)
	{
		numCalled = 0;

		// 如果内存大于默认, 则重新分配
		if (gNumVertex > DEFAULT_NUM_VERTEX)
		{
			resizeSelectionVertexBuffer(&gSelectionVertexs, DEFAULT_NUM_VERTEX);
		}
	}
	
	// 显示拾取花费的时间
	m_selectTime = OsUtil::milliseconds() - m_selectTime;

	Printf("select time: %d\n", m_selectTime);

	return m_selectRecSeq;
}

void Selection::testLine(const LinePrim *line)
{
	// 转换顶点坐标成为视坐标或归一化坐标
	const DebugVertex *vertexs = line->getVertexesPointer();

	int numVexTranslate = line->getNumVertexes();

	if (numVexTranslate > gNumVertex)
	{
		resizeSelectionVertexBuffer(&gSelectionVertexs, numVexTranslate);
	}

	Vector3 v;
	for (int i=0; i<numVexTranslate; ++i)
	{
		translateToEyeCoor(vertexs[i].xyz, gSelectionVertexs[i].eyeCoor);

		CheckNumError(gSelectionVertexs[i].eyeCoor.x);
		CheckNumError(gSelectionVertexs[i].eyeCoor.y);
		CheckNumError(gSelectionVertexs[i].eyeCoor.z);

		gSelectionVertexs[i].flags = 0;

		// 先判断是否要对Z进行裁剪
		if (gSelectionVertexs[i].eyeCoor.z > -zNear)
		{
			gSelectionVertexs[i].flags |= NEAR_CLIP;
		}
		else if (gSelectionVertexs[i].eyeCoor.z < -zFar)
		{
			gSelectionVertexs[i].flags |= FAR_CLIP;
		}
		else // 不用Z裁剪的顶点直接转换成投影归一化坐标
		{
			translateToProCoor(gSelectionVertexs[i]);
			initVertexFlags(gSelectionVertexs[i]);

			CheckNumError(gSelectionVertexs[i].x);
			CheckNumError(gSelectionVertexs[i].y);
			CheckNumError(gSelectionVertexs[i].z);
		}
	}

	// 分析顶点数据并裁剪图元
	int numIndex = line->getActivedIndexes();

	if (numIndex == 0)
	{
		numIndex = line->getNumIndexes();
	}

	const ushort_t *indexes = line->getIndexPointer();

	for (int i=0; i<numIndex-1; i+=2)
	{
		if (indexes[i] < numVexTranslate && indexes[i+1] < numVexTranslate)
		{
			aliasClipLine(gSelectionVertexs[indexes[i]], gSelectionVertexs[indexes[i+1]]);
		}
	}
}

void Selection::testMesh(const MeshPrim *mesh)
{
	// 转换顶点坐标成为归一化坐标
	const Vertex *vertexs = mesh->getVertexesPointer();

	int numVexTranslate = mesh->getNumVertexes();

	if (numVexTranslate > gNumVertex)
	{
		resizeSelectionVertexBuffer(&gSelectionVertexs, numVexTranslate);
	}

	Vector3 v;
	for (int i=0; i<numVexTranslate; ++i)
	{
		translateToEyeCoor(vertexs[i].xyz, gSelectionVertexs[i].eyeCoor);

		CheckNumError(gSelectionVertexs[i].eyeCoor.x);
		CheckNumError(gSelectionVertexs[i].eyeCoor.y);
		CheckNumError(gSelectionVertexs[i].eyeCoor.z);

		gSelectionVertexs[i].flags = 0;

		// 先判断是否要对Z进行裁剪
		if (gSelectionVertexs[i].eyeCoor.z > -zNear)
		{
			gSelectionVertexs[i].flags |= NEAR_CLIP;
		}
		else if (gSelectionVertexs[i].eyeCoor.z < -zFar)
		{
			gSelectionVertexs[i].flags |= FAR_CLIP;
		}
		else // 不用Z裁剪的顶点直接转换成投影归一化坐标
		{
			translateToProCoor(gSelectionVertexs[i]);

			CheckNumError(gSelectionVertexs[i].x);
			CheckNumError(gSelectionVertexs[i].y);
			CheckNumError(gSelectionVertexs[i].z);

			initVertexFlags(gSelectionVertexs[i]);
		}
	}

	// 分析顶点数据并裁剪图元
	int numIndex = mesh->getActivedIndexes();

	if (numIndex == 0)
	{
		numIndex = mesh->getNumIndexes();
	}

	const ushort_t *indexes = mesh->getIndexPointer();

	// 如果是带状索引
	if (mesh->isStriped())
	{
		for (int i=0; i<numIndex-2; ++i)
		{
			if (indexes[i] < numVexTranslate && indexes[i+1] < numVexTranslate && indexes[i+2] < numVexTranslate)
			{
				if (i % 2 == 0)
				{
					aliasClipTriangle(gSelectionVertexs[indexes[i]], gSelectionVertexs[indexes[i+1]], gSelectionVertexs[indexes[i+2]]);
				}
				else
				{
					aliasClipTriangle(gSelectionVertexs[indexes[i+1]], gSelectionVertexs[indexes[i]], gSelectionVertexs[indexes[i+2]]);
				}
			}
		}
	}
	else
	{
		for (int i=0; i+2<numIndex; i+=3)
		{
			if (indexes[i] < numVexTranslate && indexes[i+1] < numVexTranslate && indexes[i+2] < numVexTranslate)
			{
				aliasClipTriangle(gSelectionVertexs[indexes[i]], gSelectionVertexs[indexes[i+1]], gSelectionVertexs[indexes[i+2]]);
			}
		}
	}
}

void Selection::testChunk(const ChunkPrim *chunk)
{
	// 转换顶点坐标成为视坐标或归一化坐标
	const ChunkVertex *vertexs = chunk->getVertexesPointer();
	int numVexTranslate = chunk->getNumVertexes();

	if (numVexTranslate > gNumVertex)
	{
		resizeSelectionVertexBuffer(&gSelectionVertexs, numVexTranslate);
	}
	
	Vector3 v;
	for (int i=0; i<numVexTranslate; ++i)
	{
		translateToEyeCoor(vertexs[i].xyz, gSelectionVertexs[i].eyeCoor);

		CheckNumError(gSelectionVertexs[i].eyeCoor.x);
		CheckNumError(gSelectionVertexs[i].eyeCoor.y);
		CheckNumError(gSelectionVertexs[i].eyeCoor.z);

		gSelectionVertexs[i].flags = 0;

		// 先判断是否要对Z进行裁剪
		if (gSelectionVertexs[i].eyeCoor.z > -zNear)
		{
			gSelectionVertexs[i].flags |= NEAR_CLIP;
		}
		else if (gSelectionVertexs[i].eyeCoor.z < -zFar)
		{
			gSelectionVertexs[i].flags |= FAR_CLIP;
		}
		else // 不用Z裁剪的顶点直接转换成投影归一化坐标
		{
			translateToProCoor(gSelectionVertexs[i]);

			CheckNumError(gSelectionVertexs[i].x);
			CheckNumError(gSelectionVertexs[i].y);
			CheckNumError(gSelectionVertexs[i].z);
			
			initVertexFlags(gSelectionVertexs[i]);
		}
	}

	// 分析顶点数据并裁剪图元
	int numIndex = chunk->getActivedIndexes();

	if (numIndex == 0)
	{
		numIndex = chunk->getNumIndexes();
	}

	const ushort_t *indexes = chunk->getIndexesPointer();

	for (int i=0; i<numIndex-2; i+=3)
	{
		if (indexes[i] < numVexTranslate && indexes[i+1] < numVexTranslate && indexes[i+2] < numVexTranslate)
		{
			aliasClipTriangle(gSelectionVertexs[indexes[i]], gSelectionVertexs[indexes[i+1]], gSelectionVertexs[indexes[i+2]]);
		}
	}
}

// 对指定的三角形进行裁剪
void Selection::aliasClipTriangle(const SelectionVertex &vertex0, 
	const SelectionVertex &vertex1, const SelectionVertex &vertex2)
{
	// check side first, because most material is one-side
	Vector3 v0(vertex0.x, vertex0.y, vertex0.z);
	Vector3 v1(vertex1.x, vertex1.y, vertex1.z);
	Vector3 v2(vertex2.x, vertex2.y, vertex2.z);

	Vector3 t = v1 - v0;
	Vector3 b = v2 - v1;
	Vector3 n = t ^ b;

	if (n.z > 0)
		return;

	// 如果3个顶点的标志位逻辑与为真, 则该图元完全在裁剪包围盒外
	if (vertex0.flags & vertex1.flags & vertex2.flags)
	{
		return ;
	}
	else if (!(vertex0.flags | vertex1.flags | vertex2.flags))	// 如果3个顶点的标志位逻辑或为假, 则该图元完全在裁剪包围盒内
	{
		HitRecord record;

		record.name = m_currentTestId;

		record.minz = std::min(std::min(vertex0.z, vertex1.z), vertex2.z);
		record.maxz = std::max(std::max(vertex0.z, vertex1.z), vertex2.z);

		addSelectionRecord(record);

		return ;
	}
	else // 根据顶点的空间区位码来裁剪
	{
		int i, k, pingpong = 0;
		unsigned clipflags;

		gAliasVertex[pingpong][0] = vertex0;
		gAliasVertex[pingpong][1] = vertex1;
		gAliasVertex[pingpong][2] = vertex2;

		// 如果线段的一个顶点区位码的某位为1,而另一个顶点的相同位为0,则该线段穿过相应的裁剪边界.
		clipflags = gAliasVertex[pingpong][0].flags | gAliasVertex[pingpong][1].flags | gAliasVertex[pingpong][2].flags;

		// 首先要判断是否有Z裁剪,有的话先在视坐标里处理,然后再转换成投影归一化坐标
		if ((clipflags & NEAR_CLIP) || (clipflags & FAR_CLIP))
		{
			if (clipflags & NEAR_CLIP)
			{
				k = aliasClip(gAliasVertex[0], gAliasVertex[1], NEAR_CLIP, 3, aliasClipNear);

				if (k == 0)
				{
					return;
				}

				pingpong ^= 1;
			}

			//clipflags = gAliasVertex[1][0].flags | gAliasVertex[1][1].flags | gAliasVertex[1][2].flags;

			if (clipflags & FAR_CLIP)
			{
				k = aliasClip(gAliasVertex[0], gAliasVertex[1], FAR_CLIP, 3, aliasClipFar);

				if (k == 0)
				{
					return;
				}

				pingpong ^= 1;
			}

			clipflags = gAliasVertex[pingpong][0].flags | gAliasVertex[pingpong][1].flags | gAliasVertex[pingpong][2].flags;
		}
		else
		{
			pingpong = 0;
			k = 3;		// 初始顶点数
		}

		if (clipflags & LEFT_CLIP)
		{
			k = aliasClip (gAliasVertex[pingpong], gAliasVertex[pingpong ^ 1],
				LEFT_CLIP, k, aliasClipLeft);

			if (k == 0)
			{
				return;
			}

			pingpong ^= 1;
		}

		if (clipflags & RIGHT_CLIP)
		{
			k = aliasClip (gAliasVertex[pingpong], gAliasVertex[pingpong ^ 1],
				RIGHT_CLIP, k, aliasClipRight);

			if (k == 0)
			{
				return;
			}

			pingpong ^= 1;
		}

		if (clipflags & BOTTOM_CLIP)
		{
			k = aliasClip (gAliasVertex[pingpong], gAliasVertex[pingpong ^ 1],
				BOTTOM_CLIP, k, aliasClipBottom);

			if (k == 0)
			{
				return;
			}

			pingpong ^= 1;
		}

		if (clipflags & TOP_CLIP)
		{
			k = aliasClip (gAliasVertex[pingpong], gAliasVertex[pingpong ^ 1],
				TOP_CLIP, k, aliasClipTop);

			if (k == 0)
			{
				return;
			}

			pingpong ^= 1;
		}

		// 存储裁剪结果
		HitRecord record;

		record.maxz = -1.0f;
		record.minz = 1.0f;
		record.name = m_currentTestId;

		for (i=0; i<k; i++)
		{
			record.minz = std::min(gAliasVertex[pingpong][i].z, record.minz);
			record.maxz = std::max(gAliasVertex[pingpong][i].z, record.maxz);
		}

		addSelectionRecord(record);
	}
}

// 线段裁剪
void Selection::aliasClipLine(const SelectionVertex &vertex0, const SelectionVertex &vertex1)
{
	// 如果2个顶点的标志位逻辑与为真, 则该图元完全在裁剪包围盒外
	if (vertex0.flags & vertex1.flags)
	{
		return ;
	}
	else if (!(vertex0.flags | vertex1.flags))	// 如果2个顶点的标志位逻辑或为假, 则该图元完全在裁剪包围盒内
	{
		HitRecord record;

		record.name = m_currentTestId;

		record.minz = std::min(vertex0.z, vertex1.z);
		record.maxz = std::max(vertex0.z, vertex1.z);

		addSelectionRecord(record);

		return ;
	}
	else // 根据顶点的空间区位码来裁剪
	{
		int i, k, pingpong = 0;
		unsigned clipflags;

		gAliasVertex[pingpong][0] = vertex0;
		gAliasVertex[pingpong][1] = vertex1;

		// 如果线段的一个顶点区位码的某位为1,而另一个顶点的相同位为0,则该线段穿过相应的裁剪边界.
		clipflags = gAliasVertex[pingpong][0].flags | gAliasVertex[pingpong][1].flags;

		// 首先要判断是否有Z裁剪,有的话先在视坐标里处理,然后再转换成投影归一化坐标
		if ((clipflags & NEAR_CLIP) || (clipflags & FAR_CLIP))
		{
			if (clipflags & NEAR_CLIP)
			{
				k = aliasClip(gAliasVertex[0], gAliasVertex[1], NEAR_CLIP, 2, aliasClipNear);

				if (k == 0)
				{
					return;
				}

				pingpong ^= 1;
			}

			if (clipflags & FAR_CLIP)
			{
				k = aliasClip(gAliasVertex[0], gAliasVertex[1], FAR_CLIP, 2, aliasClipFar);

				if (k == 0)
				{
					return;
				}

				pingpong ^= 1;
			}

			clipflags = gAliasVertex[pingpong][0].flags | gAliasVertex[pingpong][1].flags;
		}
		else
		{
			pingpong = 0;
			k = 2;		// 初始顶点数
		}

		if (clipflags & LEFT_CLIP)
		{
			k = aliasClip (gAliasVertex[pingpong], gAliasVertex[pingpong ^ 1],
				LEFT_CLIP, k, aliasClipLeft);

			if (k == 0)
			{
				return;
			}

			pingpong ^= 1;
		}

		if (clipflags & RIGHT_CLIP)
		{
			k = aliasClip (gAliasVertex[pingpong], gAliasVertex[pingpong ^ 1],
				RIGHT_CLIP, k, aliasClipRight);

			if (k == 0)
			{
				return;
			}

			pingpong ^= 1;
		}

		if (clipflags & BOTTOM_CLIP)
		{
			k = aliasClip (gAliasVertex[pingpong], gAliasVertex[pingpong ^ 1],
				BOTTOM_CLIP, k, aliasClipBottom);

			if (k == 0)
			{
				return;
			}

			pingpong ^= 1;
		}

		if (clipflags & TOP_CLIP)
		{
			k = aliasClip (gAliasVertex[pingpong], gAliasVertex[pingpong ^ 1],
				TOP_CLIP, k, aliasClipTop);

			if (k == 0)
			{
				return;
			}

			pingpong ^= 1;
		}

		// 存储裁剪结果
		HitRecord record;

		record.maxz = -1.0f;
		record.minz = 1.0f;
		record.name = m_currentTestId;

		for (i=0; i<k; i++)
		{
			record.minz = std::min(gAliasVertex[pingpong][i].z, record.minz);
			record.maxz = std::max(gAliasVertex[pingpong][i].z, record.maxz);
		}

		addSelectionRecord(record);
	}
}

// 将视坐标转换成归一化投影坐标
void Selection::translateToProCoor(SelectionVertex &vexter)
{
	Vector3 v;

	v = m_selectionCamera.getProjMatrix() * vexter.eyeCoor;

	vexter.x = v.x;
	vexter.y = v.y;
	vexter.z = v.z;
}

// 将世界坐标转换成视坐标
void Selection::translateToEyeCoor(const Vector3 &inVertex, Vector3 &outVertex)
{
	outVertex = m_selectModelViewMatrix * inVertex;	// 将模型坐标转换成视坐标
}

// 增加一个记录,但不一定会新增一个记录,也可能是替换原来的记录.
void Selection::addSelectionRecord(const HitRecord &record)
{
	// 如果是第一次, 则新增一个记录
	if (m_idPos == INVALID_POS)
	{
		m_idPos = (int) m_selectRecSeq.size();
		m_selectRecSeq.push_back(record);
	}
	else // 否则比较并替换原有的记录
	{
		if (record.minz < m_selectRecSeq[m_idPos].minz)
		{
			m_selectRecSeq[m_idPos].minz = record.minz;
		}

		if (record.maxz > m_selectRecSeq[m_idPos].maxz)
		{
			m_selectRecSeq[m_idPos].maxz = record.maxz;
		}
	}
}

// 初始化顶点区位码标志.由于Z裁剪在视坐标里已经判断,故不在此处判断
void Selection::initVertexFlags(SelectionVertex &vertex)
{
	vertex.flags = 0;

	if (vertex.x < -1.0f)
	{
		vertex.flags |= LEFT_CLIP;
	}
	else if (vertex.x > 1.0f)
	{
		vertex.flags |= RIGHT_CLIP;
	}

	if (vertex.y < -1.0f)
	{
		vertex.flags |= BOTTOM_CLIP;
	}
	else if (vertex.y > 1.0f)
	{
		vertex.flags |= TOP_CLIP;
	}
}

// 对多边形进行裁剪, count>=2才能正确裁剪.返回裁剪后的顶点数.
int Selection::aliasClip(SelectionVertex *in, SelectionVertex *out, int flag, int count,
		void (*clip) (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out))
{
	int i, j, k;
	int flags, oldflags;

	j = count-1;
	k = 0;

	for (i=0 ; i<count ; j = i, i++)
	{
		oldflags = in[j].flags & flag;
		flags = in[i].flags & flag;

		if (flags && oldflags)
		{
			continue;
		}

		if (oldflags ^ flags)
		{
			// 启用裁剪
			clip(&in[j], &in[i], &out[k]);

			// 初始标志位
			initVertexFlags(out[k]);

			k++;
		}

		if (!flags)
		{
			out[k] = in[i];
			k++;
		}
	}

	return k;	// 返回裁剪后的顶点个数
}

void aliasClipLeft (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out)
{
	if (fabs(pfv0->x - pfv1->x) < epision)
	{
		out->x = -1.0f;
		out->y = pfv0->y;
		out->z = pfv0->z;
		
		return ;
	}

	float scale;

	if (pfv0->x >= pfv1->x)
	{
		scale =(-1.0f - pfv0->x) / (pfv1->x - pfv0->x);

		out->x = -1.0f;
		out->y = pfv0->y + (pfv1->y - pfv0->y) * scale;
		out->z = pfv0->z + (pfv1->z - pfv0->z) * scale;
	}
	else
	{
		scale =(-1.0f - pfv1->x) / (pfv0->x - pfv1->x);

		out->x = -1.0f;
		out->y = pfv1->y + (pfv0->y - pfv1->y) * scale;
		out->z = pfv1->z + (pfv0->z - pfv1->z) * scale;
	}

	CheckNumError(out->x);
	CheckNumError(out->y);
	CheckNumError(out->z);
}

void aliasClipRight (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out)
{
	if (fabs(pfv0->x - pfv1->x) < epision)
	{
		out->x = 1.0f;
		out->y = pfv0->y;
		out->z = pfv0->z;

		return ;
	}

	float scale;

	if (pfv0->x >= pfv1->x)
	{
		scale =(1.0f - pfv0->x) / (pfv1->x - pfv0->x);

		out->x = 1.0f;
		out->y = pfv0->y + (pfv1->y - pfv0->y) * scale;
		out->z = pfv0->z + (pfv1->z - pfv0->z) * scale;
	}
	else
	{
		scale =(1.0f - pfv1->x) / (pfv0->x - pfv1->x);

		out->x = 1.0f;
		out->y = pfv1->y + (pfv0->y - pfv1->y) * scale;
		out->z = pfv1->z + (pfv0->z - pfv1->z) * scale;
	}

	CheckNumError(out->x);
	CheckNumError(out->y);
	CheckNumError(out->z);
}

void aliasClipTop (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out)
{
	if (fabs(pfv0->y - pfv1->y) < epision)
	{
		out->x = pfv0->x;
		out->y = 1.0f;
		out->z = pfv0->z;

		return ;
	}

	float scale;

	if (pfv0->y >= pfv1->y)
	{
		scale =(1.0f - pfv0->y) / (pfv1->y - pfv0->y);

		out->x = pfv0->x + (pfv1->x - pfv0->x) * scale;
		out->y = 1.0f;
		out->z = pfv0->z + (pfv1->z - pfv0->z) * scale;
	}
	else
	{
		scale =(1.0f - pfv1->y) / (pfv0->y - pfv1->y);

		out->x = pfv1->x + (pfv0->x - pfv1->x) * scale;
		out->y = 1.0f;
		out->z = pfv1->z + (pfv0->z - pfv1->z) * scale;
	}

	CheckNumError(out->x);
	CheckNumError(out->y);
	CheckNumError(out->z);
}

void aliasClipBottom (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out)
{
	if (fabs(pfv0->y - pfv1->y) < epision)
	{
		out->x = pfv0->x;
		out->y = -1.0f;
		out->z = pfv0->z;

		return ;
	}

	float scale;

	if (pfv0->y >= pfv1->y)
	{
		scale =(-1.0f - pfv0->y) / (pfv1->y - pfv0->y);

		out->x = pfv0->x + (pfv1->x - pfv0->x) * scale;
		out->y = -1.0f;
		out->z = pfv0->z + (pfv1->z - pfv0->z) * scale;
	}
	else
	{
		scale =(-1.0f - pfv1->y) / (pfv0->y - pfv1->y);

		out->x = pfv1->x + (pfv0->x - pfv1->x) * scale;
		out->y = -1.0f;
		out->z = pfv1->z + (pfv0->z - pfv1->z) * scale;
	}

	CheckNumError(out->x);
	CheckNumError(out->y);
	CheckNumError(out->z);
}

void aliasClipNear (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out)
{
	if (fabs(pfv0->eyeCoor.z - pfv1->eyeCoor.z) < epision)
	{
		out->eyeCoor.x = pfv0->eyeCoor.x;
		out->eyeCoor.y = pfv0->eyeCoor.y;
		out->eyeCoor.z = -zNear;

		Selection::translateToProCoor(*out);
		Selection::initVertexFlags(*out);

		return ;
	}

	float scale;

	if (pfv0->eyeCoor.z >= pfv1->eyeCoor.z)
	{
		scale =(-zNear - pfv0->eyeCoor.z) / (pfv1->eyeCoor.z - pfv0->eyeCoor.z);

		out->eyeCoor.x = pfv0->eyeCoor.x + (pfv1->eyeCoor.x - pfv0->eyeCoor.x) * scale;
		out->eyeCoor.y = pfv0->eyeCoor.y + (pfv1->eyeCoor.y - pfv0->eyeCoor.y) * scale;
		out->eyeCoor.z = -zNear;
	}
	else
	{
		scale =(-zNear - pfv1->eyeCoor.z) / (pfv0->eyeCoor.z - pfv1->eyeCoor.z);

		out->eyeCoor.x = pfv1->eyeCoor.x + (pfv0->eyeCoor.x - pfv1->eyeCoor.x) * scale;
		out->eyeCoor.y = pfv1->eyeCoor.y + (pfv0->eyeCoor.y - pfv1->eyeCoor.y) * scale;
		out->eyeCoor.z = -zNear;
	}

	Selection::translateToProCoor(*out);

	Selection::initVertexFlags(*out);

	CheckNumError(out->x);
	CheckNumError(out->y);
	CheckNumError(out->z);
}

void aliasClipFar (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out)
{
	if (fabs(pfv0->eyeCoor.z - pfv1->eyeCoor.z) < epision)
	{
		out->eyeCoor.x = pfv0->eyeCoor.x;
		out->eyeCoor.y = pfv0->eyeCoor.y;
		out->eyeCoor.z = -zFar;

		Selection::translateToProCoor(*out);
		//Selection::initVertexFlags(*out);

		return ;
	}

	float scale;

	if (pfv0->eyeCoor.z >= pfv1->eyeCoor.z)
	{
		scale =(-zFar - pfv0->eyeCoor.z) / (pfv1->eyeCoor.z - pfv0->eyeCoor.z);

		out->eyeCoor.x = pfv0->eyeCoor.x + (pfv1->eyeCoor.x - pfv0->eyeCoor.x) * scale;
		out->eyeCoor.y = pfv0->eyeCoor.y + (pfv1->eyeCoor.y - pfv0->eyeCoor.y) * scale;
		out->eyeCoor.z = -zFar;
	}
	else
	{
		scale =(-zFar - pfv1->eyeCoor.z) / (pfv0->eyeCoor.z - pfv1->eyeCoor.z);

		out->eyeCoor.x = pfv1->eyeCoor.x + (pfv0->eyeCoor.x - pfv1->eyeCoor.x) * scale;
		out->eyeCoor.y = pfv1->eyeCoor.y + (pfv0->eyeCoor.y - pfv1->eyeCoor.y) * scale;
		out->eyeCoor.z = -zFar;
	}

	Selection::translateToProCoor(*out);

	//Selection::initVertexFlags(*out);

	CheckNumError(out->x);
	CheckNumError(out->y);
	CheckNumError(out->z);
}

// 重新分配顶点buffer的大小
void resizeSelectionVertexBuffer(SelectionVertex** buffer, int num)
{
	AX_ASSERT(num > 0);

	// 记得要先释放
	if (*buffer != NULL)
	{
		//memcpy();
		delete []*buffer;
	}

	*buffer = new SelectionVertex[num];

	gNumVertex = num;
}

AX_END_NAMESPACE
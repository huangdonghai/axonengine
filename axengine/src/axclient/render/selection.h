/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_CLIENT_RENDER_SELECTION_H
#define AX_CLIENT_RENDER_SELECTION_H

AX_BEGIN_NAMESPACE

struct SelectionVertex;
struct Record;

// 三维齐次坐标系中的裁剪, 主要对三角形和线段的裁剪.
// 如果想获得更多的描述, 请参阅<<计算机图形学(第三版)>>中7.11三维裁剪算法一节.

// 裁剪流程: 模型坐标->世界坐标->视坐标->归一化投影坐标->裁剪->返回结果
// 需要注意的是,对于Z裁剪,由于在摄像机后面的Z值投影到归一化坐标后,
// 会得到不正确的结果.所以对Z裁剪要在视坐标里进行判断并裁剪,裁剪后再转换成归一化投影坐标.

class Selection
{

public:
	Selection(void);
	~Selection(void);


	void beginSelect(const RenderCamera& view);
	void loadSelectId(int id);
	void testEntity(RenderEntity* re);
	void testPrimitive(Primitive* prim);
	void testPrimitive(Primitive* prim, const AffineMat& matrix);
	HitRecords endSelect();

	bool isSelectMode()	{return m_isSelectMode;}

	// 将指定的顶点转化成视坐标
	void translateToEyeCoor(const Vector3& inVertex, Vector3& outVertex);
	// 将指定的顶点转换成归一化投影坐标
	static void translateToProCoor(SelectionVertex& vertex);

	// 初始化顶点区位标志位
	static void initVertexFlags(SelectionVertex& vertex);


private:

	void testLine(const LinePrim* line);
	void testMesh(const MeshPrim* mesh);
	void testChunk(const ChunkPrim* chunk);

	void aliasClipTriangle(const SelectionVertex& vertex0,
							const SelectionVertex& vertex1, const SelectionVertex& vertex2);
	void aliasClipLine(const SelectionVertex& vertexStart, const SelectionVertex& vertexEnd);

	int aliasClip(SelectionVertex *in, SelectionVertex *out, int flag, int count,
							void (*clip) (SelectionVertex *pfv0, SelectionVertex *pfv1, SelectionVertex *out));
	
	void addSelectionRecord(const HitRecord &record);


private:

	static RenderCamera m_selectionCamera;
	Matrix4 m_selectModelViewMatrix;

	int m_selectTime;

	bool m_isSelectMode;
	HitRecords m_selectRecSeq;

	int m_currentTestId;	// 当前检测的id
	int m_idPos;			// 存储裁剪结果的位置
	bool m_isActor;
};

AX_END_NAMESPACE

#endif